/*
 * TransposeConn.cpp
 *
 *  Created on: May 16, 2011
 *      Author: peteschultz
 */

#include "TransposeConn.hpp"

namespace PV {

TransposeConn::TransposeConn() {
    initialize_base();
}  // TransposeConn::~TransposeConn()

TransposeConn::TransposeConn(const char * name, HyPerCol * hc, HyPerLayer * preLayer, HyPerLayer * postLayer, ChannelType channelType, KernelConn * auxConn) {
    initialize_base();
    initialize(name, hc, preLayer, postLayer, channelType, auxConn);
}  // TransposeConn::TransposeConn(const char * name, HyPerCol * hc, HyPerLayer *, HyPerLayer *, ChannelType, KernelConn *)

TransposeConn::~TransposeConn() {

}  // TransposeConn::~TransposeConn()

int TransposeConn::initialize_base() {
   plasticityFlag = true; // Default value; override in params
   weightUpdatePeriod = 1;   // Default value; override in params
   // TransposeConn::initialize_base() gets called after
   // KernelConn::initialize_base() so these default values override
   // those in KernelConn::initialize_base().
   // TransposeConn::initialize_base() gets called before
   // KernelConn::initialize(), so these values still get overridden
   // by the params file values.

   originalConn = NULL;
   return PV_SUCCESS;
}  // TransposeConn::initialize_base()

int TransposeConn::initialize(const char * name, HyPerCol * hc, HyPerLayer * preLayer, HyPerLayer * postLayer, ChannelType channelType, KernelConn * auxConn) {

   originalConn = auxConn;
   normalize_flag = false; // HyPerConn::initializeWeights never gets called (most of what it does isn't needed) so initNormalize never gets called
   int status = KernelConn::initialize(name, hc, preLayer, postLayer, channelType, NULL, NULL);
   //TransposeConn has not been updated to support multiple arbors!
   assert(numberOfAxonalArborLists()==1);
   return status;
}

PVPatch *** TransposeConn::initializeWeights(PVPatch *** arbors, int numPatches, const char * filename) {
    if( filename ) {
        return KernelConn::initializeWeights(arbors, numPatches, filename);
    }
    else {
        transposeKernels();
    }
    return arbors;
}  // TransposeConn::initializeWeights(PVPatch **, int, const char *)

// int TransposeConn::initNormalize() {
//    normalize_flag = false;
//    return PV_SUCCESS;
// }

int TransposeConn::setPatchSize(const char * filename) {
    int status = PV_SUCCESS;

    int xscaleDiff = pre->getXScale() - post->getXScale();
    // If originalConn is many-to-one, the transpose connection is one-to-many.
    // Then xscaleDiff > 0.
    // Similarly, if originalConn is one-to-many, xscaleDiff < 0.
    int yscaleDiff = pre->getYScale() - post->getYScale();

    nxp = originalConn->xPatchSize();
    if(xscaleDiff > 0 ) {
        nxp *= (int) pow( 2, xscaleDiff );
    }
    else if(xscaleDiff < 0) {
        nxp /= (int) pow(2,-xscaleDiff);
        assert(originalConn->xPatchSize()==nxp*pow( 2, (float) (-xscaleDiff) ));
    }
    nyp = originalConn->yPatchSize();
    if(yscaleDiff > 0 ) {
        nyp *= (int) pow( 2, (float) yscaleDiff );
    }
    else if(yscaleDiff < 0) {
        nyp /= (int) pow(2,-yscaleDiff);
        assert(originalConn->yPatchSize()==nyp*pow( 2, (float) (-yscaleDiff) ));
    }
    nfp = post->getLayerLoc()->nf;

    assert( checkPatchSize(nyp, pre->getXScale(), post->getXScale(), 'x') ==
            PV_SUCCESS );

    assert( checkPatchSize(nyp, pre->getYScale(), post->getYScale(), 'y') ==
            PV_SUCCESS );

    status = filename ? patchSizeFromFile(filename) : PV_SUCCESS;
    return status;
}  // TransposeConn::setPatchSize(const char *)

int TransposeConn::updateWeights(int axonID) {
   int status;
   if(originalConn->getLastUpdateTime() > lastUpdateTime ) {
      status = transposeKernels();
      lastUpdateTime = parent->simulationTime();
   }
   else
      status = PV_SUCCESS;
   return status;
}  // end of TransposeConn::updateWeights(int);

// TODO reorganize transposeKernels():  Loop over kernelNumberFB on outside, call transposeOneKernel(kpFB, kernelnumberFB), which handles all the cases.
// This would play better with Kris's initWeightsMethod.
int TransposeConn::transposeKernels() {
    // compute the transpose of originalConn->kernelPatches and
    // store into this->kernelPatches
    // assume scale factors are 1 and that nxp, nyp are odd.

    int xscalediff = pre->getXScale()-post->getXScale();
    int yscalediff = pre->getYScale()-post->getYScale();
    // scalediff>0 means TransposeConn's post--that is, the originalConn's pre--has a higher neuron density

    int numFBKernelPatches = numDataPatches();
    int numFFKernelPatches = originalConn->numDataPatches();

    if( xscalediff <= 0 && yscalediff <= 0) {
        int xscaleq = (int) pow(2,-xscalediff);
        int yscaleq = (int) pow(2,-yscalediff);

        for( int kernelnumberFB = 0; kernelnumberFB < numFBKernelPatches; kernelnumberFB++ ) {
            PVPatch * kpFB = getKernelPatch(0, kernelnumberFB);
            int nfFB = nfp;
               assert(numFFKernelPatches == nfFB);
            int nxFB = kpFB->nx;
            int nyFB = kpFB->ny;
            for( int kyFB = 0; kyFB < nyFB; kyFB++ ) {
                for( int kxFB = 0; kxFB < nxFB; kxFB++ ) {
                    for( int kfFB = 0; kfFB < nfFB; kfFB++ ) {
                        int kIndexFB = kIndex(kxFB,kyFB,kfFB,nxFB,nyFB,nfFB);
                        int kernelnumberFF = kfFB;
                        PVPatch * kpFF = originalConn->getKernelPatch(0, kernelnumberFF);
                           assert(numFBKernelPatches == originalConn->fPatchSize() * xscaleq * yscaleq);
                        int kfFF = featureIndex(kernelnumberFB, xscaleq, yscaleq, originalConn->fPatchSize());
                        int kxFFoffset = kxPos(kernelnumberFB, xscaleq, yscaleq, originalConn->fPatchSize());
                        int kxFF = (nxp - 1 - kxFB) * xscaleq + kxFFoffset;
                        int kyFFoffset = kyPos(kernelnumberFB, xscaleq, yscaleq, originalConn->fPatchSize());
                        int kyFF = (nyp - 1 - kyFB) * yscaleq + kyFFoffset;
                        int kIndexFF = kIndex(kxFF, kyFF, kfFF, kpFF->nx, kpFF->ny, originalConn->fPatchSize());
                        // can the calls to kxPos, kyPos, featureIndex be replaced by one call to patchIndexToKernelIndex?
                        kpFB->data[kIndexFB] = kpFF->data[kIndexFF];
                    }
                }
            }
        }
    }
    else if( xscalediff > 0 && yscalediff > 0) {
        int xscaleq = (int) pow(2,xscalediff);
        int yscaleq = (int) pow(2,yscalediff);
        for( int kernelnumberFB = 0; kernelnumberFB < numFBKernelPatches; kernelnumberFB++ ) {
            PVPatch * kpFB = getKernelPatch(0, kernelnumberFB);
            int nxFB = kpFB->nx;
            int nyFB = kpFB->ny;
            int nfFB = nfp;
            for( int kyFB = 0; kyFB < nyFB; kyFB++ ) {
                int precelloffsety = kyFB % yscaleq;
                   for( int kxFB = 0; kxFB < nxFB; kxFB++ ) {
                    int precelloffsetx = kxFB % xscaleq;
                    for( int kfFB = 0; kfFB < nfFB; kfFB++ ) {
                        int kernelnumberFF = (precelloffsety*xscaleq + precelloffsetx)*nfFB + kfFB;
                        PVPatch * kpFF = originalConn->getKernelPatch(0, kernelnumberFF);
                        int kxFF = (nxp-kxFB-1)/xscaleq;
                        assert(kxFF >= 0 && kxFF < originalConn->xPatchSize());
                        int kyFF = (nyp-kyFB-1)/yscaleq;
                        assert(kyFF >= 0 && kyFF < originalConn->yPatchSize());
                        int kfFF = kernelnumberFB;
                        assert(kfFF >= 0 && kfFF < originalConn->fPatchSize());
                        int kIndexFF = kIndex(kxFF, kyFF, kfFF, kpFF->nx, kpFF->ny, originalConn->fPatchSize());
                        int kIndexFB = kIndex(kxFB, kyFB, kfFB, nxFB, nyFB, nfFB);
                        kpFB->data[kIndexFB] = kpFF->data[kIndexFF];
                    }
                }
            }
        }
    }
    else {
        fprintf(stderr,"xscalediff = %d, yscalediff = %d: the case of many-to-one in one dimension and one-to-many in the other"
                       "has not yet been implemented.\n", xscalediff, yscalediff);
        exit(1);
    }

    return PV_SUCCESS;
}  // TransposeConn::transposeKernels()

} // end namespace PV
