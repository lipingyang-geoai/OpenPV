#ifndef SUMPOOLTESTINPUTLAYER_HPP_
#define SUMPOOLTESTINPUTLAYER_HPP_

#include <layers/HyPerLayer.hpp>

namespace PV {

class SumPoolTestInputLayer : public HyPerLayer {
  public:
   SumPoolTestInputLayer(const char *name, HyPerCol *hc);

  protected:
   ActivityComponent *createActivityComponent() override;
}; // end class SumPoolTestInputLayer

} /* namespace PV */
#endif
