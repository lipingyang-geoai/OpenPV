/*
 * SigmoidLayer.hpp
 * can be used to implement Sigmoid junctions
 *
 *  Created on: May 11, 2011
 *      Author: garkenyon
 */

#ifndef SIGMOIDLAYER_HPP_
#define SIGMOIDLAYER_HPP_

#include "CloneVLayer.hpp"

namespace PV {

// SigmoidLayer can be used to implement Sigmoid junctions between spiking neurons
class SigmoidLayer: public CloneVLayer {
public:
   SigmoidLayer(const char * name, HyPerCol * hc);
   virtual ~SigmoidLayer();
   virtual int communicateInitInfo();
   virtual int allocateDataStructures();
   virtual int updateState(double timef, double dt);
   virtual int setActivity();
protected:
   SigmoidLayer();
   int initialize(const char * name, HyPerCol * hc);
   virtual int setParams(PVParams * params);
   virtual void readVrest(PVParams * params);
   virtual void readVthRest(PVParams * params);
   virtual void readInverseFlag(PVParams * params);
   virtual void readSigmoidFlag(PVParams * params);
   virtual void readSigmoidAlpha(PVParams * params);
   /* static */ int updateState(double timef, double dt, const PVLayerLoc * loc, pvdata_t * A, pvdata_t * V, int num_channels, pvdata_t * gSynHead, float Vth, float V0, float sigmoid_alpha, bool sigmoid_flag, bool inverse_flag, unsigned int * active_indices, unsigned int * num_active);
private:
   int initialize_base();
   float V0;
   float Vth;
   bool  InverseFlag;
   bool  SigmoidFlag;
   float SigmoidAlpha;
   // Use CloneVLayer's originalLayerName and originalLayer member variables
   // char * sourceLayerName;
   // HyPerLayer * sourceLayer;
};

}

#endif /* CLONELAYER_HPP_ */
