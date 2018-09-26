/*
 * updateStateFunctions.h
 *
 * Static inline methods to be called by layers' updateState methods
 *
 *  Created on: Mar 7, 2012
 *      Author: pschultz
 */
#ifndef UPDATESTATEFUNCTIONS_H_
#define UPDATESTATEFUNCTIONS_H_

#ifndef PV_USE_CUDA
#include "../include/pv_types.h"
#include "../utils/conversions.h"
#endif // PV_USE_CUDA

#include "../include/pv_common.h"
#include <float.h>

#ifndef PV_USE_CUDA
#define KERNEL inline
#define MEM_GLOBAL
#define MEM_CONST
#define MEM_LOCAL
#else
#define KERNEL __device__
#define MEM_GLOBAL
#define MEM_CONST
#define MEM_LOCAL
#define getIndex() (blockIdx.x * blockDim.x) + threadIdx.x
#include "../cudakernels/conversions.hcu"
#define CHANNEL_EXC 0
#define CHANNEL_INH 1
#define CHANNEL_INHB 2
#define CHANNEL_GAP 3
#endif

// Prototypes
KERNEL
int applyGSyn_LabelErrorLayer(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *V,
      MEM_GLOBAL float *GSynHead,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up,
      int isBinary);

KERNEL
int setActivity_ANNLayer_vertices(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *V,
      int num_channels,
      MEM_GLOBAL float *activity,
      int numVertices,
      float *verticesV,
      float *verticesA,
      float *slopes,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up);

KERNEL
int setActivity_ANNLayer_threshminmax(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *V,
      int num_channels,
      MEM_GLOBAL float *activity,
      float VThresh,
      float AMin,
      float AMax,
      float AShift,
      float VWidth,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up);
KERNEL
int applyGSyn_HyPerLCALayer(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *V,
      MEM_GLOBAL float *GSynHead,
      MEM_GLOBAL float *activity,
      float dt_tau,
      float selfInteract,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up);
KERNEL
int applyGSyn_HyPerLCALayer2(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *V,
      MEM_GLOBAL float *GSynHead,
      MEM_GLOBAL float *activity,
      double *dtAdapt,
      float tau,
      float selfInteract,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up);
KERNEL
int applyGSyn_ISTALayer(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *V,
      MEM_GLOBAL float *GSynHead,
      MEM_GLOBAL float *activity,
      double *dtAdapt,
      float tau,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up,
      float VThresh);
KERNEL
int applyGSyn_ISTALayer2(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *V,
      MEM_GLOBAL float *GSynHead,
      MEM_GLOBAL float *activity,
      double *dtAdapt,
      float tau,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up,
      float VThresh);
KERNEL
int applyGSyn_ANNWhitenedLayer(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *V,
      MEM_GLOBAL float *GSynHead);

KERNEL
int updateV_HyPerLCALayer(
      int nbatch,
      int numNeurons,
      int numChannels,
      MEM_GLOBAL float *V,
      MEM_GLOBAL float *GSynHead,
      MEM_GLOBAL float *activity,
      int numVertices,
      float *verticesV,
      float *verticesA,
      float *slopes,
      double *dtAdapt,
      float tau,
      float selfInteract,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up);

KERNEL
int updateV_MomentumLCALayer(
      int nbatch,
      int numNeurons,
      int numChannels,
      MEM_GLOBAL float *V,
      MEM_GLOBAL float *GSynHead,
      MEM_GLOBAL float *activity,
      MEM_GLOBAL float *prevDrive,
      int numVertices,
      float *verticesV,
      float *verticesA,
      float *slopes,
      double *dtAdapt,
      float tau,
      float LCAMomentumRate,
      float selfInteract,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up);

KERNEL
int updateV_ISTALayer(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *V,
      MEM_GLOBAL float *GSynHead,
      MEM_GLOBAL float *activity,
      float VThresh,
      MEM_GLOBAL double *dtAdapt,
      float tau,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up,
      int numChannels);
KERNEL
int updateV_SigmoidLayer();
KERNEL
int applyVMax_ANNLayer_threshminmax(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *V,
      float AMax,
      MEM_GLOBAL float *activity,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up);
KERNEL
int applyVThresh_ANNLayer_threshminmax(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *V,
      float AMin,
      float VThresh,
      float AShift,
      float VWidth,
      MEM_GLOBAL float *activity,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up);
