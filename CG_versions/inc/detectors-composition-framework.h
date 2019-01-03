#ifndef _DETECTORS_H
#define _DETECTORS_H

#include <stdio.h>

#if defined(AID)
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

#endif

#if defined(SADAY)
#define dPartialCheck_n_times(c,v,n) (c) += (n)* *(long long*)(&(v))
#define dPartialCheck(c,v) (c) +=  *(long long*)(&(v))

#if defined(CG)
static long long def_checksum = 0;
static long long use_checksum = 0;
static long long e_def_checksum = 0;
static long long e_use_checksum = 0;

static int* cols_count;
static int* vals_count;
static int* p_new_count;
#endif
#else
#define dPartialCheck_n_times(c,v,n)
#define dPartialCheck(c,v)
#endif

inline void dInit(int n, int nz,  int *rows, int *cols, double* vals, double *r, double* p_new, double* p_old, double* q, double* x, double* y){
  //The list of detectors to be used
#if defined(AID)
  char* aidConfig;
  aidConfig = getenv("AID_CONFIG");                                                                                                                                                         
  if (aidConfig == NULL){                                                                                                                                                                  
    printf("[AID] AID_CONFIG File is not set ...\n");                                                                                                                                        
    assert(false);                                                                                                                                                                          
  }
  printf("[AID] Initializing ...\n");
  SDC_Init_serial(aidConfig);
#endif

#if defined(AID) && defined(CG)
  /** Protect data ****/
  SDC_Protect("rows", rows, SDC_INTEGER, 0, 0, 0, 0, n+1);
  //Segmentation fault
  /*
    SDC_Protect("cols", rows, SDC_INTEGER, 0, 0, 0, 0, nz);
    SDC_Protect("vals", vals, SDC_DOUBLE,  0, 0, 0, 0, nz);
    SDC_Protect("r", r, SDC_DOUBLE,  0, 0, 0, 0, n);
    SDC_Protect("p_new", p_new, SDC_DOUBLE,  0, 0, 0, 0, n);
    SDC_Protect("p_old", p_old, SDC_DOUBLE,  0, 0, 0, 0, n);
    SDC_Protect("q", q, SDC_DOUBLE,  0, 0, 0, 0, n);
    //////SDC_Protect("x", x, SDC_DOUBLE,  0, 0, 0, 0, n);
    SDC_Protect("y", y, SDC_DOUBLE,  0, 0, 0, 0, n);
  */
  /********************/
#endif

#if defined(GAGAN)
#endif

#if defined(SADAY) && defined(CG)
  printf("[SADAY] Initializing ...\n");
  cols_count = (int*)malloc(sizeof(int) * nz);
  vals_count = (int*)malloc(sizeof(int) * nz);
  p_new_count = (int*)malloc(sizeof(int) * nz);
  
  int i, i_1, k_1;
  for (i = 0; i < n; i++){
    p_new_count[i] = 3;// live-in for p_new[]                                                                                                                                             
  }// for (i = 0; i < n; i++)                                                                                                                                                                 
  for (i = 0; i < nz; i++){
    cols_count[i] = 0;
    vals_count[i] = 0;
  }// for (i = 0; i < nz; i++)                                                                                                                                                                
  for ( i_1 = 0; i_1 < n; i_1++ ) {
    for ( k_1 = rows[i_1]; k_1 < rows[i_1+1]; k_1++ ) {
      vals_count[k_1]++; // count update                                                                                                                    
      cols_count[k_1]++; // count update                                                                                                                                                    
      p_new_count[cols[k_1]]++; // count update                                                                                                                                            
    } // k_1                                                                                                                                                                                
  }// i_1 
  
  for (i = 0; i < n; i++){
    dPartialCheck(def_checksum, r[i]);//live-in: r[i] = r[i] - XX                                       
    dPartialCheck_n_times(def_checksum, p_new[i], p_new_count[i]);
    dPartialCheck(def_checksum, x[i]);
  }// for (i = 0; i < n; i++)                                                                                                                                                                 
  for (i = 0; i < nz; i++){
    dPartialCheck(def_checksum, vals[i]);
    dPartialCheck(def_checksum, cols[i]);
    dPartialCheck(e_def_checksum, vals[i]);
    dPartialCheck(e_def_checksum, cols[i]);
  }// for (i = 0; i < nz; i++)                                                                                                                                                                
  for (i = 0; i < n+1; i++){
    dPartialCheck(def_checksum, rows[i]);
    dPartialCheck(e_def_checksum, rows[i]);
  }// for (i = 0; i < nz; i++)                      
#endif
    
}

inline void dTotalCheck(int iter){
#if defined(AID)
  int result;
  result = SDC_Snapshot();
  SDC_SetMark(iter,result);
#endif

#if defined(GAGAN)
#endif
}

inline void dFinalize(int n, int nz, int *rows, int *cols, double *r, double* x, double* p_new, double* vals, int iter){
#if defined(AID)
  printf("[AID] Finalizing ...\n");
  SDC_Finalize();
#endif

#if defined(GAGAN)
#endif

#if defined(SADAY) && defined(CG)
  printf("[SADAY] Finalizing ...\n");
  
  int i;
  for (i = 0; i < n; i++){
    dPartialCheck(use_checksum, r[i]);
    dPartialCheck_n_times(use_checksum, p_new[i], p_new_count[i]);
    dPartialCheck_n_times(use_checksum, x[i], 1);
  }// for (i = 0; i < n; i++)                                                                                                                                                                 
  for (i = 0; i < nz; i++){
    dPartialCheck(e_use_checksum, vals[i]);
    dPartialCheck(e_use_checksum, cols[i]);
    dPartialCheck_n_times(def_checksum, vals[i], iter*vals_count[i]-1);
    dPartialCheck_n_times(def_checksum, cols[i], iter*cols_count[i]-1);
  }// for (i = 0; i < nz; i++)                                                                                                                                                                
  for (i = 0; i < n+1; i++){
    if (i == 0 || i == n){
      dPartialCheck_n_times(def_checksum, rows[i], iter-1);
      dPartialCheck(e_use_checksum, rows[i]);
    }// if (i == 0 || i == n)                                                                                                                                                             
    else{
      dPartialCheck_n_times(def_checksum, rows[i], 2*iter-1);
      dPartialCheck(e_use_checksum, rows[i]);
    }// else                                                                                                                                                                              
  }// for (i = 0; i < n+1; i++)  

#endif
 
}

inline void dPrintDetectResults(){
#if defined(AID)
  printf("[AID] Printing results ...\n");
  SDC_PrintDetectResult();
#endif

#if defined(SADAY) && defined(CG)  
  printf("[SADAY] Printing results ...\n");
  printf ("DEF Checksum= %Ld, USE Checksum= %Ld\n", def_checksum, use_checksum);
  printf ("EDEF Checksum= %Ld, EUSE Checksum= %Ld\n", e_def_checksum, e_use_checksum);
#endif

}

#endif
