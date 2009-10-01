#include "imageio.hpp"
#include "io.h"

#include <assert.h>
#include <gdal_priv.h>
#include <ogr_spatialref.h>

#undef DEBUG_OUTPUT

static void copyToLocBuffer(int buf[], LayerLoc * loc)
{
	buf[0] = loc->nx;
	buf[1] = loc->ny;
	buf[2] = loc->nxGlobal;
	buf[3] = loc->nyGlobal;
	buf[4] = loc->kx0;
	buf[5] = loc->ky0;
	buf[6] = loc->nPad;
	buf[7] = loc->nBands;
}

static void copyFromLocBuffer(int buf[], LayerLoc * loc)
{
	loc->nx       = buf[0];
	loc->ny       = buf[1];
	loc->nxGlobal = buf[2];
	loc->nyGlobal = buf[3];
	loc->kx0      = buf[4];
	loc->ky0      = buf[5];
	loc->nPad     = buf[6];
	loc->nBands   = buf[7];
}

int getFileType(const char * filename)
{
   const char * ext = strrchr(filename, '.');
   if (strcmp(ext, ".pvp") == 0) {
      return PVP_FILE_TYPE;
   }
   return 0;
}

/**
 * Calculates location information given processor distribution and the
 * size of the image.  
 *
 * @filename the name of the image file (in)
 * @ic the inter-column communicator (in)
 * @loc location information (inout) (loc->nx and loc->ny are out)
 */
int getImageInfo(const char * filename, PV::Communicator * comm, LayerLoc * loc)
{
   if (getFileType(filename) == PVP_FILE_TYPE) {
      return getImageInfoPVP(filename, comm, loc);
   }
   return getImageInfoGDAL(filename, comm, loc);
}

int getImageInfoPVP(const char * filename, PV::Communicator * comm, LayerLoc * loc)
{
   const int locSize = sizeof(LayerLoc) / sizeof(int);
   int locBuf[locSize];
   int status = 0;

   // LayerLoc should contain 8 ints
   assert(locSize == 8);

   const int nxProcs = comm->numCommColumns();
   const int nyProcs = comm->numCommRows();

   const int icCol = comm->commColumn();
   const int icRow = comm->commRow();

#ifdef DEBUG_OUTPUT
   fprintf(stderr, "[%2d]: nxProcs==%d nyProcs==%d icRow==%d icCol==%d\n",
           comm->commRank(), nxProcs, nyProcs, icRow, icCol);
#endif

   if (comm->commRank() == 0) {
      int numParams, params[NUM_PAR_BYTE_PARAMS];

      FILE * fp = fopen(filename, "rb");
      assert(fp != NULL);

      numParams = pv_read_binary_params(fp, NUM_PAR_BYTE_PARAMS, params);
      fclose(fp);

      assert(numParams == NUM_PAR_BYTE_PARAMS);
      assert(params[INDEX_FILE_TYPE] == PVP_FILE_TYPE);

      const int dataSize = params[INDEX_DATA_SIZE];
      const int dataType = params[INDEX_DATA_TYPE];
//      const int nxProcs  = params[INDEX_NX_PROCS];
//      const int nyProcs  = params[INDEX_NY_PROCS];

      loc->nx       = params[INDEX_NX];
      loc->ny       = params[INDEX_NY];
      loc->nxGlobal = params[INDEX_NX_GLOBAL];
      loc->nyGlobal = params[INDEX_NY_GLOBAL];
      loc->kx0      = params[INDEX_KX0];
      loc->ky0      = params[INDEX_KY0];
      loc->nPad     = params[INDEX_NPAD];
      loc->nBands   = params[INDEX_NBANDS];

      assert(dataSize == 1);
      assert(dataType == PV_BYTE_TYPE);

      copyToLocBuffer(locBuf, loc);
   }

#ifdef PV_USE_MPI
   // broadcast location information
   MPI_Bcast(locBuf, 1+locSize, MPI_INT, 0, comm->communicator());
#endif

   copyFromLocBuffer(locBuf, loc);

   // fix up layer indices
   loc->kx0 = loc->nx * icCol;
   loc->ky0 = loc->ny * icRow;

   return status;
}