KERNEL
int setActivity_HyPerLayer(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *A,
      MEM_GLOBAL float const *V,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up);

KERNEL
int setActivity_PtwiseLinearTransferLayer(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *A,
      MEM_GLOBAL float *V,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up,
      int numVertices,
      float *verticesV,
      float *verticesA,
      float *slopes);

KERNEL
int setActivity_GapLayer(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *A,
      MEM_GLOBAL float *V,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up,
      int orig_lt,
      int orig_rt,
      int orig_dn,
      int orig_up,
      MEM_GLOBAL float *active,
      float ampSpiklet);

// Definitions
KERNEL
int applyGSyn_LabelErrorLayer(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *V,
      MEM_GLOBAL float *GSynHead,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up,
      int isBinary) {
   int kbatch;
   MEM_GLOBAL float const *GSynExc = &GSynHead[CHANNEL_EXC * nbatch * numNeurons];
   MEM_GLOBAL float const *GSynInh = &GSynHead[CHANNEL_INH * nbatch * numNeurons];

   if (isBinary > 0) {
#ifndef PV_USE_CUDA
#ifdef PV_USE_OPENMP_THREADS
#pragma omp parallel for schedule(static)
#endif
      for (kbatch = 0; kbatch < numNeurons * nbatch; kbatch++)
#else
      kbatch = getIndex();
#endif // PV_USE_CUDA
      {
         int b                                = kbatch / numNeurons;
         int k                                = kbatch % numNeurons;
         MEM_GLOBAL float *VBatch             = V + b * numNeurons;
         MEM_GLOBAL float const *GSynExcBatch = GSynExc + b * numNeurons;
         MEM_GLOBAL float const *GSynInhBatch = GSynInh + b * numNeurons;
         VBatch[k]                            = GSynExcBatch[k] - GSynInhBatch[k];
         if (GSynExcBatch[k] > 0.0f) { // target label is positive
            VBatch[k] = VBatch[k] > 0.0f ? VBatch[k] : 0.0f;
         }
         else { // target label is negative
            VBatch[k] = VBatch[k] < 0.0f ? VBatch[k] : 0.0f;
         }
      }
   }
   else {
#ifndef PV_USE_CUDA
#ifdef PV_USE_OPENMP_THREADS
#pragma omp parallel for schedule(static)
#endif
      for (kbatch = 0; kbatch < numNeurons * nbatch; kbatch++)
#else
      kbatch = getIndex();
#endif // PV_USE_CUDA
      {
         int b                     = kbatch / numNeurons;
         int k                     = kbatch % numNeurons;
         float *VBatch             = V + b * numNeurons;
         float const *GSynExcBatch = GSynExc + b * numNeurons;
         float const *GSynInhBatch = GSynInh + b * numNeurons;

         float ratio = 1.0f;
         // Need to find maximum value of target label
         // If first feature, find ratio between target and guess feature val
         int iF = featureIndex(k, nx + lt + rt, ny + dn + up, nf);
         if (iF == 0) {
            float maxTargetVal = GSynExcBatch[k];
            int maxIdx         = k;
            // Find max value in feature space
            for (int iif = 1; iif < nf; iif++) {
               if (GSynExcBatch[k + iif] > maxTargetVal) {
                  maxTargetVal = GSynExcBatch[k + iif];
                  maxIdx       = k + iif;
               }
            }
            // Find ratio
            // if target label is positive and guess is over target
            if (maxTargetVal > 0 && GSynInhBatch[maxIdx] > maxTargetVal) {
               ratio = maxTargetVal / GSynInhBatch[maxIdx];
            }
            else {
               ratio = 1.0f;
            }
         }
         // Calculate V value based on target and rescaled guess
         VBatch[k] = GSynExcBatch[k] - (GSynInhBatch[k] * ratio);
         // If target label is negative, and guess is lower than target label, err = 0
         if (GSynExcBatch[k] < 0.0f) {
            VBatch[k] = VBatch[k] < 0.0f ? VBatch[k] : 0.0f;
         }
      }
   }
   return PV_SUCCESS;
}

