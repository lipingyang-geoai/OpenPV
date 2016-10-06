/*
 * pv.cpp
 *
 */

#include "FailBeforeExpectedStartTimeLayer.hpp"
#include "columns/HyPerCol.hpp"
#include "columns/PV_Init.hpp"
#include "copyOutput.hpp"
#include "utils/PVLog.hpp"
#include <vector>

using namespace PV;

int main(int argc, char *argv[]) {
   int status = PV_SUCCESS;
   PV_Init pv_init{&argc, &argv, false /*do not allow unrecognized arguments*/};
   status = pv_init.registerKeyword(
         "FailBeforeExpectedStartTimeLayer", Factory::create<FailBeforeExpectedStartTimeLayer>);
   pvErrorIf(status != PV_SUCCESS, "Unable to add FailBeforeExpectedStartTimeLayer\n");
   if (pv_init.getCheckpointReadDir() != nullptr) {
      if (pv_init.getCommunicator()->commRank() == 0) {
         pvErrorNoExit() << argv[0] << " cannot be run with the -c argument.\n";
      }
      status = PV_FAILURE;
   }
   if (pv_init.getRestartFlag()) {
      if (pv_init.getCommunicator()->commRank() == 0) {
         pvErrorNoExit() << argv[0] << " cannot be run with the -r flag.\n";
      }
      status = PV_FAILURE;
   }
   if (status != PV_SUCCESS) {
      MPI_Barrier(pv_init.getCommunicator()->communicator());
      exit(EXIT_FAILURE);
   }

   HyPerCol *hc;
   FailBeforeExpectedStartTimeLayer *outputLayer;

   hc = createHyPerCol(&pv_init);
   pvErrorIf(hc == nullptr, "failed to create HyPerCol.\n");
   outputLayer = dynamic_cast<FailBeforeExpectedStartTimeLayer *>(hc->getLayerFromName("Output"));
   pvErrorIf(
         outputLayer == nullptr,
         "Params file does not have a FailBeforeExpectedStartTimeLayer called \"Output\".\n");
   outputLayer->setExpectedStartTime(0.0);
   status = hc->run(0.0, 10.0, 1.0);
   pvErrorIf(status != PV_SUCCESS, "HyPerCol::run failed with arguments (0.0, 10.0, 1.0).\n");
   std::vector<pvdata_t> withoutRestart = copyOutput(outputLayer);
   delete hc;

   hc = createHyPerCol(&pv_init);
   pvErrorIf(hc == nullptr, "failed to create HyPerCol.\n");
   outputLayer = dynamic_cast<FailBeforeExpectedStartTimeLayer *>(hc->getLayerFromName("Output"));
   pvErrorIf(
         outputLayer == nullptr,
         "Params file does not have a FailBeforeExpectedStartTimeLayer called \"Output\".\n");
   outputLayer->setExpectedStartTime(0.0);
   status = hc->run(0.0, 5.0, 1.0);
   pvErrorIf(status != PV_SUCCESS, "HyPerCol::run failed with arguments (0.0, 5.0, 1.0).\n");
   delete hc;

   pv_init.setRestartFlag(true);
   hc          = createHyPerCol(&pv_init);
   outputLayer = dynamic_cast<FailBeforeExpectedStartTimeLayer *>(hc->getLayerFromName("Output"));
   pvErrorIf(
         outputLayer == nullptr,
         "Params file does not have a FailBeforeExpectedStartTimeLayer called \"Output\".\n");
   outputLayer->setExpectedStartTime(6.0);
   pvErrorIf(hc == nullptr, "failed to create HyPerCol.\n");
   status = hc->run();
   pvErrorIf(status != PV_SUCCESS, "HyPerCol::run failed with restart flag set to true.\n");
   std::vector<pvdata_t> afterRestart = copyOutput(outputLayer);
   delete hc;

   size_t const numNeurons = afterRestart.size();
   pvErrorIf(
         withoutRestart.size() != numNeurons,
         "Number of neurons differ between runs (%zu versus %zu)\n",
         withoutRestart.size(),
         numNeurons);

   for (size_t n = 0; n < numNeurons; n++) {
      if (afterRestart.at(n) != withoutRestart.at(n)) {
         pvErrorNoExit() << "Index " << n
                         << ": values differ (without restart: " << withoutRestart.at(n)
                         << ", after restart: " << afterRestart.at(n) << "\n";
         status = PV_FAILURE;
      }
   }

   return status == PV_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
}
