/* CloneConn.cpp
 *
 * Created on: May 23, 2011
 *     Author: peteschultz
 */

#include "CloneConn.hpp"
#include "columns/HyPerCol.hpp"
#include "components/CloneWeightsPair.hpp"
#include "components/DependentArborList.hpp"
#include "components/DependentPatchSize.hpp"
#include "components/DependentSharedWeights.hpp"
#include "delivery/CloneDeliveryFacade.hpp"

namespace PV {

CloneConn::CloneConn(char const *name, HyPerCol *hc) { initialize(name, hc); }

CloneConn::CloneConn() {}

CloneConn::~CloneConn() {}

int CloneConn::initialize(char const *name, HyPerCol *hc) {
   int status = HyPerConn::initialize(name, hc);
   return status;
}

void CloneConn::createComponentTable(char const *description) {
   HyPerConn::createComponentTable(description);
   mOriginalConnNameParam = createOriginalConnNameParam();
   if (mOriginalConnNameParam) {
      addUniqueComponent(mOriginalConnNameParam->getDescription(), mOriginalConnNameParam);
   }
}

BaseDelivery *CloneConn::createDeliveryObject() { return new CloneDeliveryFacade(name, parent); }

ArborList *CloneConn::createArborList() { return new DependentArborList(name, parent); }

PatchSize *CloneConn::createPatchSize() { return new DependentPatchSize(name, parent); }

SharedWeights *CloneConn::createSharedWeights() { return new DependentSharedWeights(name, parent); }

WeightsPairInterface *CloneConn::createWeightsPair() { return new CloneWeightsPair(name, parent); }

InitWeights *CloneConn::createWeightInitializer() { return nullptr; }

NormalizeBase *CloneConn::createWeightNormalizer() { return nullptr; }

BaseWeightUpdater *CloneConn::createWeightUpdater() { return nullptr; }

OriginalConnNameParam *CloneConn::createOriginalConnNameParam() {
   return new OriginalConnNameParam(name, parent);
}

Response::Status CloneConn::initializeState(std::shared_ptr<InitializeStateMessage const> message) {
   return Response::NO_ACTION;
}

} // namespace PV