KERNEL
int applyGSyn_HyPerLCALayer(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *V,
      MEM_GLOBAL float *GSynHead,
      MEM_GLOBAL float *activity,
      double *dtAdapt,
      float tau,
      float selfInteract,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up) {
   int kbatch;
   MEM_GLOBAL float *GSynError = &GSynHead[0 * nbatch * numNeurons]; // weighted input
#ifndef PV_USE_CUDA
#ifdef PV_USE_OPENMP_THREADS
#pragma omp parallel for schedule(static)
#endif
   for (kbatch = 0; kbatch < numNeurons * nbatch; kbatch++)
#else
   kbatch    = getIndex();
#endif // PV_USE_CUDA
   {
      int b                            = kbatch / numNeurons;
      int k                            = kbatch % numNeurons;
      float exp_tau                    = (float)exp(-dtAdapt[b] / (double)tau);
      MEM_GLOBAL float *VBatch         = V + b * numNeurons;
      MEM_GLOBAL float *GSynErrorBatch = GSynError + b * numNeurons;
      // Activity extended
      MEM_GLOBAL float *activityBatch = activity + b * (nx + rt + lt) * (ny + up + dn) * nf;
      int kex                         = kIndexExtended(k, nx, ny, nf, lt, rt, dn, up);
      VBatch[k]                       = exp_tau * VBatch[k]
                  + (1.0f - exp_tau) * (GSynErrorBatch[k] + selfInteract * activityBatch[kex]);
   }
   return PV_SUCCESS;
}

KERNEL
int applyGSyn_HyPerLCALayer2(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *V,
      MEM_GLOBAL float *GSynHead,
      MEM_GLOBAL float *activity,
      double *dtAdapt,
      float tau,
      float selfInteract,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up) {
   int kbatch;
   MEM_GLOBAL float *GSynError  = &GSynHead[0 * nbatch * numNeurons]; // weighted input
   MEM_GLOBAL float *GSynError2 = &GSynHead[1 * nbatch * numNeurons]; // weighted input

#ifndef PV_USE_CUDA
#ifdef PV_USE_OPENMP_THREADS
#pragma omp parallel for schedule(static)
#endif
   for (kbatch = 0; kbatch < numNeurons * nbatch; kbatch++)
#else
   kbatch    = getIndex();
#endif // PV_USE_CUDA
   {
      int b = kbatch / numNeurons;
      int k = kbatch % numNeurons;

      float exp_tau                     = (float)exp(-dtAdapt[b] / (double)tau);
      MEM_GLOBAL float *VBatch          = V + b * numNeurons;
      MEM_GLOBAL float *GSynErrorBatch  = GSynError + b * numNeurons;
      MEM_GLOBAL float *GSynError2Batch = GSynError2 + b * numNeurons;
      // Activity extended
      MEM_GLOBAL float *activityBatch = activity + b * (nx + rt + lt) * (ny + up + dn) * nf;

      int kex   = kIndexExtended(k, nx, ny, nf, lt, rt, dn, up);
      VBatch[k] = exp_tau * VBatch[k]
                  + (1.0f - exp_tau) * (GSynErrorBatch[k] - GSynError2Batch[k]
                                        + selfInteract * activityBatch[kex]);
   }
   return PV_SUCCESS;
}

KERNEL
int applyGSyn_MomentumLCALayer(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *V,
      MEM_GLOBAL float *GSynHead,
      MEM_GLOBAL float *activity,
      MEM_GLOBAL float *prevDrive,
      double *dtAdapt,
      float tau,
      float LCAMomentumRate,
      float selfInteract,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up) {
   int kbatch;
   MEM_GLOBAL float *GSynError = &GSynHead[0 * nbatch * numNeurons]; // weighted input
#ifndef PV_USE_CUDA
#ifdef PV_USE_OPENMP_THREADS
#pragma omp parallel for schedule(static)
#endif
   for (kbatch = 0; kbatch < numNeurons * nbatch; kbatch++)
#else
   kbatch    = getIndex();
#endif // PV_USE_CUDA
   {
      int b                            = kbatch / numNeurons;
      int k                            = kbatch % numNeurons;
      float exp_tau                    = expf((float)-dtAdapt[b] / tau);
      MEM_GLOBAL float *VBatch         = V + b * numNeurons;
      MEM_GLOBAL float *GSynErrorBatch = GSynError + b * numNeurons;
      MEM_GLOBAL float *prevDriveBatch = prevDrive + b * numNeurons;
      // Activity extended
      MEM_GLOBAL float *activityBatch = activity + b * (nx + rt + lt) * (ny + up + dn) * nf;
      int kex                         = kIndexExtended(k, nx, ny, nf, lt, rt, dn, up);
      // Calculate current drive
      float currentDrive =
            (1.0f - exp_tau) * (GSynErrorBatch[k] + selfInteract * activityBatch[kex]);
      // Accumulate into VBatch with decay and momentum
      VBatch[k] = exp_tau * VBatch[k] + currentDrive + LCAMomentumRate * prevDriveBatch[k];
      // Update momentum buffer
      prevDriveBatch[k] = currentDrive;
   }
   return PV_SUCCESS;
}

