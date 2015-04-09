/*
 * pv.cpp
 *
 */


#include <columns/buildandrun.hpp>
#include "PlasticConnTestLayer.hpp"
#include "PlasticTestConn.hpp"
#include "PlasticConnTestProbe.hpp"

#define MAIN_USES_CUSTOMGROUPS

#ifdef MAIN_USES_CUSTOMGROUPS

// CustomGroupHandler is for adding objects not supported by CoreParamGroupHandler.
class CustomGroupHandler : public PV::ParamGroupHandler {
public:
   CustomGroupHandler() {}
   virtual ~CustomGroupHandler() {}
   // getGroupType should return one of UnrecognizedGroupType, LayerGroupType, ConnectionGroupType,
   // ProbeGroupType, WeightInitializerGroupType, WeightNormalizerGroupType based on the type of the object
   // corresponding to the input keyword.

   virtual ParamGroupType getGroupType(char const * keyword) {
      ParamGroupType result = UnrecognizedGroupType;
      if (keyword == NULL) { return result; }
      else if (!strcmp(keyword, "PlasticConnTestLayer")) { result = LayerGroupType; }
      else if (!strcmp(keyword, "PlasticTestConn")) { result = ConnectionGroupType; }
      else if (!strcmp(keyword, "PlasticConnTestProbe")) { result = ProbeGroupType; }
      return result;
   }

   // Uncomment and define the methods below for any custom keywords that need to be handled.
   virtual HyPerLayer * createLayer(char const * keyword, char const * name, HyPerCol * hc) {
      HyPerLayer * addedLayer = NULL;
      if (keyword == NULL || getGroupType(keyword) != LayerGroupType) { return addedLayer; }
      else if (!strcmp(keyword, "PlasticConnTestLayer")) {
         addedLayer = new PlasticConnTestLayer(name, hc);
      }
      if (addedLayer==NULL) {
         fprintf(stderr, "Rank %d process unable to add %s \"%s\"\n", hc->columnId(), keyword, name);
         exit(EXIT_FAILURE);
      }
      return addedLayer;
   }

   virtual BaseConnection * createConnection(char const * keyword, char const * name, HyPerCol * hc, InitWeights * weightInitializer, NormalizeBase * weightNormalizer) {
      BaseConnection * addedConn = NULL;
      if (keyword == NULL || getGroupType(keyword) != ConnectionGroupType) { return addedConn; }
      else if (!strcmp(keyword, "PlasticTestConn")) {
         addedConn = new PlasticTestConn(name, hc, weightInitializer, weightNormalizer);
      }
      if (addedConn==NULL) {
         fprintf(stderr, "Rank %d process unable to add %s \"%s\"\n", hc->columnId(), keyword, name);
         exit(EXIT_FAILURE);
      }
      return addedConn;
   }

   virtual BaseProbe * createProbe(char const * keyword, char const * name, HyPerCol * hc) {
      BaseProbe * addedProbe = NULL;
      if (keyword == NULL || getGroupType(keyword) != ProbeGroupType) { return addedProbe; }
      else if (!strcmp(keyword, "PlasticConnTestProbe")) {
         addedProbe = new PlasticConnTestProbe(name, hc);
      }
      if (addedProbe==NULL) {
         fprintf(stderr, "Rank %d process unable to add %s \"%s\"\n", hc->columnId(), keyword, name);
         exit(EXIT_FAILURE);
      }
      return addedProbe;
    }
   // virtual InitWeights * createWeightInitializer(char const * keyword, char const * name, HyPerCol * hc) { return NULL; }
   // virtual NormalizeBase * createWeightNormalizer(char const * keyword, char const * name, HyPerCol * hc) { return NULL; }
};

#endif // MAIN_USES_ADDCUSTOM

int main(int argc, char * argv[]) {

   int status;
#ifdef MAIN_USES_CUSTOMGROUPS
   ParamGroupHandler * customGroupHandler = new CustomGroupHandler();
   status = buildandrun(argc, argv, NULL, NULL, &customGroupHandler, 1);
   delete customGroupHandler;
#else
   status = buildandrun(argc, argv);
#endif // MAIN_USES_CUSTOMGROUPS
   return status==PV_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
}
