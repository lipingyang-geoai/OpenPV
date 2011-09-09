/*
 * ConnectionProbe.hpp
 *
 *  Created on: Apr 25, 2009
 *      Author: rasmussn
 */

#ifndef CONNECTIONPROBE_HPP_
#define CONNECTIONPROBE_HPP_

#include "../connections/HyPerConn.hpp"

namespace PV {

class ConnectionProbe {
public:
   ConnectionProbe(int kPre, int arbID=0);
   ConnectionProbe(int kxPre, int kyPre, int kfPre, int arbID=0);
   ConnectionProbe(const char * filename, HyPerCol * hc, int kPre, int arbID=0);
   ConnectionProbe(const char * filename, HyPerCol * hc, int kxPre, int kyPre, int kfPre, int arbID=0);
   virtual ~ConnectionProbe();

   virtual int outputState(float time, HyPerConn * c);

   static int text_write_patch(FILE * fd, PVPatch * patch, float * data);

   static int write_patch_indices(FILE * fp, PVPatch * patch,
                                  const PVLayerLoc * loc, int kx0, int ky0, int kf0);

   void setOutputIndices(bool flag)   {outputIndices = flag;}
   void setStdpVars(bool flag)   {stdpVars = flag;}

protected:
   FILE * fp;
   int    kPre;  // index of pre-synaptic neuron
   int    kxPre, kyPre, kfPre;
   int arborID;
   bool   outputIndices;
   bool   stdpVars;

};

} // namespace PV

#endif /* CONNECTIONPROBE_HPP_ */