KERNEL
int applyGSyn_MomentumLCALayer2(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *V,
      MEM_GLOBAL float *GSynHead,
      MEM_GLOBAL float *activity,
      MEM_GLOBAL float *prevDrive,
      double *dtAdapt,
      float tau,
      float LCAMomentumRate,
      float selfInteract,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up) {
   int kbatch;
   MEM_GLOBAL float *GSynError  = &GSynHead[0 * nbatch * numNeurons]; // weighted input
   MEM_GLOBAL float *GSynError2 = &GSynHead[1 * nbatch * numNeurons]; // weighted input

#ifndef PV_USE_CUDA
#ifdef PV_USE_OPENMP_THREADS
#pragma omp parallel for schedule(static)
#endif
   for (kbatch = 0; kbatch < numNeurons * nbatch; kbatch++)
#else
   kbatch    = getIndex();
#endif // PV_USE_CUDA
   {
      int b = kbatch / numNeurons;
      int k = kbatch % numNeurons;

      float exp_tau                     = expf((float)-dtAdapt[b] / tau);
      MEM_GLOBAL float *VBatch          = V + b * numNeurons;
      MEM_GLOBAL float *GSynErrorBatch  = GSynError + b * numNeurons;
      MEM_GLOBAL float *GSynError2Batch = GSynError2 + b * numNeurons;
      MEM_GLOBAL float *prevDriveBatch  = prevDrive + b * numNeurons;
      // Activity extended
      MEM_GLOBAL float *activityBatch = activity + b * (nx + rt + lt) * (ny + up + dn) * nf;

      int kex = kIndexExtended(k, nx, ny, nf, lt, rt, dn, up);

      float currentDrive = (1.0f - exp_tau) * ((GSynErrorBatch[k] - GSynError2Batch[k])
                                               + selfInteract * activityBatch[kex]);
      VBatch[k]         = exp_tau * VBatch[k] + currentDrive + LCAMomentumRate * prevDriveBatch[k];
      prevDriveBatch[k] = currentDrive;
   }
   return PV_SUCCESS;
}

KERNEL
int applyGSyn_ISTALayer(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *V,
      MEM_GLOBAL float *GSynHead,
      MEM_GLOBAL float *activity,
      double *dtAdapt,
      float tau,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up,
      float VThresh) {
   int kbatch;
   MEM_GLOBAL float *GSynError = &GSynHead[CHANNEL_EXC * nbatch * numNeurons]; // weighted input
#ifndef PV_USE_CUDA
#ifdef PV_USE_OPENMP_THREADS
#pragma omp parallel for schedule(static)
#endif
   for (kbatch = 0; kbatch < numNeurons * nbatch; kbatch++)
#else
   kbatch    = getIndex();
#endif // PV_USE_CUDA
   {
      int b                            = kbatch / numNeurons;
      int k                            = kbatch % numNeurons;
      MEM_GLOBAL float *VBatch         = V + b * numNeurons;
      MEM_GLOBAL float *GSynErrorBatch = GSynError + b * numNeurons;
      // Activity extended
      MEM_GLOBAL float *activityBatch = activity + b * (nx + rt + lt) * (ny + up + dn) * nf;
      int kex                         = kIndexExtended(k, nx, ny, nf, lt, rt, dn, up);
      float sign                      = 0.0f;
      if (activityBatch[kex] != 0.0f) {
         sign = activityBatch[kex] / fabsf(activityBatch[kex]);
      }
      VBatch[k] += ((float)dtAdapt[b] / tau) * (GSynErrorBatch[k] - (VThresh * sign));
   }
   return PV_SUCCESS;
}

