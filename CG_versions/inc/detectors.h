#ifndef _DETECTORS_H
#define _DETECTORS_H

#include <stdio.h>
#include <fstream>

#if defined(AID) || defined(SSD)
extern "C"{
  int SDC_Init_serial(char *configFile);
  void SDC_Finalize();
  void SDC_Protect(char* var_name, void* data, int data_type, int r5, int r4, int r3, int r2, int r1);
  int SDC_Snapshot();
  int SDC_SetMark(int i, int res);
  void SDC_PrintDetectResult();
}

//Fix this
#define SDC_INTEGER 1000
#define SDC_FLOAT 1001
#define SDC_DOUBLE 1002

#elif defined(GAGAN)
#include "mad_detector.h"
#endif

#if defined ALL_ITERATIONS
#define MAX_DETECT_ITER 50000
int detectionResult[MAX_DETECT_ITER];
int detected_max_iter = 1;

inline void initDetectionResults(){
  for(int i=0; i < MAX_DETECT_ITER;i++)
    detectionResult[i] = -1;
}
#endif

#if defined(AID)
inline void dInit(){
  char* aidConfig;
  aidConfig = getenv("AID_CONFIG");                 
  if (aidConfig == NULL){
    printf("[AID] AID_CONFIG File is not set ...\n");
    assert(false);
  }
  printf("[AID] Initializing ...\n");
  SDC_Init_serial(aidConfig);
#if defined ALL_ITERATIONS  
  initDetectionResults();
#endif
}
#elif defined(SSD)
inline void dInit(){
  char* ssdConfig;
  ssdConfig = getenv("SSD_CONFIG");
  if (ssdConfig == NULL){
    printf("[SSD] SSD_CONFIG File is not set ...\n");
    assert(false);
  }
   printf("[SSD] Initializing ...\n");
  SDC_Init_serial(ssdConfig);
#if defined ALL_ITERATIONS
  initDetectionResults();
#endif
}
#elif defined(GAGAN)
inline void dInit(int maxit, int msize, double thr){
  SDC_Gagan_Init(maxit, msize, thr);
#if defined ALL_ITERATIONS
  initDetectionResults();
#endif
}
#else
inline void dInit(){
#if defined ALL_ITERATIONS
#if defined(ORTH)
  detected_max_iter ++; //Orth starts detection at the second iteration
#endif
  initDetectionResults();
#endif
}
#endif


#if defined(AID) || defined(SSD)
inline void dCheck(int iter){
#if defined ALL_ITERATIONS
  detected_max_iter ++;
#endif

  int result;
  result = SDC_Snapshot();
#if defined ALL_ITERATIONS
  detectionResult[iter] = result;
#endif
  SDC_SetMark(iter,result);
}
#elif defined(GAGAN)
inline void dCheck(double curRes, int iteration){
#if defined ALL_ITERATIONS
  detected_max_iter ++;
#endif

  if (SDC_Gagan_Snapshot(curRes)){
#if defined ALL_ITERATIONS
    detectionResult[iteration] = 1;
#else
    cout << "[Gagan Check] Bit error detected, terminating application" << endl;
    SDC_Gagan_Finalize();
    cout << "Failed at iteration: " << iteration << endl;
    exit(-1);
#endif
  } 
#if defined ALL_ITERATIONS
  else {
    detectionResult[iteration] = 0;
  }
#endif

  SDC_Gagan_SetMark(curRes);
}  
#elif defined(NEWSUM)
inline void dCheck(double threshold, double ex1, double ex2, double checksum_scaler, int iteration){
#if defined ALL_ITERATIONS
  detected_max_iter ++;
#endif

  //printf("GOKCEN:%0.12le;%0.12le\n",ex1,ex2);
  if ( ex1 > threshold || ex2 > threshold || checksum_scaler != 0){
#if defined ALL_ITERATIONS
    detectionResult[iteration] = 1;
#else
    cout << "[NewSum Check] Bit error detected, terminating application, Exp1= " << abs(ex1) << " Exp2= " << abs(ex2) <<"  Threshold= " <<  threshold <<
     " Checksum_Scaler: " << checksum_scaler<<endl;
    cout << "Failed at iteration: " << iteration << endl;
    exit(-1);
#endif
  }
#if defined ALL_ITERATIONS
  else{
    detectionResult[iteration] = 0;
  }
#endif

}
#elif defined(ORTH)
inline void dCheck(double threshold, double ex1, double ex2, int iteration){
#if defined ALL_ITERATIONS
  detected_max_iter ++;
#endif

  //printf("GOKCEN:%0.12le;%0.12le;",ex1,ex2);
  if ( ex1 > threshold || ex2 > threshold ){
#if defined ALL_ITERATIONS
    detectionResult[iteration] = 1;
    //printf("%d\n",1);
#else
    cout << "[Orthogonality Check] Bit error detected, terminating application, Exp1= " << abs(ex1) << " Exp2= " << abs(ex2) <<"  Threshold= " <<  threshold << endl;
    cout << "Failed at iteration: " << iteration << endl;
    exit(-1);
#endif
  }
#if defined ALL_ITERATIONS
  else{
    //printf("%d\n",0);
    detectionResult[iteration] = 0;
  }
#endif
}
#endif


inline void dFinalize(){
#if defined(AID)
  printf("[AID] Finalizing ...\n");
  SDC_Finalize();
#elif defined(SSD)
  printf("[SSD] Finalizing ...\n");
  SDC_Finalize();
#elif defined(GAGAN)
  printf("[Gagan] Finalizing ...\n");
  SDC_Gagan_Finalize();
#endif
  int first_detection=-1;
  int num_detections= 0;
  int i;
  printf("DETECTION_RESULTS:%d", -1); //for iteration zero
  for(i=1; i < detected_max_iter;i++){

    if (detectionResult[i] == 1){
      num_detections++;
      if (first_detection == -1)
	first_detection = i;
    }
    printf(",%d", detectionResult[i]);
  }
  printf("\n");
  
  printf("First Detection at: %d\n", first_detection);
  printf("Total number of Iterations: %d\n", detected_max_iter);
  printf("Total number of Detections:  %d\n",num_detections);
}

inline void dPrintDetectResults(){
#if defined(AID) || defined(SSD)
#if defined(AID)
  printf("[AID] Printing results ...\n");
#else
  printf("[SSD] Printing results ...\n");
#endif
  SDC_PrintDetectResult();
#endif
}

#endif
