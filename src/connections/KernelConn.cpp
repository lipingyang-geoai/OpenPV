/*
 * KernelConn.cpp
 *
 *  Created on: Aug 6, 2009
 *      Author: gkenyon
 */

#include "KernelConn.hpp"
#include <assert.h>
#include <float.h>
#include <mpi.h>
#include "../io/io.h"

namespace PV {

KernelConn::KernelConn()
{
   initialize_base();
}

KernelConn::KernelConn(const char * name, HyPerCol * hc, HyPerLayer * pre,
      HyPerLayer * post, ChannelType channel)
{
   initialize_base();
   initialize(name, hc, pre, post, channel, NULL);
}

KernelConn::KernelConn(const char * name, HyPerCol * hc, HyPerLayer * pre,
      HyPerLayer * post)
{
   initialize_base();
   initialize(name, hc, pre, post, channel, NULL); // use default channel
}

// provide filename or set to NULL
KernelConn::KernelConn(const char * name, HyPerCol * hc, HyPerLayer * pre,
      HyPerLayer * post, ChannelType channel, const char * filename)
{
   initialize_base();
   initialize(name, hc, pre, post, channel, filename);
}

int KernelConn::initialize_base()
{
   plasticityFlag = false;
   kernelPatches = NULL;
   return HyPerConn::initialize_base();
}

int KernelConn::initialize(const char * name, HyPerCol * hc,
      HyPerLayer * pre, HyPerLayer * post, ChannelType channel, const char * filename){
   PVParams * params = parent->parameters();
   plasticityFlag = (bool) params->value(name, "plasticityFlag", 0);
   return KernelConn::initialize(name, hc, pre, post, channel, filename);
}


PVPatch ** KernelConn::allocWeights(PVPatch ** patches, int nPatches, int nxPatch,
      int nyPatch, int nfPatch)
{
   const int arbor = 0;
   int numKernelPatches = numDataPatches(arbor);

   assert(kernelPatches == NULL);
   kernelPatches = (PVPatch**) calloc(sizeof(PVPatch*), numKernelPatches);
   assert(kernelPatches != NULL);

   for (int kernelIndex = 0; kernelIndex < numKernelPatches; kernelIndex++) {
      kernelPatches[kernelIndex] = pvpatch_inplace_new(nxPatch, nyPatch, nfPatch);
      assert(kernelPatches[kernelIndex] != NULL );
   }
   for (int patchIndex = 0; patchIndex < nPatches; patchIndex++) {
      patches[patchIndex] = pvpatch_new(nxPatch, nyPatch, nfPatch);
   }
   for (int patchIndex = 0; patchIndex < nPatches; patchIndex++) {
      int kernelIndex = this->patchIndexToKernelIndex(patchIndex);
      patches[patchIndex]->data = kernelPatches[kernelIndex]->data;
   }
   return patches;
}

/*TODO  createWeights currently breaks in this subclass if called more than once,
 * fix interface by adding extra dataPatches argument to overloaded method
 * so asserts are unnecessary
 */
PVPatch ** KernelConn::createWeights(PVPatch ** patches, int nPatches, int nxPatch,
      int nyPatch, int nfPatch)
{
   assert(numAxonalArborLists == 1);

   assert(patches == NULL);

   patches = (PVPatch**) calloc(sizeof(PVPatch*), nPatches);
   assert(patches != NULL);

   assert(kernelPatches == NULL);
   allocWeights(patches, nPatches, nxPatch, nyPatch, nfPatch);

   return patches;
}

int KernelConn::deleteWeights()
{
   const int arbor = 0;

   for (int k = 0; k < numDataPatches(arbor); k++) {
      pvpatch_inplace_delete(kernelPatches[k]);
   }
   free(kernelPatches);

   return HyPerConn::deleteWeights();
}

PVPatch ** KernelConn::initializeWeights(PVPatch ** patches, int numPatches,
      const char * filename)
{
   int arbor = 0;
   int numKernelPatches = numDataPatches(arbor);
   HyPerConn::initializeWeights(kernelPatches, numKernelPatches, filename);
   return patches;
}

PVPatch ** KernelConn::readWeights(PVPatch ** patches, int numPatches,
      const char * filename)
{
   HyPerConn::readWeights(patches, numPatches, filename);
   // return HyPerConn::normalizeWeights(patches, numPatches);

   if( parent->parameters()->value(name, "normalize", 0.0f, true) ) {
      patches = HyPerConn::normalizeWeights(patches, numPatches);
   }
   return patches;
}

int KernelConn::numDataPatches(int arbor)
{
   int nxKernel = (pre->getXScale() < post->getXScale()) ? pow(2,
         post->getXScale() - pre->getXScale()) : 1;
   int nyKernel = (pre->getYScale() < post->getYScale()) ? pow(2,
         post->getYScale() - pre->getYScale()) : 1;
   int numKernelPatches = pre->clayer->loc.nf * nxKernel * nyKernel;
   return numKernelPatches;
}

float KernelConn::minWeight()
{
   const int axonID = 0;
   const int numKernels = numDataPatches(axonID);
   const int numWeights = nxp * nyp * nfp;
   float min_weight = FLT_MAX;
   for (int iKernel = 0; iKernel < numKernels; iKernel++) {
      pvdata_t * kernelWeights = kernelPatches[iKernel]->data;
      for (int iWeight = 0; iWeight < numWeights; iWeight++) {
         min_weight = (min_weight < kernelWeights[iWeight]) ? min_weight
               : kernelWeights[iWeight];
      }
   }
   return min_weight;
}

float KernelConn::maxWeight()
{
   const int axonID = 0;
   const int numKernels = numDataPatches(axonID);
   const int numWeights = nxp * nyp * nfp;
   float max_weight = -FLT_MAX;
   for (int iKernel = 0; iKernel < numKernels; iKernel++) {
      pvdata_t * kernelWeights = kernelPatches[iKernel]->data;
      for (int iWeight = 0; iWeight < numWeights; iWeight++) {
         max_weight = (max_weight > kernelWeights[iWeight]) ? max_weight
               : kernelWeights[iWeight];
      }
   }
   return max_weight;
}


int KernelConn::updateState(float time, float dt){
   // merge kernel changes across processes
   int status = PV_SUCCESS;
#ifdef PV_USE_MPI
   if (~plasticityFlag) {
      return status;
   }
   Communicator * comm = parent->icCommunicator();
   const MPI_Comm mpi_comm = comm->communicator();

   const int axonId = 0;       // assume only one for now
   const int numPatches = numDataPatches(axonId);
   const size_t patchSize = nxp*nyp*nfp*sizeof(float);
   const size_t localSize = numPatches * patchSize;

   //TODO!!! preallocate buf
   float * buf = (float *) malloc(localSize);
   float * buf0 = buf;
   assert(buf != NULL);

   // load kernel weights in buffer
   for (int k = 0; k < numPatches; k++) {
      PVPatch * p = kernelPatches[k];
      const pvdata_t * data = p->data;

      const int sxp = p->sx;
      const int syp = p->sy;
      const int sfp = p->sf;

      for (int y = 0; y < p->ny; y++) {
         for (int x = 0; x < p->nx; x++) {
            for (int f = 0; f < p->nf; f++) {
               float val = data[x * sxp + y * syp + f * sfp];
               memcpy(buf, &val, sizeof(float));
               buf += sizeof(float);
            }
         }
      }
   }

   // sum weights from each proc into buf, send and recv buffers are the same
   int ierr;
   ierr = MPI_Allreduce(MPI_IN_PLACE, buf,localSize, MPI_FLOAT, MPI_SUM, mpi_comm);

   // replace individual kernel weights with average over procs
   const int nxProcs = comm->numCommColumns();
   const int nyProcs = comm->numCommRows();
   const int nProcs = nxProcs * nyProcs;
   buf = buf0;
   for (int k = 0; k < numPatches; k++) {
      PVPatch * p = kernelPatches[k];
      const pvdata_t * data = p->data;

      const int sxp = p->sx;
      const int syp = p->sy;
      const int sfp = p->sf;

      for (int y = 0; y < p->ny; y++) {
         for (int x = 0; x < p->nx; x++) {
            for (int f = 0; f < p->nf; f++) {
               float val;
               memcpy(&val, buf, sizeof(float));
               val /= nProcs;
               data[x * sxp + y * syp + f * sfp] = val;
               buf += sizeof(float);
            }
         }
      }
   }

   return status;
#else
   return status;
#endif // PV_USE_MPI


}


int KernelConn::updateWeights(int axonId){

   return 0;
}



int KernelConn::gauss2DCalcWeights(PVPatch * wp, int kKernel, int no, int numFlanks,
                                   float shift, float rotate, float aspect, float sigma,
                                   float r2Max, float strength,
                                   float deltaThetaMax, float thetaMax, float bowtieFlag,
                                   float bowtieAngle)
{
   int kPatch;
   kPatch = kernelIndexToPatchIndex(kKernel);
   return HyPerConn::gauss2DCalcWeights(wp, kPatch, no, numFlanks,
                                        shift, rotate, aspect, sigma, r2Max, strength,
                                        deltaThetaMax, thetaMax, bowtieFlag, bowtieAngle);
}

int KernelConn::cocircCalcWeights(PVPatch * wp, int kKernel, int noPre, int noPost,
      float sigma_cocirc, float sigma_kurve, float sigma_chord, float delta_theta_max,
      float cocirc_self, float delta_radius_curvature, int numFlanks, float shift,
      float aspect, float rotate, float sigma, float r2Max, float strength)
{
   int kPatch;
   kPatch = kernelIndexToPatchIndex(kKernel);
   return HyPerConn::cocircCalcWeights(wp, kPatch, noPre, noPost, sigma_cocirc,
         sigma_kurve, sigma_chord, delta_theta_max, cocirc_self, delta_radius_curvature,
         numFlanks, shift, aspect, rotate, sigma, r2Max, strength);
}

PVPatch ** KernelConn::normalizeWeights(PVPatch ** patches, int numPatches)
{
   const int arbor = 0;
   const int num_kernels = numDataPatches(arbor);
   HyPerConn::normalizeWeights(kernelPatches, num_kernels);
   PVParams * inputParams = parent->parameters();
   float symmetrizeWeightsFlag = inputParams->value(name, "symmetrizeWeights",0);
   if ( symmetrizeWeightsFlag ){
      symmetrizeWeights(kernelPatches, num_kernels);
   }
   return patches;
}

PVPatch ** KernelConn::symmetrizeWeights(PVPatch ** patches, int numPatches)
{
   printf("Entering KernelConn::symmetrizeWeights for connection \"%s\"\n", name);
   PVPatch ** symPatches;
   symPatches = (PVPatch**) calloc(sizeof(PVPatch*), numPatches);
   assert(symPatches != NULL);
   for (int iKernel = 0; iKernel < numPatches; iKernel++) {
      symPatches[iKernel] = pvpatch_inplace_new(nxp, nyp, nfp);
      assert(symPatches[iKernel] != NULL );
   }
   const int sy = nxp * nfp;
   const float deltaTheta = PI / nfp;
   const float offsetTheta = 0.5f * deltaTheta;
   const int kyMid = nyp / 2;
   const int kxMid = nxp / 2;
   for (int iSymKernel = 0; iSymKernel < numPatches; iSymKernel++) {
      PVPatch * symWp = symPatches[iSymKernel];
      pvdata_t * symW = symWp->data;
      float symTheta = offsetTheta + iSymKernel * deltaTheta;
      for (int kySym = 0; kySym < nyp; kySym++) {
         float dySym = kySym - kyMid;
         for (int kxSym = 0; kxSym < nxp; kxSym++) {
            float dxSym = kxSym - kxMid;
            float distSym = sqrt(dxSym * dxSym + dySym * dySym);
            if (distSym > abs(kxMid > kyMid ? kxMid : kyMid)) {
               continue;
            }
            float dyPrime = dySym * cos(symTheta) - dxSym * sin(symTheta);
            float dxPrime = dxSym * cos(symTheta) + dySym * sin(symTheta);
            for (int kfSym = 0; kfSym < nfp; kfSym++) {
               int kDf = kfSym - iSymKernel;
               int iSymW = kfSym + nfp * kxSym + sy * kySym;
               for (int iKernel = 0; iKernel < nfp; iKernel++) {
                  PVPatch * kerWp = kernelPatches[iKernel];
                  pvdata_t * kerW = kerWp->data;
                  int kfRot = iKernel + kDf;
                  if (kfRot < 0) {
                     kfRot = nfp + kfRot;
                  }
                  else {
                     kfRot = kfRot % nfp;
                  }
                  float rotTheta = offsetTheta + iKernel * deltaTheta;
                  float yRot = dyPrime * cos(rotTheta) + dxPrime * sin(rotTheta);
                  float xRot = dxPrime * cos(rotTheta) - dyPrime * sin(rotTheta);
                  yRot += kyMid;
                  xRot += kxMid;
                  // should find nearest neighbors and do weighted average
                  int kyRot = yRot + 0.5f;
                  int kxRot = xRot + 0.5f;
                  int iRotW = kfRot + nfp * kxRot + sy * kyRot;
                  symW[iSymW] += kerW[iRotW] / nfp;
               } // kfRot
            } // kfSymm
         } // kxSym
      } // kySym
   } // iKernel
   const int num_weights = nfp * nxp * nyp;
   for (int iKernel = 0; iKernel < numPatches; iKernel++) {
      PVPatch * kerWp = kernelPatches[iKernel];
      pvdata_t * kerW = kerWp->data;
      PVPatch * symWp = symPatches[iKernel];
      pvdata_t * symW = symWp->data;
      for (int iW = 0; iW < num_weights; iW++) {
         kerW[iW] = symW[iW];
      }
   } // iKernel
   for (int iKernel = 0; iKernel < numPatches; iKernel++) {
      pvpatch_inplace_delete(symPatches[iKernel]);
   } // iKernel
   free(symPatches);
   printf("Exiting KernelConn::symmetrizeWeights for connection \"%s\"\n", name);
   return patches;
}

int KernelConn::writeWeights(float time, bool last)
{
   const int arbor = 0;
   const int numPatches = numDataPatches(arbor);
   return HyPerConn::writeWeights(kernelPatches, numPatches, NULL, time, last);
}

} // namespace PV