KERNEL
int applyGSyn_ISTALayer2(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *V,
      MEM_GLOBAL float *GSynHead,
      MEM_GLOBAL float *activity,
      double *dtAdapt,
      float tau,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up,
      float VThresh) {
   int kbatch;
   MEM_GLOBAL float *GSynError  = &GSynHead[0 * nbatch * numNeurons]; // weighted input
   MEM_GLOBAL float *GSynError2 = &GSynHead[1 * nbatch * numNeurons]; // weighted input

#ifndef PV_USE_CUDA
#ifdef PV_USE_OPENMP_THREADS
#pragma omp parallel for schedule(static)
#endif
   for (kbatch = 0; kbatch < numNeurons * nbatch; kbatch++)
#else
   kbatch    = getIndex();
#endif // PV_USE_CUDA
   {
      int b = kbatch / numNeurons;
      int k = kbatch % numNeurons;

      MEM_GLOBAL float *VBatch          = V + b * numNeurons;
      MEM_GLOBAL float *GSynErrorBatch  = GSynError + b * numNeurons;
      MEM_GLOBAL float *GSynError2Batch = GSynError2 + b * numNeurons;
      // Activity extended
      MEM_GLOBAL float *activityBatch = activity + b * (nx + rt + lt) * (ny + up + dn) * nf;

      int kex    = kIndexExtended(k, nx, ny, nf, lt, rt, dn, up);
      float sign = 0.0f;
      if (activityBatch[kex] != 0.0f) {
         sign = activityBatch[kex] / fabsf(activityBatch[kex]);
      }
      VBatch[k] += ((float)dtAdapt[b] / tau)
                   * ((GSynErrorBatch[k] - GSynError2Batch[k]) - (VThresh * sign));
   }
   return PV_SUCCESS;
}

KERNEL
int applyGSyn_ANNWhitenedLayer(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *V,
      MEM_GLOBAL float *GSynHead) {
   int k;
   MEM_GLOBAL float *GSynInput           = &GSynHead[0 * nbatch * numNeurons]; // un-whitened input
   MEM_GLOBAL float *GSynAveInput        = &GSynHead[1 * nbatch * numNeurons]; // un-whitened input
   MEM_GLOBAL float *GSynAveSquaredInput = &GSynHead[2 * nbatch * numNeurons]; // un-whitened input
#ifndef PV_USE_CUDA
#ifdef PV_USE_OPENMP_THREADS
#pragma omp parallel for schedule(static)
#endif
   for (k = 0; k < numNeurons * nbatch; k++)
#else
   k         = getIndex();
#endif // PV_USE_CUDA
   {
      // set mean to zero and standard deviation to one over patch window
      V[k] = (GSynInput[k] - GSynAveInput[k])
             / (sqrtf(GSynAveSquaredInput[k] - GSynAveInput[k] * GSynAveInput[k]) + FLT_MIN);
   }
   return PV_SUCCESS;
}

KERNEL
int setActivity_ANNLayer_vertices(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *V,
      int num_channels,
      MEM_GLOBAL float *activity,
      int numVertices,
      float *verticesV,
      float *verticesA,
      float *slopes,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up) {
   int status = setActivity_PtwiseLinearTransferLayer(
         nbatch,
         numNeurons,
         activity,
         V,
         nx,
         ny,
         nf,
         lt,
         rt,
         dn,
         up,
         numVertices,
         verticesV,
         verticesA,
         slopes);
   return status;
}

KERNEL
int setActivity_ANNLayer_threshminmax(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *V,
      int num_channels,
      MEM_GLOBAL float *activity,
      float VThresh,
      float AMin,
      float AMax,
      float AShift,
      float VWidth,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up) {
   setActivity_HyPerLayer(nbatch, numNeurons, activity, V, nx, ny, nf, lt, rt, dn, up);
   applyVThresh_ANNLayer_threshminmax(
         nbatch,
         numNeurons,
         V,
         VThresh,
         AMin,
         AShift,
         VWidth,
         activity,
         nx,
         ny,
         nf,
         lt,
         rt,
         dn,
         up);
   applyVMax_ANNLayer_threshminmax(
         nbatch, numNeurons, V, AMax, activity, nx, ny, nf, lt, rt, dn, up);
   return PV_SUCCESS;
}