int getImageInfoGDAL(const char * filename, PV::Communicator * comm, LayerLoc * loc)
{
   const int locSize = sizeof(LayerLoc) / sizeof(int);
   int locBuf[locSize];
   int status = 0;

   // LayerLoc should contain 8 ints
   assert(locSize == 8);

   const int nxProcs = comm->numCommColumns();
   const int nyProcs = comm->numCommRows();

   const int icCol = comm->commColumn();
   const int icRow = comm->commRow();

#ifdef DEBUG_OUTPUT
   fprintf(stderr, "[%2d]: nxProcs==%d nyProcs==%d icRow==%d icCol==%d\n",
           comm->commRank(), nxProcs, nyProcs, icRow, icCol);
#endif

   if (comm->commRank() == 0) {
      GDALAllRegister();

      GDALDataset * dataset = (GDALDataset *) GDALOpen(filename, GA_ReadOnly);
      if (dataset == NULL) return 1;

      int xImageSize = dataset->GetRasterXSize();
      int yImageSize = dataset->GetRasterYSize();

      loc->nBands = dataset->GetRasterCount();

      // calculate local layer size
   
      int nx = xImageSize / nxProcs;
      int ny = yImageSize / nyProcs;

      loc->nx = nx;
      loc->ny = ny;

      loc->nxGlobal = nxProcs * nx;
      loc->nyGlobal = nyProcs * ny;

      copyToLocBuffer(locBuf, loc);

      GDALClose(dataset);
   }

#ifdef PV_USE_MPI
   // broadcast location information
   MPI_Bcast(locBuf, 1+locSize, MPI_INT, 0, comm->communicator());
#endif

   copyFromLocBuffer(locBuf, loc);

   // fix up layer indices
   loc->kx0 = loc->nx * icCol;
   loc->ky0 = loc->ny * icRow;

   return status;
}

/**
 * @filename
 */
int gatherImageFile(const char * filename,
                    PV::Communicator * comm, LayerLoc * loc, unsigned char * buf)
{
   int status = 0;
   const int maxBands = 3;

   const int nxProcs = comm->numCommColumns();
   const int nyProcs = comm->numCommRows();

   const int icRank = comm->commRank();

   const int nx = loc->nx;
   const int ny = loc->ny;

   const int numBands = loc->nBands;
   assert(numBands <= maxBands);

   const int nxny = nx * ny;

#ifdef PV_USE_MPI
   const int tag = 14;
   const int numTotal = nxny * numBands;
   const MPI_Comm mpi_comm = comm->communicator();
#endif // PV_USE_MPI

   if (icRank > 0) {
#ifdef PV_USE_MPI
      const int dest = 0;

      for (int b = 0; b < numBands; b++) {
         MPI_Send(&buf[b*nxny], nx*ny, MPI_FLOAT, dest, tag, mpi_comm);
      }
#ifdef DEBUG_OUTPUT
      fprintf(stderr, "[%2d]: gather: sent to 0, nx==%d ny==%d size==%d\n",
              comm->commRank(), nx, ny, nx*ny);
#endif
#endif // PV_USE_MPI
   }
   else {
      GDALAllRegister();

      char ** metadata;

      GDALDriver * driver = GetGDALDriverManager()->GetDriverByName("GTiff");

      if( driver == NULL )
          exit( 1 );

      metadata = driver->GetMetadata();
      if( CSLFetchBoolean( metadata, GDAL_DCAP_CREATE, FALSE ) ) {
          // printf("Driver %s supports Create() method.\n", "GTiff");
      }

      GDALDataset * dataset;
      char ** options = NULL;

      int xImageSize = nx * nxProcs;
      int yImageSize = ny * nyProcs;

      dataset = driver->Create(filename, xImageSize, yImageSize, numBands,
                               GDT_Byte, options);

      if (dataset == NULL) {
          fprintf(stderr, "[%2d]: gather: failed to open file %s\n", comm->commRank(), filename);
      }
      else {
#ifdef DEBUG_OUTPUT
          fprintf(stderr, "[%2d]: gather: opened file %s\n", comm->commRank(), filename);
#endif
      }

//      double adfGeoTransform[6] = { 444720, 30, 0, 3751320, 0, -30 };
//      OGRSpatialReference oSRS;
//      char *pszSRS_WKT = NULL;

//      dataset->SetGeoTransform( adfGeoTransform );

//      oSRS.SetUTM( 11, TRUE );
//      oSRS.SetWellKnownGeogCS( "NAD27" );
//      oSRS.exportToWkt( &pszSRS_WKT );
//      dataset->SetProjection( pszSRS_WKT );
//      CPLFree( pszSRS_WKT );

      GDALRasterBand * band[maxBands];

      assert(numBands <= dataset->GetRasterCount());

      for (int b = 0; b < numBands; b++) {
         band[b] = dataset->GetRasterBand(b+1);
      }

      // write local image portion
      for (int b = 0; b < numBands; b++) {
         band[b]->RasterIO(GF_Write, 0, 0, nx, ny,
                           &buf[b*nxny], nx, ny, GDT_Float32, 0, 0);
      }

#ifdef PV_USE_MPI
      int src = -1;
      for (int py = 0; py < nyProcs; py++) {
         for (int px = 0; px < nxProcs; px++) {
            if (++src == 0) continue;
            int kx = nx * px;
            int ky = ny * py;
#ifdef DEBUG_OUTPUT
            fprintf(stderr, "[%2d]: gather: receiving from %d xSize==%d"
                    " ySize==%d size==%d total==%d\n",
                    comm->commRank(), src, nx, ny, numTotal,
                    numTotal*comm->commSize());
#endif
            for (int b = 0; b < numBands; b++) {
               MPI_Recv(&buf[b*nxny], numTotal, MPI_FLOAT, src, tag, mpi_comm, MPI_STATUS_IGNORE);
               band[b]->RasterIO(GF_Write, kx, ky, nx, ny,
                                 &buf[b*nxny], nx, ny, GDT_Float32, 0, 0);
            }
         }
      }
#endif // PV_USE_MPI
      GDALClose(dataset);
   }

   return status;
}

