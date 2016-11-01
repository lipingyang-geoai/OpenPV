#include "testPvpBatch.hpp"
#include "checkpointing/CheckpointEntryPvp.hpp"
#include "utils/conversions.h"

void testPvpBatch(PV::Communicator *comm, std::string const &directory) {
   PVLayerLoc loc;
   loc.nbatchGlobal = 4;
   loc.nxGlobal     = 16;
   loc.nyGlobal     = 4;
   loc.nf           = 1;
   loc.halo.lt      = 0;
   loc.halo.rt      = 0;
   loc.halo.dn      = 0;
   loc.halo.up      = 0;
   pvErrorIf(
         loc.nbatchGlobal % comm->numCommBatches(),
         "Global batch size %d is not a multiple of batch width %d\n",
         loc.nbatchGlobal,
         comm->numCommBatches());
   loc.nbatch = loc.nbatchGlobal / comm->numCommBatches();
   loc.kb0    = loc.nbatchGlobal * comm->commBatch();
   pvErrorIf(
         loc.nxGlobal % comm->numCommColumns(),
         "Global width %d is not a multiple of the number of MPI columns %d\n",
         loc.nxGlobal,
         comm->numCommColumns());
   loc.nx  = loc.nxGlobal / comm->numCommColumns();
   loc.kx0 = loc.nx * comm->commColumn();
   pvErrorIf(
         loc.nyGlobal % comm->numCommRows(),
         "Global height %d is not a multiple of the number of MPI rows %d\n",
         loc.nyGlobal,
         comm->numCommRows());
   loc.ny  = loc.nyGlobal / comm->numCommRows();
   loc.ky0 = loc.ny * comm->commRow();

   int const localSize = loc.nbatch * loc.nx * loc.ny * loc.nf;
   std::vector<float> correctData(localSize);
   for (int k = 0; k < localSize; k++) {
      int kbatchGlobal = batchIndex(k, loc.nbatch, loc.nx, loc.ny, loc.nf) + loc.kb0;
      int kxGlobal     = kxPos(k, loc.nx, loc.ny, loc.nf) + loc.kx0;
      int kyGlobal     = kyPos(k, loc.nx, loc.ny, loc.nf) + loc.ky0;
      int kf           = featureIndex(k, loc.nx, loc.ny, loc.nf);
      int kGlobal      = kIndexBatch(
            kbatchGlobal,
            kxGlobal,
            kyGlobal,
            kf,
            loc.nbatchGlobal,
            loc.nxGlobal,
            loc.nyGlobal,
            loc.nf);
      correctData.at(k) = (float)kGlobal;
   }

   // Initialize checkpointData as a vector with the same size as correctData.
   // Need to make sure that checkpointData.data() never gets relocated, since the
   // CheckpointEntryPvp's mDataPointer doesn't change with it.
   std::vector<float> checkpointData(correctData.size());
   PV::CheckpointEntryPvp<float> checkpointEntryPvp{"checkpointEntryPvpBatch",
                                                    comm,
                                                    checkpointData.data(),
                                                    &loc,
                                                    false /*not extended*/};

   double const simTime = 10.0;
   // Copy correct data into checkpoint data.
   for (int k = 0; k < localSize; k++) {
      checkpointData.at(k) = correctData.at(k);
   }
   checkpointEntryPvp.write(directory, simTime, false /*not verifying writes*/);

   // Data has now been checkpointed. Change the vector to make sure that checkpointRead is really
   // modifying the data.
   for (auto &a : checkpointData) {
      a = -1.0f;
   }

   // Read the data back
   double readTime = (double)(simTime == 0);
   pvAssert(simTime != readTime);
   checkpointEntryPvp.read(directory, &readTime);

   // Verify the read
   pvErrorIf(readTime != simTime, "Read timestamp %f; expected %f.\n", readTime, simTime);
   for (int k = 0; k < localSize; k++) {
      pvErrorIf(
            checkpointData.at(k) != correctData.at(k),
            "testDataPvp failed: data at rank %d, index %d is %f, but should be %f\n",
            comm->commRank(),
            k,
            (double)checkpointData.at(k),
            (double)correctData.at(k));
   }
   MPI_Barrier(comm->communicator());
   pvInfo() << "testDataPvpBatch passed.\n";
}