KERNEL
int updateV_HyPerLCALayer(
      int nbatch,
      int numNeurons,
      int numChannels,
      MEM_GLOBAL float *V,
      MEM_GLOBAL float *GSynHead,
      MEM_GLOBAL float *activity,
      int numVertices,
      float *verticesV,
      float *verticesA,
      float *slopes,
      double *dtAdapt,
      float tau,
      float selfInteract,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up) {
   int status = PV_SUCCESS;
   if (numChannels == 2) {
      if (status == PV_SUCCESS)
         status = applyGSyn_HyPerLCALayer2(
               nbatch,
               numNeurons,
               V,
               GSynHead,
               activity,
               dtAdapt,
               tau,
               selfInteract,
               nx,
               ny,
               nf,
               lt,
               rt,
               dn,
               up);
   }
   else if (numChannels == 1) {
      if (status == PV_SUCCESS)
         status = applyGSyn_HyPerLCALayer(
               nbatch,
               numNeurons,
               V,
               GSynHead,
               activity,
               dtAdapt,
               tau,
               selfInteract,
               nx,
               ny,
               nf,
               lt,
               rt,
               dn,
               up);
   }

   if (status == PV_SUCCESS) {
      status = setActivity_PtwiseLinearTransferLayer(
            nbatch,
            numNeurons,
            activity,
            V,
            nx,
            ny,
            nf,
            lt,
            rt,
            dn,
            up,
            numVertices,
            verticesV,
            verticesA,
            slopes);
   }
   return status;
}

KERNEL
int updateV_MomentumLCALayer(
      int nbatch,
      int numNeurons,
      int numChannels,
      MEM_GLOBAL float *V,
      MEM_GLOBAL float *GSynHead,
      MEM_GLOBAL float *activity,
      MEM_GLOBAL float *prevDrive,
      int numVertices,
      float *verticesV,
      float *verticesA,
      float *slopes,
      double *dtAdapt,
      float tau,
      float LCAMomentumRate,
      float selfInteract,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up) {
   int status = PV_SUCCESS;
   if (numChannels == 2) {
      if (status == PV_SUCCESS)
         status = applyGSyn_MomentumLCALayer2(
               nbatch,
               numNeurons,
               V,
               GSynHead,
               activity,
               prevDrive,
               dtAdapt,
               tau,
               LCAMomentumRate,
               selfInteract,
               nx,
               ny,
               nf,
               lt,
               rt,
               dn,
               up);
   }
   else if (numChannels == 1) {
      if (status == PV_SUCCESS)
         status = applyGSyn_MomentumLCALayer(
               nbatch,
               numNeurons,
               V,
               GSynHead,
               activity,
               prevDrive,
               dtAdapt,
               tau,
               LCAMomentumRate,
               selfInteract,
               nx,
               ny,
               nf,
               lt,
               rt,
               dn,
               up);
   }

   if (status == PV_SUCCESS) {
      status = setActivity_PtwiseLinearTransferLayer(
            nbatch,
            numNeurons,
            activity,
            V,
            nx,
            ny,
            nf,
            lt,
            rt,
            dn,
            up,
            numVertices,
            verticesV,
            verticesA,
            slopes);
   }
   return status;
}

KERNEL
int updateV_ISTALayer(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *V,
      MEM_GLOBAL float *GSynHead,
      MEM_GLOBAL float *activity,
      float VThresh,
      MEM_GLOBAL double *dtAdapt,
      float tau,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up,
      int numChannels) {
   int status = PV_SUCCESS;
   if (numChannels == 2) {
      if (status == PV_SUCCESS)
         status = applyGSyn_ISTALayer2(
               nbatch,
               numNeurons,
               V,
               GSynHead,
               activity,
               dtAdapt,
               tau,
               nx,
               ny,
               nf,
               lt,
               rt,
               dn,
               up,
               VThresh);
   }
   else if (numChannels == 1) {
      if (status == PV_SUCCESS)
         status = applyGSyn_ISTALayer(
               nbatch,
               numNeurons,
               V,
               GSynHead,
               activity,
               dtAdapt,
               tau,
               nx,
               ny,
               nf,
               lt,
               rt,
               dn,
               up,
               VThresh);
   }
   if (status == PV_SUCCESS)
      status = setActivity_HyPerLayer(nbatch, numNeurons, activity, V, nx, ny, nf, lt, rt, dn, up);
   return status;
}

KERNEL
int updateV_SigmoidLayer() {
   return PV_SUCCESS; // sourcelayer is responsible for updating V.
}

