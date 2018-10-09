#ifndef SEGMENTIFY_HPP_
#define SEGMENTIFY_HPP_

#include "HyPerLayer.hpp"
#include "components/OriginalLayerNameParam.hpp"
#include "layers/SegmentLayer.hpp"

namespace PV {

class Segmentify : public PV::HyPerLayer {
  public:
   Segmentify(const char *name, HyPerCol *hc);
   virtual Response::Status allocateDataStructures() override;
   virtual bool activityIsSpiking() override { return false; }
   virtual ~Segmentify();

  protected:
   Segmentify();
   int initialize(const char *name, HyPerCol *hc);
   virtual void createComponentTable(char const *description) override;
   virtual OriginalLayerNameParam *createOriginalLayerNameParam();
   virtual LayerInputBuffer *createLayerInput();
   int ioParamsFillGroup(enum ParamsIOFlag ioFlag) override;
   void ioParam_segmentLayerName(enum ParamsIOFlag ioFlag);
   // Defines the way to reduce values within a segment
   // into a single scalar. Options are "average", "sum", and "max".
   void ioParam_inputMethod(enum ParamsIOFlag ioFlag);
   // Defines the way to fill the output segment with the
   // reduced scalar method. Options are "centroid" and "fill"
   void ioParam_outputMethod(enum ParamsIOFlag ioFlag);
   virtual void initializeActivity() override;
   virtual Response::Status
   communicateInitInfo(std::shared_ptr<CommunicateInitInfoMessage const> message) override;
   void setOriginalLayer();

   virtual Response::Status updateState(double timef, double dt) override;

   float calcNormDist(float xVal, float mean, float binSigma);

  private:
   int initialize_base();

  protected:
   int checkLabelValBuf(int newSize);
   int buildLabelToIdx(int batchIdx);
   int calculateLabelVals(int batchIdx);
   int setOutputVals(int batchIdx);

   HyPerLayer *mOriginalLayer = nullptr;
   char *segmentLayerName;
   SegmentLayer *segmentLayer;

   // Reusing this buffer for batches
   // Map to go from label to index into labelVals
   std::map<int, int> labelToIdx;
   // Matrix to store values (one dim for features, one for # labels
   int numLabelVals;
   int *labelIdxBuf;
   float **labelVals;
   int **labelCount;

   char *inputMethod;
   char *outputMethod;

}; // class Segmentify

} /* namespace PV */
#endif
