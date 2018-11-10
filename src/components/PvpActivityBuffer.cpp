/*
 * PvpActivityBuffer.cpp
 *
 *  Created on: Aug 16, 2016
 *      Author: Austin Thresher
 */

#include "PvpActivityBuffer.hpp"
#include "columns/HyPerCol.hpp"

namespace PV {

PvpActivityBuffer::PvpActivityBuffer(char const *name, HyPerCol *hc) { initialize(name, hc); }

PvpActivityBuffer::~PvpActivityBuffer() {}

int PvpActivityBuffer::initialize(char const *name, HyPerCol *hc) {
   int status = InputActivityBuffer::initialize(name, hc);
   return status;
}

void PvpActivityBuffer::setObjectType() { mObjectType = "PvpActivityBuffer"; }

int PvpActivityBuffer::countInputImages() {
   FileStream headerStream(
         getInputPath().c_str(), std::ios_base::in | std::ios_base::binary, false);
   struct BufferUtils::ActivityHeader header = BufferUtils::readActivityHeader(headerStream);

   int pvpFrameCount = header.nBands;
   if (header.fileType == PVP_ACT_SPARSEVALUES_FILE_TYPE || header.fileType == PVP_ACT_FILE_TYPE) {
      mSparseTable = BufferUtils::buildSparseFileTable(headerStream, pvpFrameCount - 1);
   }
   return header.nBands;
}

Buffer<float> PvpActivityBuffer::retrieveData(int inputIndex) {
   // If we're playing through the pvp file like a movie, use
   // BatchIndexer to get the frame number. Otherwise, just use
   // the start_frame_index value for this batch.
   Buffer<float> result;
   BufferUtils::readActivityFromPvp<float>(
         getInputPath().c_str(), &result, inputIndex, &mSparseTable);

   return result;
}

} // namespace PV
