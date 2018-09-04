/*
 * ArborTestForOnesProbe.cpp
 *
 *  Created on: Sep 6, 2011
 *      Author: kpeterson
 */

#include "ArborTestForOnesProbe.hpp"
#include <include/pv_arch.h>
#include <layers/HyPerLayer.hpp>
#include <string.h>
#include <utils/PVLog.hpp>

namespace PV {

ArborTestForOnesProbe::ArborTestForOnesProbe(const char *name, HyPerCol *hc) : StatsProbe() {
   initialize_base();
   initialize(name, hc);
}

ArborTestForOnesProbe::~ArborTestForOnesProbe() {}

int ArborTestForOnesProbe::initialize_base() { return PV_SUCCESS; }

int ArborTestForOnesProbe::initialize(const char *name, HyPerCol *hc) {
   return StatsProbe::initialize(name, hc);
}

Response::Status ArborTestForOnesProbe::outputState(double simTime, double deltaTime) {
   Response::Status status = StatsProbe::outputState(simTime, deltaTime);
   if (status != Response::SUCCESS) {
      return status;
   }
   int const rank    = parent->getCommunicator()->commRank();
   const int rcvProc = 0;
   if (rank != rcvProc) {
      return status;
   }
   if (simTime > 1.0) {
      for (int b = 0; b < mLocalBatchWidth; b++) {
         FatalIf(!((fMin[b] > 0.99f) && (fMin[b] < 1.01f)), "Test failed.\n");
         FatalIf(!((fMax[b] > 0.99f) && (fMax[b] < 1.01f)), "Test failed.\n");
         FatalIf(!((avg[b] > 0.99f) && (avg[b] < 1.01f)), "Test failed.\n");
      }
   }

   return status;
}

} /* namespace PV */