/**
 * @filename
 */
int scatterImageFile(const char * filename,
                     PV::Communicator * comm, LayerLoc * loc, unsigned char * buf)
{
   int status = 0;
   const int maxBands = 3;

   const int nxProcs = comm->numCommColumns();
   const int nyProcs = comm->numCommRows();

   const int icRank = comm->commRank();

   const int nx = loc->nx;
   const int ny = loc->ny;

   const int numBands = loc->nBands;
   assert(numBands <= maxBands);

   const int nxny     = nx * ny;

   if (icRank > 0) {
#ifdef PV_USE_MPI
      const int numTotal = nxny * numBands;

      const int src = 0;
      const int tag = 13;
      const MPI_Comm mpi_comm = comm->communicator();

      for (int b = 0; b < numBands; b++) {
         MPI_Recv(&buf[b*nxny], numTotal, MPI_FLOAT, src, tag, mpi_comm, MPI_STATUS_IGNORE);
      }
#ifdef DEBUG_OUTPUT
      fprintf(stderr, "[%2d]: scatter: received from 0, nx==%d ny==%d size==%d\n",
              comm->commRank(), nx, ny, numTotal);
#endif
#endif // PV_USE_MPI
   }
   else {
      GDALAllRegister();

      GDALDataset * dataset = (GDALDataset *) GDALOpen(filename, GA_ReadOnly);

      int xImageSize = dataset->GetRasterXSize();
      int yImageSize = dataset->GetRasterYSize();

      int xTotalSize = nx * nxProcs;
      int yTotalSize = ny * nyProcs;

      if (xTotalSize > xImageSize || yTotalSize > yImageSize) {
         fprintf(stderr, "[ 0]: scatterImageFile: image size too small, "
                 "xTotalSize==%d xImageSize==%d yTotalSize==%d yImageSize==%d\n",
                 xTotalSize, xImageSize, yTotalSize, yImageSize);
         fprintf(stderr, "[ 0]: xSize==%d ySize==%d nxProcs==%d nyProcs==%d\n",
                 nx, ny, nxProcs, nyProcs);
         GDALClose(dataset);
         return -1;
      }

      GDALRasterBand * band[maxBands];

      assert(numBands <= dataset->GetRasterCount());

      for (int b = 0; b < numBands; b++) {
         band[b] = dataset->GetRasterBand(b+1);
      }

#ifdef PV_USE_MPI
      int dest = -1;
      const int tag = 13;
      const MPI_Comm mpi_comm = comm->communicator();

      for (int py = 0; py < nyProcs; py++) {
         for (int px = 0; px < nxProcs; px++) {
            if (++dest == 0) continue;
            int kx = nx * px;
            int ky = ny * py;
#ifdef DEBUG_OUTPUT
            fprintf(stderr, "[%2d]: scatter: sending to %d xSize==%d"
                    " ySize==%d size==%d total==%d\n",
                    comm->commRank(), dest, nx, ny, nx*ny,
                    nx*ny*comm->commSize());
#endif
            for (int b = 0; b < numBands; b++) {
               band[b]->RasterIO(GF_Read, kx, ky, nx, ny,
                                 &buf[b*nxny], nx, ny, GDT_Float32, 0, 0);
               MPI_Send(&buf[b*nxny], nx*ny, MPI_FLOAT, dest, tag, mpi_comm);
            }
         }
      }
#endif // PV_USE_MPI

      // get local image portion
      for (int b = 0; b < numBands; b++) {
         band[b]->RasterIO(GF_Read, 0, 0, nx, ny,
                           &buf[b*nxny], nx, ny, GDT_Float32, 0, 0);
      }
      GDALClose(dataset);
   }

   return status;
}

