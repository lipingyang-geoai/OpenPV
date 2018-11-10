#ifndef SUMPOOLTESTLAYER_HPP_
#define SUMPOOLTESTLAYER_HPP_

#include <layers/HyPerLayer.hpp>

namespace PV {

class SumPoolTestLayer : public HyPerLayer {
  public:
   SumPoolTestLayer(const char *name, HyPerCol *hc);

  protected:
   ActivityComponent *createActivityComponent() override;
}; // end class SumPoolTestLayer

} /* namespace PV */
#endif