KERNEL
int applyVMax_ANNLayer_threshminmax(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *V,
      float AMax,
      MEM_GLOBAL float *activity,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up) {
   if (AMax < FLT_MAX) {
      int kbatch = 0;
#ifndef PV_USE_CUDA
#ifdef PV_USE_OPENMP_THREADS
#pragma omp parallel for schedule(static)
#endif
      for (kbatch = 0; kbatch < numNeurons * nbatch; kbatch++)
#else
      kbatch = getIndex();
#endif // PV_USE_CUDA
      {
         int b                           = kbatch / numNeurons;
         int k                           = kbatch % numNeurons;
         MEM_GLOBAL float *activityBatch = activity + b * (nx + lt + rt) * (ny + up + dn) * nf;
         int kex                         = kIndexExtended(k, nx, ny, nf, lt, rt, dn, up);
         if (activityBatch[kex] > AMax) {
            activityBatch[kex] = AMax;
         }
      }
   }
   return PV_SUCCESS;
}

KERNEL
int applyVThresh_ANNLayer_threshminmax(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *V,
      float VThresh,
      float AMin,
      float AShift,
      float VWidth,
      MEM_GLOBAL float *activity,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up) {
   if (VThresh > -FLT_MAX) {
      int kbatch = 0;
#ifndef PV_USE_CUDA
#ifdef PV_USE_OPENMP_THREADS
#pragma omp parallel for schedule(static)
#endif
      for (kbatch = 0; kbatch < numNeurons * nbatch; kbatch++)
#else
      kbatch = getIndex();
      ;
#endif // PV_USE_CUDA
      {
         int b                           = kbatch / numNeurons;
         int k                           = kbatch % numNeurons;
         MEM_GLOBAL float *VBatch        = V + b * numNeurons;
         MEM_GLOBAL float *activityBatch = activity + b * (nx + lt + rt) * (ny + up + dn) * nf;
         int kex                         = kIndexExtended(k, nx, ny, nf, lt, rt, dn, up);
         if (VBatch[k] < VThresh) {
            activityBatch[kex] = AMin;
         }
         else if (VBatch[k] < VThresh + VWidth) {
            activityBatch[kex] =
                  AMin + (VThresh + VWidth - AShift - AMin) * (VBatch[k] - VThresh) / VWidth;
         }
         else {
            activityBatch[kex] -= AShift;
         }
      }
   }
   return PV_SUCCESS;
}

KERNEL
int setActivity_HyPerLayer(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *A,
      MEM_GLOBAL float const *V,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up) {
   int kbatch;
#ifndef PV_USE_CUDA
#ifdef PV_USE_OPENMP_THREADS
#pragma omp parallel for schedule(static)
#endif
   for (kbatch = 0; kbatch < numNeurons * nbatch; kbatch++)
#else
   kbatch = getIndex();
#endif // PV_USE_CUDA
   {
      int b                          = kbatch / numNeurons;
      int k                          = kbatch % numNeurons;
      MEM_GLOBAL float *ABatch       = A + b * ((nx + lt + rt) * (ny + up + dn) * nf);
      MEM_GLOBAL float const *VBatch = V + b * numNeurons;
      int kex                        = kIndexExtended(k, nx, ny, nf, lt, rt, dn, up);
      ABatch[kex]                    = VBatch[k];
   }
   return PV_SUCCESS;
}

KERNEL
int setActivity_PtwiseLinearTransferLayer(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *A,
      MEM_GLOBAL float *V,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up,
      int numVertices,
      float *verticesV,
      float *verticesA,
      float *slopes) {
   int kbatch;
   int last = numVertices - 1;
#ifndef PV_USE_CUDA
#ifdef PV_USE_OPENMP_THREADS
#pragma omp parallel for
#endif
   for (kbatch = 0; kbatch < numNeurons * nbatch; kbatch++)
#else
   kbatch = getIndex();
#endif // PV_USE_CUDA
   {
      int b         = kbatch / numNeurons;
      int k         = kbatch % numNeurons;
      float *VBatch = V + b * numNeurons;
      float *ABatch = A + b * (nx + lt + rt) * (ny + up + dn) * nf;
      int kex       = kIndexExtended(k, nx, ny, nf, lt, rt, dn, up);
      int v;
      float potential = VBatch[k];
      float activity  = 0.0f;

      if (potential < verticesV[0]) {
         activity = verticesA[0] + slopes[0] * (potential - verticesV[0]);
      }
      else if (potential >= verticesV[last]) {
         activity = verticesA[last] + slopes[numVertices] * (potential - verticesV[last]);
      }
      else {
         for (v = 0; v < last; v++) {
            if (potential < verticesV[v]) {
               break; // makes the jumps continuous from the right.  TODO: allow user control over
               // value at jump
            }
            if (potential == verticesV[v]) {
               activity = verticesA[v];
            }
            else if (potential > verticesV[v] && potential < verticesV[v + 1]) {
               activity = verticesA[v] + slopes[v + 1] * (potential - verticesV[v]);
            }
         }
      }
      ABatch[kex] = activity;
   }
   return PV_SUCCESS;
}