int scatterImageBlocks(const char* filename,
                       PV::Communicator * comm, LayerLoc * loc, float * buf)
{
   int status = 0;
#ifdef UNIMPLEMENTED

   const MPI_Comm icComm = comm->communicator();

   const int nxProcs = comm->numCommColumns();
   const int nyProcs = comm->numCommRows();

   const int icRank = comm->commRank();
   const int icCol  = comm->commColumn();
   const int icRow  = comm->commRow();

   const int nx = loc->nx;
   const int ny = loc->ny;

   const int nxGlobal = loc->nxGlobal;
   const int nyBorder = loc->nyBorder;

   const int xSize = nx + 2 * nxGlobal;
   const int ySize = ny + 2 * nyBorder;

   if (icRank > 0) {
   }
   else {
      int nxBlocks, nyBlocks, nxBlockSize, nyBlockSize;
      int ixBlock, iyBlock;

      GDALAllRegister();

      GDALDataset * dataset = (GDALDataset *) GDALOpen(filename, GA_ReadOnly);
      GDALRasterBand * band = dataset->GetRasterBand(1);

      CPLAssert(band->GetRasterDataType() == GDT_Byte);

      band->GetBlockSize(&nxBlockSize, &nyBlockSize);
      nxBlocks = (band->GetXSize() + nxBlockSize - 1) / nxBlockSize;
      nyBlocks = (band->GetYSize() + nyBlockSize - 1) / nyBlockSize;

      GByte * data = (GByte *) CPLMalloc(nxBlockSize * nyBlockSize);

      fprintf(stderr, "[ 0]: nxBlockSize==%d nyBlockSize==%d"
              " nxBlocks==%d nyBlocks==%d\n",
              nxBlockSize, nyBlockSize, nxBlocks, nyBlocks);

      for (iyBlock = 0; iyBlock < nyBlocks; iyBlock++) {
         for (ixBlock = 0; ixBlock < nxBlocks; ixBlock++) {
            int nxValid, nyValid;
            band->ReadBlock(ixBlock, ixBlock, data);
         }
      }
   }
#endif

   return status;
}

