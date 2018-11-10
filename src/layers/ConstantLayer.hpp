/*
 * ConstantLayer.hpp
 *
 *  Created on: Dec 17, 2013
 *      Author: slundquist
 */

#ifndef CONSTANTLAYER_HPP_
#define CONSTANTLAYER_HPP_

#include "HyPerLayer.hpp"

namespace PV {

class ConstantLayer : public HyPerLayer {
  protected:
   /**
    * List of parameters needed from the ConstantLayer class
    * @name HyPerLayer Parameters
    * @{
    */

   /**
    * @brief ConstantLayer does not use triggerLayerName.
    */
   virtual void ioParam_triggerLayerName(enum ParamsIOFlag ioFlag) override;
   /** @} */ // End list of ConstantLayer parameters

  public:
   ConstantLayer(const char *name, HyPerCol *hc);
   virtual ~ConstantLayer();
   virtual bool needUpdate(double simTime, double dt) const override;

  protected:
   ConstantLayer();
   int initialize(const char *name, HyPerCol *hc);

}; // class ConstantLayer

} /* namespace PV */
#endif
