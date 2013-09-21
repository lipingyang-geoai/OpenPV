/*
 * pv.cpp
 *
 */


#include <columns/buildandrun.hpp>
#include "CPTestInputLayer.hpp"
#include "VaryingKernelConn.hpp"
#include "VaryingHyPerConn.hpp"

void * customgroup(const char * keyword, const char * name, HyPerCol * hc);
int customexit(HyPerCol * hc, int argc, char * argv[]);


int main(int argc, char * argv[]) {

   bool argerr = false;
   int reqrtn = 0;
   if (argc > 2) argerr = 2;
   else if (argc == 2) {
      argerr = strcmp(argv[1], "--require-return");
      reqrtn = 1;
   }
   if (argerr) {
      fprintf(stderr, "%s: run without input arguments (except for --require-return); the necessary arguments are hardcoded.\n", argv[0]);
      exit(EXIT_FAILURE);
   }
#ifdef PV_USE_MPI
   MPI_Init(&argc, &argv);
   int rank;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif // PV_USE_MPI

#undef REQUIRE_RETURN
#ifdef REQUIRE_RETURN
   int charhit;
   fflush(stdout);
   if( rank == 0 ) {
      printf("Hit enter to begin! ");
      fflush(stdout);
      charhit = getc(stdin);
   }
#ifdef PV_USE_MPI
   int ierr;
   ierr = MPI_Bcast(&charhit, 1, MPI_INT, 0, MPI_COMM_WORLD);
#endif // PV_USE_MPI
#endif // REQUIRE_RETURN

   int status;
   assert(reqrtn==0 || reqrtn==1);
   int cl_argc = 3+reqrtn;
   char * cl_args[cl_argc];
   cl_args[0] = strdup(argv[0]);
   cl_args[1] = strdup("-p");
   cl_args[2] = strdup("input/CheckpointParameters1.params");
   if (reqrtn) {
      assert(cl_argc==4);
      cl_args[3] = strdup("--require-return");
   }
   status = buildandrun(cl_argc, cl_args, NULL, NULL, &customgroup);
   if( status != PV_SUCCESS ) {
      fprintf(stderr, "%s: running with params file %s returned error %d.\n", cl_args[0], cl_args[2], status);
      exit(status);
   }

   free(cl_args[2]);
   cl_args[2] = strdup("input/CheckpointParameters2.params");
   status = buildandrun(cl_argc, cl_args, NULL, &customexit, &customgroup);
   if( status != PV_SUCCESS ) {
      fprintf(stderr, "%s: running with params file %s returned error %d.\n", cl_args[0], cl_args[2], status);
   }

#ifdef PV_USE_MPI
   MPI_Finalize();
#endif // PV_USE_MPI

   for (int i=0; i<cl_argc; i++) {
      free(cl_args[i]);
   }
   return status==PV_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
}

void * customgroup(const char * keyword, const char * name, HyPerCol * hc) {
   void * addedGroup = NULL;
   PVParams * params = hc->parameters();
   char * preLayerName = NULL;
   char * postLayerName = NULL;
   if( !strcmp(keyword, "CPTestInputLayer") ) {
      addedGroup = (void *) new CPTestInputLayer(name, hc);
   }
   if( !strcmp(keyword, "VaryingKernelConn") ) {
      HyPerConn::getPreAndPostLayerNames(name, params, &preLayerName, &postLayerName);
      if( preLayerName && postLayerName ) {
         InitWeights *weightInitializer;
         weightInitializer = createInitWeightsObject(name, hc);
         if( weightInitializer == NULL ) {
            weightInitializer = getDefaultInitWeightsMethod(keyword);
         }
         const char * fileName = getStringValueFromParameterGroup(name, params, "initWeightsFile", false);
         addedGroup = (void * ) new VaryingKernelConn(name, hc, preLayerName, postLayerName, fileName, weightInitializer);
      }
   }
   if( !strcmp(keyword, "VaryingHyPerConn") ) {
      HyPerConn::getPreAndPostLayerNames(name, params, &preLayerName, &postLayerName);
      if( preLayerName && postLayerName ) {
         InitWeights *weightInitializer;
         weightInitializer = createInitWeightsObject(name, hc);
         if( weightInitializer == NULL ) {
            weightInitializer = getDefaultInitWeightsMethod(keyword);
         }
         const char * fileName = getStringValueFromParameterGroup(name, params, "initWeightsFile", false);
         addedGroup = (void * ) new VaryingHyPerConn(name, hc, preLayerName, postLayerName, fileName, weightInitializer);
      }
   }
   return addedGroup;
}

int customexit(HyPerCol * hc, int argc, char * argv[]) {
   int status = PV_SUCCESS;
   int rank = hc->icCommunicator()->commRank();
   int rootproc = 0;
   if( rank == rootproc ) {
      int index = hc->parameters()->value("column", "numSteps");
      const char * cpdir1 = hc->parameters()->stringValue("column", "checkpointReadDir");
      const char * cpdir2 = hc->parameters()->stringValue("column", "checkpointWriteDir");
      if(cpdir1 == NULL || cpdir2 == NULL) {
         fprintf(stderr, "%s: unable to allocate memory for names of checkpoint directories", argv[0]);
         exit(EXIT_FAILURE);
      }
      char * shellcommand;
      char c;
      const char * fmtstr = "diff -r -q %s/Checkpoint%d %s/Checkpoint%d";
      int len = snprintf(&c, 1, fmtstr, cpdir1, index, cpdir2, index);
      shellcommand = (char *) malloc(len+1);
      if( shellcommand == NULL) {
         fprintf(stderr, "%s: unable to allocate memory for shell diff command.\n", argv[0]);
         status = PV_FAILURE;
      }
      assert( snprintf(shellcommand, len+1, fmtstr, cpdir1, index, cpdir2, index) == len );
      status = system(shellcommand);
      if( status != 0 ) {
         fprintf(stderr, "system(\"%s\") returned %d\n", shellcommand, status);
         // Because system() seems to return the result of the shell command multiplied by 256,
         // and Unix only sees the 8 least-significant bits of the value returned by a C/C++ program,
         // simply returning the result of the system call doesn't work.
         // I haven't found the mult-by-256 behavior in the documentation, so I'm not sure what's
         // going on.
         status = PV_FAILURE;
      }
   }
#ifdef PV_USE_MPI
   MPI_Bcast(&status, 1, MPI_INT, rootproc, hc->icCommunicator()->communicator());
#endif // PV_USE_MPI
   return status;
}