int gatherParByteFile (const char * filename,
                       PV::Communicator * comm, LayerLoc * loc, unsigned char * buf)
{
   int status = 0;
   const int maxBands = 3;

   const int nxProcs = comm->numCommColumns();
   const int nyProcs = comm->numCommRows();

   const int icRank = comm->commRank();

   const int nx = loc->nx;
   const int ny = loc->ny;

   const int numBands = loc->nBands;
   assert(numBands <= maxBands);

   const int nxny     = nx * ny;
   const int numItems = nxny * numBands;

#ifdef PV_USE_MPI
   const int tag = PVP_FILE_TYPE;
   const MPI_Comm mpi_comm = comm->communicator();
#endif // PV_USE_MPI

   if (icRank > 0) {
#ifdef PV_USE_MPI
      const int dest = 0;
      MPI_Send(buf, numItems, MPI_BYTE, dest, tag, mpi_comm);
#ifdef DEBUG_OUTPUT
      fprintf(stderr, "[%2d]: gather: sent to 0, nx==%d ny==%d size==%d\n",
              comm->commRank(), nx, ny, nx*ny);
#endif
#endif // PV_USE_MPI
   }
   else {
      int params[NUM_PAR_BYTE_PARAMS];

      const int numParams  = NUM_PAR_BYTE_PARAMS;
      const int headerSize = numParams * sizeof(int);
      const int recordSize = numItems * sizeof(unsigned char);

      FILE * fp = fopen(filename, "wb");

      params[INDEX_HEADER_SIZE] = headerSize;
      params[INDEX_NUM_PARAMS]  = numParams;
      params[INDEX_FILE_TYPE]   = PVP_FILE_TYPE;
      params[INDEX_NX]          = loc->nx;
      params[INDEX_NY]          = loc->ny;
      params[INDEX_NF]          = 1;
      params[INDEX_NUM_RECORDS] = nxProcs * nyProcs;
      params[INDEX_RECORD_SIZE] = recordSize;
      params[INDEX_DATA_SIZE]   = sizeof(unsigned char);
      params[INDEX_DATA_TYPE]   = PV_BYTE_TYPE;
      params[INDEX_NX_PROCS]    = nxProcs;
      params[INDEX_NY_PROCS]    = nyProcs;
      params[INDEX_NX_GLOBAL]   = loc->nxGlobal;
      params[INDEX_NY_GLOBAL]   = loc->nyGlobal;
      params[INDEX_KX0]         = loc->kx0;
      params[INDEX_KY0]         = loc->ky0;
      params[INDEX_NPAD]        = loc->nPad;
      params[INDEX_NBANDS]      = loc->nBands;

      int numWrite = fwrite(params, sizeof(int), numParams, fp);
      assert(numWrite == numParams);

      // write local image portion
      fseek(fp, (long) headerSize, SEEK_SET);
      numWrite = fwrite(buf, sizeof(unsigned char), numItems, fp);
      assert(numWrite == numItems);

#ifdef PV_USE_MPI
      int src = -1;
      for (int py = 0; py < nyProcs; py++) {
         for (int px = 0; px < nxProcs; px++) {
            if (++src == 0) continue;
#ifdef DEBUG_OUTPUT
            fprintf(stderr, "[%2d]: gather: receiving from %d xSize==%d"
                    " ySize==%d size==%d total==%d\n",
                    comm->commRank(), src, nx, ny, numTotal,
                    numTotal*comm->commSize());
#endif
            MPI_Recv(buf, numItems, MPI_BYTE, src, tag, mpi_comm, MPI_STATUS_IGNORE);

            long offset = headerSize + src * recordSize;
            fseek(fp, offset, SEEK_SET);
            numWrite = fwrite(buf, sizeof(unsigned char), numItems, fp);
            assert(numWrite == numItems);
         }
      }
#endif // PV_USE_MPI

      status = fclose(fp);
   }

   return status;
}