KERNEL
int setActivity_GapLayer(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *A,
      MEM_GLOBAL float *V,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up,
      int orig_lt,
      int orig_rt,
      int orig_dn,
      int orig_up,
      MEM_GLOBAL float *checkActive,
      float ampSpikelet) {
   int kbatch;
#ifndef PV_USE_CUDA
#ifdef PV_USE_OPENMP_THREADS
#pragma omp parallel for schedule(static)
#endif
   for (kbatch = 0; kbatch < numNeurons * nbatch; kbatch++)
#else
   kbatch = getIndex();
#endif // PV_USE_CUDA
   {
      int b                              = kbatch / numNeurons;
      int k                              = kbatch % numNeurons;
      MEM_GLOBAL float *ABatch           = A + b * ((nx + lt + rt) * (ny + up + dn) * nf);
      MEM_GLOBAL float *VBatch           = V + b * numNeurons;
      MEM_GLOBAL float *checkActiveBatch = checkActive + b * numNeurons;
      int kex                            = kIndexExtended(k, nx, ny, nf, lt, rt, dn, up);
      int kexorig = kIndexExtended(k, nx, ny, nf, orig_lt, orig_rt, orig_dn, orig_up);
      ABatch[kex] = VBatch[k];
      if (checkActiveBatch[kexorig] > 0.0f) {
         ABatch[kex] += ampSpikelet;
      }
   }
   return PV_SUCCESS;
}

KERNEL
int setActivity_SigmoidLayer(
      int nbatch,
      int numNeurons,
      MEM_GLOBAL float *A,
      MEM_GLOBAL float *V,
      int nx,
      int ny,
      int nf,
      int lt,
      int rt,
      int dn,
      int up,
      float VthRest,
      float Vrest,
      float sigmoid_alpha,
      bool sigmoid_flag,
      bool inverse_flag,
      float dt) {
   float Vth       = (VthRest + Vrest) / 2.0f;
   float sig_scale = -logf(1.0f / sigmoid_alpha - 1.0f) / (Vth - Vrest);
   if (!sigmoid_flag) {
      sig_scale = sig_scale / logf(3.0f);
      // If sigmoid_flag is off, A is a piecewise linear function of V, with slope of sig_scale/2 at
      // V=Vth, truncated to have minimum value 0 and maximum value 1.
      // The log(3) term makes alpha=1/4 have the slope such that V reaches 0 at Vrest, and V
      // reaches 1 at VthRest.  Without it, that alpha happens at 0.26894...
   }

   int kbatch;
#ifndef PV_USE_CUDA
#ifdef PV_USE_OPENMP_THREADS
#pragma omp parallel for schedule(static)
#endif
   for (kbatch = 0; kbatch < numNeurons; kbatch++)
#else
   kbatch = getIndex();
#endif // PV_USE_CUDA
   {
      int b                    = kbatch / numNeurons;
      int k                    = kbatch % numNeurons;
      MEM_GLOBAL float *ABatch = A + b * ((nx + lt + rt) * (ny + up + dn) * nf);
      MEM_GLOBAL float *VBatch = V + b * numNeurons;
      int kex                  = kIndexExtended(k, nx, ny, nf, lt, rt, dn, up);
      float activity           = 0.0f;
      if (!sigmoid_flag) {
         activity = 0.5f - (VBatch[k] - Vth) * sig_scale / 2.0f;
         activity = activity < 0.0f ? 0.0f : activity;
         activity = activity > 1.0f ? 1.0f : activity;
      }
      else {
         activity = 1.0f / (1.0f + expf(2.0f * (VBatch[k] - Vth) * sig_scale));
      }
      ABatch[kex] = activity;
      if (inverse_flag) {
         ABatch[kex] = 1.0f - ABatch[kex];
      }
   }
   return PV_SUCCESS;
}
#endif /* UPDATESTATEFUNCTIONS_H_ */
