/*
 * WeightsPair.hpp
 *
 *  Created on: Nov 17, 2017
 *      Author: Pete Schultz
 */

#ifndef WEIGHTSPAIR_HPP_
#define WEIGHTSPAIR_HPP_

#include "components/SharedWeights.hpp"
#include "components/WeightsPairInterface.hpp"

namespace PV {

class WeightsPair : public WeightsPairInterface {
  protected:
   /**
    * List of parameters needed from the WeightsPair class
    * @name WeightsPair Parameters
    * @{
    */

   virtual void ioParam_writeStep(enum ParamsIOFlag ioFlag);
   virtual void ioParam_initialWriteTime(enum ParamsIOFlag ioFlag);
   virtual void ioParam_writeCompressedWeights(enum ParamsIOFlag ioFlag);
   virtual void ioParam_writeCompressedCheckpoints(enum ParamsIOFlag ioFlag);

   /** @} */ // end of WeightsPair parameters

  public:
   WeightsPair(char const *name, HyPerCol *hc);

   virtual ~WeightsPair();

   int respond(std::shared_ptr<BaseMessage const> message) override;

   Weights *getPreWeights() { return mPreWeights; }
   Weights *getPostWeights() { return mPostWeights; }

   bool getWriteCompressedCheckpoints() const { return mWriteCompressedCheckpoints; }

  protected:
   WeightsPair() {}

   int initialize(char const *name, HyPerCol *hc);

   virtual int setDescription() override;

   int ioParamsFillGroup(enum ParamsIOFlag ioFlag) override;

   int
   respondConnectionFinalizeUpdate(std::shared_ptr<ConnectionFinalizeUpdateMessage const> message);

   int respondConnectionOutput(std::shared_ptr<ConnectionOutputMessage const> message);

   virtual int
   communicateInitInfo(std::shared_ptr<CommunicateInitInfoMessage const> message) override;

   virtual void createPreWeights();
   virtual void createPostWeights();

   virtual int allocateDataStructures() override;

   virtual void allocatePreWeights();

   virtual void allocatePostWeights();

   virtual int registerData(Checkpointer *checkpointer) override;

   virtual int readStateFromCheckpoint(Checkpointer *checkpointer) override;

   virtual void finalizeUpdate(double timestamp, double deltaTime);

   void openOutputStateFile(Checkpointer *checkpointer);

   virtual void outputState(double timestamp);

  protected:
   double mWriteStep                = 0.0;
   double mInitialWriteTime         = 0.0;
   bool mWriteCompressedWeights     = false;
   bool mWriteCompressedCheckpoints = false;

   SharedWeights *mSharedWeights = nullptr;
   double mWriteTime             = 0.0;

   CheckpointableFileStream *mOutputStateStream = nullptr; // weights file written by outputState
};

} // namespace PV

#endif // WEIGHTSPAIR_HPP_
