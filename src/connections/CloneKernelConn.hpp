/*
 * CloneKernelConn.hpp
 *
 *  Created on: May 24, 2011
 *      Author: peteschultz
 */

#ifndef CLONEKERNELCONN_HPP_
#define CLONEKERNELCONN_HPP_

#include "KernelConn.hpp"
#include "../weightinit/InitCloneKernelWeights.hpp"

namespace PV {

class CloneKernelConn : public KernelConn {

public:
   CloneKernelConn();
   CloneKernelConn(const char * name, HyPerCol * hc,
      HyPerLayer * pre, HyPerLayer * post,
      KernelConn * originalConn);
   virtual ~CloneKernelConn();
   int initialize_base();
   int initialize(const char * name, HyPerCol * hc,
      HyPerLayer * pre, HyPerLayer * post,
      KernelConn * originalConn);

   virtual int setPatchSize(const char * filename);
   // For CloneKernelConn, filename is ignored, but we include it
   // to agree with the interface for HyPerConn

   virtual int updateState(double time, double dt);

   virtual int writeWeights(double time, bool last=false){return PV_SUCCESS;};
   virtual int writeWeights(const char * filename){return PV_SUCCESS;};
   virtual int checkpointWrite(const char * cpDir){return PV_SUCCESS;};
   virtual int checkpointRead(const char * cpDir, double *timef){return PV_SUCCESS;};

protected:
   virtual PVPatch *** initializeWeights(PVPatch *** patches, pvdata_t ** dataStart, int numPatches,
            const char * filename);
   virtual int constructWeights(const char * filename);
   void constructWeightsOutOfMemory();
   virtual int createAxonalArbors(int arborId);
   virtual int initNormalize();

   virtual int setParams(PVParams * params);
   virtual void readNumAxonalArborLists(PVParams * params);
   virtual void readPlasticityFlag(PVParams * params);
   virtual void readShrinkPatches(PVParams * params);
   virtual int  readPatchSize(PVParams * params);
   virtual int  readNfp(PVParams * params);

   KernelConn * originalConn;

private:
   int deleteWeights();

}; // end class CloneKernelConn

}  // end namespace PV

#endif /* CLONEKERNELCONN_HPP_ */