int scatterParByteFile(const char * filename,
                       PV::Communicator * comm, LayerLoc * loc, unsigned char * buf)
{
   int status = 0;
   const int maxBands = 3;

   const int icRank = comm->commRank();

   const int nx = loc->nx;
   const int ny = loc->ny;

   const int numBands = loc->nBands;
   assert(numBands <= maxBands);

   const int nxny     = nx * ny;
   const int numItems = nxny * numBands;

   if (icRank > 0) {
#ifdef PV_USE_MPI
      const int src = 0;
      const int tag = PVP_FILE_TYPE;
      const MPI_Comm mpi_comm = comm->communicator();

      MPI_Recv(buf, numItems, MPI_BYTE, src, tag, mpi_comm, MPI_STATUS_IGNORE);

#ifdef DEBUG_OUTPUT
      fprintf(stderr, "[%2d]: scatter: received from 0, nx==%d ny==%d size==%d\n",
              comm->commRank(), nx, ny, numTotal);
#endif
#endif // PV_USE_MPI
   }
   else {
      int params[NUM_PAR_BYTE_PARAMS];
      int numParams, numRead, type, nxIn, nyIn, nfIn;

      FILE * fp = pv_open_binary(filename, &numParams, &type, &nxIn, &nyIn, &nfIn);
      assert(fp != NULL);
      assert(numParams == NUM_PAR_BYTE_PARAMS);
      assert(type      == PVP_FILE_TYPE);

      status = pv_read_binary_params(fp, numParams, params);
      assert(status == numParams);

      const size_t headerSize = (size_t) params[INDEX_HEADER_SIZE];
      const size_t recordSize = (size_t) params[INDEX_RECORD_SIZE];

      const int numRecords = params[INDEX_NUM_RECORDS];
      const int dataSize = params[INDEX_DATA_SIZE];
      const int dataType = params[INDEX_DATA_TYPE];
      const int nxProcs  = params[INDEX_NX_PROCS];
      const int nyProcs  = params[INDEX_NY_PROCS];

      loc->nx       = params[INDEX_NX];
      loc->ny       = params[INDEX_NY];
      loc->nxGlobal = params[INDEX_NX_GLOBAL];
      loc->nyGlobal = params[INDEX_NY_GLOBAL];
      loc->kx0      = params[INDEX_KX0];
      loc->ky0      = params[INDEX_KY0];
      loc->nPad     = params[INDEX_NPAD];
      loc->nBands   = params[INDEX_NBANDS];

      assert(dataSize == 1);
      assert(dataType == PV_BYTE_TYPE);
      assert(nxProcs == comm->numCommColumns());
      assert(nyProcs == comm->numCommRows());

#ifdef PV_USE_MPI
      int dest = -1;
      const int tag = PVP_FILE_TYPE;
      const MPI_Comm mpi_comm = comm->communicator();

      for (int py = 0; py < nyProcs; py++) {
         for (int px = 0; px < nxProcs; px++) {
            if (++dest == 0) continue;

#ifdef DEBUG_OUTPUT
            fprintf(stderr, "[%2d]: scatter: sending to %d xSize==%d"
                    " ySize==%d size==%d total==%d\n",
                    comm->commRank(), dest, nx, ny, nx*ny,
                    nx*ny*comm->commSize());
#endif
            long offset = headerSize + dest * recordSize;
            fseek(fp, offset, SEEK_SET);
            numRead = fread(buf, sizeof(unsigned char), numItems, fp);
            assert(numRead == numItems);
            MPI_Send(buf, numItems, MPI_BYTE, dest, tag, mpi_comm);
         }
      }
#endif // PV_USE_MPI

      // get local image portion
      fseek(fp, (long) headerSize, SEEK_SET);
      numRead = fread(buf, sizeof(unsigned char), numItems, fp);
      assert(numRead == numItems);

      status = pv_close_binary(fp);
   }

   return status;
}

/**
 * gather relevant portions of buf on root process from all others
 *    NOTE: buf is np times larger on root process
 */
int gather (PV::Communicator * comm, LayerLoc * loc, float * buf)
{
   return -1;
}

/**
 * scatter relevant portions of buf from root process to all others
 *    NOTE: buf is np times larger on root process
 */
int scatter(PV::Communicator * comm, LayerLoc * loc, float * buf)
{
   return -1;
}

int writeWithBorders(const char * filename, LayerLoc * loc, float * buf)
{
   int X = loc->nx + 2 * loc->nPad;
   int Y = loc->ny + 2 * loc->nPad;
   int B = loc->nBands;

   GDALDriver * driver = GetGDALDriverManager()->GetDriverByName("GTiff");
   GDALDataset* layer_file = driver->Create(filename, X, Y, B, GDT_Byte, NULL);

   // TODO - add multiple raster bands
   GDALRasterBand * band = layer_file->GetRasterBand(1);

   band->RasterIO(GF_Write, 0, 0, X, Y, buf, X, Y, GDT_Float32, 0, 0);

   GDALClose(layer_file);

   return 0;
}
