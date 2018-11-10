/*
 * MomentumLCAInternalStateBuffer.cpp
 *
 *  Created on: Mar 15, 2016
 *      Author: slundquist
 */

#include "MomentumLCAInternalStateBuffer.hpp"
#include <iostream>

namespace PV {

MomentumLCAInternalStateBuffer::MomentumLCAInternalStateBuffer() {}

MomentumLCAInternalStateBuffer::MomentumLCAInternalStateBuffer(const char *name, HyPerCol *hc) {
   initialize(name, hc);
}

MomentumLCAInternalStateBuffer::~MomentumLCAInternalStateBuffer() {}

int MomentumLCAInternalStateBuffer::initialize(const char *name, HyPerCol *hc) {
   HyPerLCAInternalStateBuffer::initialize(name, hc);
   return PV_SUCCESS;
}

int MomentumLCAInternalStateBuffer::ioParamsFillGroup(enum ParamsIOFlag ioFlag) {
   int status = HyPerLCAInternalStateBuffer::ioParamsFillGroup(ioFlag);
   ioParam_LCAMomentumRate(ioFlag);
   return status;
}

void MomentumLCAInternalStateBuffer::ioParam_LCAMomentumRate(enum ParamsIOFlag ioFlag) {
   parent->parameters()->ioParamValue(
         ioFlag,
         name,
         "LCAMomentumRate",
         &mLCAMomentumRate,
         mLCAMomentumRate,
         true /*warnIfAbsent*/);
}

Response::Status MomentumLCAInternalStateBuffer::communicateInitInfo(
      std::shared_ptr<CommunicateInitInfoMessage const> message) {
   auto status = HyPerLCAInternalStateBuffer::communicateInitInfo(message);
   if (!Response::completed(status)) {
      return status;
   }
   auto *hierarchy           = message->mHierarchy;
   std::string prevDriveName = std::string("prevDrive \"") + getName() + "\"";
   mPrevDrive                = hierarchy->lookupByName<RestrictedBuffer>(prevDriveName);
   FatalIf(
         mPrevDrive == nullptr,
         "%s requires a RestrictedBuffer with the label \"prevDrive\" and the name \"%s\".\n",
         getDescription_c(),
         getName());
   return Response::SUCCESS;
}

Response::Status MomentumLCAInternalStateBuffer::allocateDataStructures() {
   return HyPerLCAInternalStateBuffer::allocateDataStructures();
}

Response::Status MomentumLCAInternalStateBuffer::initializeState(
      std::shared_ptr<InitializeStateMessage const> message) {
   return HyPerLCAInternalStateBuffer::initializeState(message);
}

#ifdef PV_USE_CUDA
void MomentumLCAInternalStateBuffer::allocateUpdateKernel() {
   PVCuda::CudaDevice *device = mCudaDevice;

   size_t size  = getLayerLoc()->nbatch * sizeof(double);
   mCudaDtAdapt = device->createBuffer(size, &getDescription());

   mCudaUpdateKernel = new PVCuda::CudaUpdateMomentumLCAInternalState(device);
}

Response::Status MomentumLCAInternalStateBuffer::copyInitialStateToGPU() {
   Response::Status status = HyPerInternalStateBuffer::copyInitialStateToGPU();
   if (!Response::completed(status)) {
      return status;
   }
   if (!isUsingGPU()) {
      return status;
   }

   // Set arguments of update kernel
   const PVLayerLoc *loc = getLayerLoc();
   int const nx          = loc->nx;
   int const ny          = loc->ny;
   int const nf          = loc->nf;
   int const numNeurons  = nx * ny * nf;
   int const nbatch      = loc->nbatch;
   int const lt          = loc->halo.lt;
   int const rt          = loc->halo.rt;
   int const dn          = loc->halo.dn;
   int const up          = loc->halo.up;
   int const numChannels = mLayerInput->getNumChannels();
   pvAssert(getCudaBuffer());
   PVCuda::CudaBuffer *prevDriveCudaBuffer  = mPrevDrive->getCudaBuffer();
   float const selfInteract                 = (float)this->mSelfInteract;
   float const tau                          = mScaledTimeConstantTau;
   PVCuda::CudaBuffer *layerInputCudaBuffer = mLayerInput->getCudaBuffer();
   PVCuda::CudaBuffer *activityCudaBuffer   = mActivity->getCudaBuffer();
   pvAssert(layerInputCudaBuffer);
   pvAssert(prevDriveCudaBuffer);

   auto *cudaKernel = dynamic_cast<PVCuda::CudaUpdateMomentumLCAInternalState *>(mCudaUpdateKernel);
   pvAssert(cudaKernel);
   // Set arguments to kernel
   cudaKernel->setArgs(
         nbatch,
         numNeurons,
         nx,
         ny,
         nf,
         lt,
         rt,
         dn,
         up,
         numChannels,
         getCudaBuffer(),
         prevDriveCudaBuffer,
         selfInteract,
         mCudaDtAdapt,
         tau,
         mLCAMomentumRate,
         layerInputCudaBuffer,
         activityCudaBuffer);
   return Response::SUCCESS;
}

void MomentumLCAInternalStateBuffer::updateBufferGPU(double simTime, double deltaTime) {
   pvAssert(isUsingGPU()); // if not using GPU, should be in updateBufferCPU() method instead.
   if (!mLayerInput->isUsingGPU()) {
      mLayerInput->copyToCuda();
   }

   // Copy over mCudaDtAdapt
   mCudaDtAdapt->copyToDevice(deltaTimes(simTime, deltaTime));

   // Sync all buffers before running
   mCudaDevice->syncDevice();

   // Run kernel
   mCudaUpdateKernel->run();
}
#endif // PV_USE_CUDA

void MomentumLCAInternalStateBuffer::updateBufferCPU(double simTime, double deltaTime) {
#ifdef PV_USE_CUDA
   pvAssert(!isUsingGPU()); // if using GPU, should be in updateBufferGPU() method instead.
   if (mLayerInput->isUsingGPU()) {
      mLayerInput->copyFromCuda();
   }
#endif // PV_USE_CUDA

   const PVLayerLoc *loc = getLayerLoc();
   float const *A        = mActivity->getBufferData();
   float *V              = mBufferData.data();
   int numChannels       = mLayerInput->getNumChannels();

   int nx            = loc->nx;
   int ny            = loc->ny;
   int nf            = loc->nf;
   int numNeurons    = getBufferSize();
   int nbatch        = loc->nbatch;
   int lt            = loc->halo.lt;
   int rt            = loc->halo.rt;
   int dn            = loc->halo.dn;
   int up            = loc->halo.up;
   float tau         = mScaledTimeConstantTau;
   bool selfInteract = mSelfInteract;

   float const *GSynExc  = mLayerInput->getChannelData(CHANNEL_EXC);
   double const *dtAdapt = deltaTimes(simTime, deltaTime);
   float *prevDrive      = mPrevDrive->getReadWritePointer();

   if (numChannels == 1) {
#ifdef PV_USE_OPENMP_THREADS
#pragma omp parallel for schedule(static)
#endif
      for (int kIndex = 0; kIndex < numNeurons * nbatch; kIndex++) {
         int b = kIndex / numNeurons;
         int k = kIndex % numNeurons;

         float exp_tau             = (float)std::exp(-dtAdapt[b] / (double)tau);
         float *VBatch             = V + b * numNeurons;
         float const *GSynExcBatch = GSynExc + b * numNeurons;
         float const gSyn          = GSynExcBatch[k]; // only one channel
         float *prevDriveBatch     = prevDrive + b * numNeurons;
         // Activity is an extended buffer.
         float const *ABatch = A + b * (nx + rt + lt) * (ny + up + dn) * nf;

         int kex            = kIndexExtended(k, nx, ny, nf, lt, rt, dn, up);
         float currentDrive = (1.0f - exp_tau) * (gSyn + selfInteract * ABatch[kex]);
         // Accumulate into VBatch with decay and momentum
         VBatch[k] = exp_tau * VBatch[k] + currentDrive + mLCAMomentumRate * prevDriveBatch[k];
         // Update momentum buffer
         prevDriveBatch[k] = currentDrive;
      }
   }
   else {
      pvAssert(numChannels > 1);
      float const *GSynInh = mLayerInput->getChannelData(CHANNEL_INH);
#ifdef PV_USE_OPENMP_THREADS
#pragma omp parallel for schedule(static)
#endif
      for (int kIndex = 0; kIndex < numNeurons * nbatch; kIndex++) {
         int b = kIndex / numNeurons;
         int k = kIndex % numNeurons;

         float exp_tau             = (float)exp(-dtAdapt[b] / (double)tau);
         float *VBatch             = V + b * numNeurons;
         float const *GSynExcBatch = GSynExc + b * numNeurons;
         float const *GSynInhBatch = GSynInh + b * numNeurons;
         float const gSyn          = GSynExcBatch[k] - GSynInhBatch[k];
         float *prevDriveBatch     = prevDrive + b * numNeurons;
         // Activity is an extended buffer.
         float const *ABatch = A + b * (nx + rt + lt) * (ny + up + dn) * nf;

         int kex            = kIndexExtended(k, nx, ny, nf, lt, rt, dn, up);
         float currentDrive = (1.0f - exp_tau) * (gSyn + selfInteract * ABatch[kex]);
         // Accumulate into VBatch with decay and momentum
         VBatch[k] = exp_tau * VBatch[k] + currentDrive + mLCAMomentumRate * prevDriveBatch[k];
         // Update momentum buffer
         prevDriveBatch[k] = currentDrive;
      }
   }
}

} /* namespace PV */
