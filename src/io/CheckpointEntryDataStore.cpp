/*
 * CheckpointEntry.cpp
 *
 *  Created on Sep 27, 2016
 *      Author: Pete Schultz
 */

#include "CheckpointEntryDataStore.hpp"
#include "structures/Buffer.hpp"
#include "utils/BufferUtilsMPI.hpp"
#include "utils/BufferUtilsPvp.hpp"

namespace PV {

void CheckpointEntryDataStore::initialize(DataStore *dataStore, PVLayerLoc const *layerLoc) {
   mDataStore = dataStore;
   mLayerLoc = layerLoc;
}

void CheckpointEntryDataStore::write(std::string const &checkpointDirectory, double simTime, bool verifyWritesFlag) const {
   int const numBuffers = mDataStore->getNumBuffers();
   int const numLevels = mDataStore->getNumLevels();
   FileStream *fileStream = nullptr;
   if (getCommunicator()->commRank() == 0) {
      std::string path = generatePath(checkpointDirectory, "pvp");
      fileStream = new FileStream(path.c_str(), std::ios_base::out, verifyWritesFlag);
      int const numBands = numBuffers * numLevels;
      int *params      = pvp_set_nonspiking_act_params(
            getCommunicator(), simTime, mLayerLoc, PV_FLOAT_TYPE, numLevels);
      pvAssert(params && params[1] == NUM_BIN_PARAMS);
      fileStream->write(params, params[0]);
   }
   int const nxExt = mLayerLoc->nx + mLayerLoc->halo.lt + mLayerLoc->halo.rt;
   int const nyExt = mLayerLoc->ny + mLayerLoc->halo.dn + mLayerLoc->halo.up;
   int const nf = mLayerLoc->nf;
   int const numElements = nxExt * nyExt * nf;
   for (int b = 0; b < numBuffers; b++) {
      for (int l = 0; l < numLevels; l++) {
         double lastUpdateTime = mDataStore->getLastUpdateTime(b, l);
         pvdata_t const *localData = mDataStore->buffer(b, l);
         Buffer<pvdata_t> localPvpBuffer = Buffer<pvdata_t>{localData, nxExt, nyExt, nf};
         Buffer<pvdata_t> globalPvpBuffer = BufferUtils::gather<pvdata_t>(getCommunicator(), localPvpBuffer, mLayerLoc->nx, mLayerLoc->ny);
         if (fileStream) { BufferUtils::writeFrame(*fileStream, &globalPvpBuffer, lastUpdateTime); }
      }
   }
   delete fileStream;
}

void CheckpointEntryDataStore::read(std::string const &checkpointDirectory, double *simTimePtr) const {
   int const numBuffers = mDataStore->getNumBuffers();
   int const numLevels = mDataStore->getNumLevels();
   int const numBands = numBuffers * numLevels;
   FileStream *fileStream = nullptr;
   if (getCommunicator()->commRank() == 0) {
      std::string path = generatePath(checkpointDirectory, "pvp");
      fileStream = new FileStream(path.c_str(), std::ios_base::in, false);
      std::vector<int> header = BufferUtils::readHeader(*fileStream);
      pvErrorIf(header.at(INDEX_NBANDS)!=numBands,
            "readDataStoreFromFile error reading \"%s\": delays*batchwidth in file is %d, "
            "but delays*batchwidth in layer is %d\n",
            path,
            header.at(INDEX_NBANDS),
            numBands);
   }
   int const nxExtGlobal = mLayerLoc->nxGlobal + mLayerLoc->halo.lt + mLayerLoc->halo.rt;
   int const nyExtGlobal = mLayerLoc->nyGlobal + mLayerLoc->halo.dn + mLayerLoc->halo.up;
   int const nf = mLayerLoc->nf;
   Buffer<pvdata_t> pvpBuffer;
   std::vector<double> updateTimes;
   updateTimes.resize(numBands);
   for (int b = 0; b < numBuffers; b++) {
      for (int l = 0; l < numLevels; l++) {
         if (fileStream) {
            pvpBuffer.resize(nxExtGlobal, nyExtGlobal, nf);
            double updateTime = BufferUtils::readFrame(*fileStream, &pvpBuffer);
            updateTimes.at(b*numLevels + l) = updateTime;
         }
         else {
            int const nxExtLocal = mLayerLoc->nx + mLayerLoc->halo.lt + mLayerLoc->halo.rt;
            int const nyExtLocal = mLayerLoc->ny + mLayerLoc->halo.dn + mLayerLoc->halo.up;
            pvpBuffer.resize(nxExtLocal, nyExtLocal, nf);
         }
         BufferUtils::scatter(getCommunicator(), pvpBuffer, mLayerLoc->nx, mLayerLoc->ny);
         pvdata_t *localData = mDataStore->buffer(b, l);
         memcpy(localData, pvpBuffer.asVector().data(), (std::size_t) pvpBuffer.getTotalElements()*sizeof(pvdata_t));
      }
   }
   MPI_Bcast(updateTimes.data(), numBands, MPI_DOUBLE, 0 /*root*/, getCommunicator()->communicator());
   for (int b = 0; b < numBuffers; b++) {
      for (int l = 0; l < numLevels; l++) {
         mDataStore->setLastUpdateTime(b, l, updateTimes[b*numLevels + l]);
      }
   }
   delete fileStream;
}

void CheckpointEntryDataStore::remove(std::string const &checkpointDirectory) const {
   deleteFile(checkpointDirectory, "pvp");
}

}  // end namespace PV
