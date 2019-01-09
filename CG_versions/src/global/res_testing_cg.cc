#include <cstdlib>                      // System includes
#include <iostream>                    // 
#include <fstream>
#include <string>
#include <sys/time.h>
#include <stdlib.h> 
#include <cmath>
using namespace std;

#include "compcol_double.h"              // Compressed column matrix header
#include "iohb_double.h"                 // Harwell-Boeing matrix I/O header

#include "mvblasd.h"                     // MV_Vector level 1 BLAS
#include "diagpre_double.h"              // Diagonal preconditioner

//#include "/people/mutl832/projects/Hamartia/hamartia_suite/injectors/pin-injector/src/code_regions.h"
//#include "/people/mutl832/projects/adem/CG_versions/inc/online_ml.h"
#if defined(INSERT_SINGLEBF_FAULT) || defined(INSERT_RANDOM_FAULT)
#ifdef INSERT_SINGLEBF_FAULT
#include "insert_single.h"
#else
#include "insert_random.h"
#endif
long ptime;
long pmag;
double ppos;
long iters;
#endif

#if defined(INSERT_MULTIBF_FAULT)
#include "insert_multi.h"
double ilimit = 2.5; //Limit after activation 
long iters;
int isinsert;
long iiternum;
long ivpos;
int inumbfs;
int bfposes[64];
int istatnum;                                                                                                                   
int ivecid;  
double err_margin=-1;
char dsname[2048];
ifstream base_f;
#endif

#if defined(COLLECT_DATA)
#include "collect_data.h"
#endif

struct timeval t0, t1, tinj;
/* Joseph: Last iteration encountered */
long liter = -1;
long flag = 0;
void printLastIter()
{
  printf("Last iteration found: %ld\n", liter);
  if(flag != 1){
     gettimeofday(&t1, 0);
     long elapsed  = (t1.tv_sec-t0.tv_sec) * 1e6 + t1.tv_usec - t0.tv_usec;
     long elapsed_until_inj = (tinj.tv_sec-t0.tv_sec) * 1e6 + tinj.tv_usec - t0.tv_usec;
     printf("time until injection: %lld\n", elapsed_until_inj);
     printf("time for method: %lld\n", elapsed);
  }
}

double get_rmsd(VECTOR_double x1, VECTOR_double x2){
    VECTOR_double tmp = x1 - x2;
    int i;
    double nrm = 0.0;
    for(i = 0; i < x1.dim(); ++i){
       double diff = x1[i] - x2[i];
       diff = diff * diff;
       nrm += diff;
    }
    nrm /= x1.dim();
    nrm = sqrt(nrm);
    printf ("VALIDATION RESID: %lf\n", nrm);
    return nrm;
}

/* Joseph */

#define MAX_ITER 55000

#if defined AID || defined GAGAN || defined ORTH || defined SSD || defined NEWSUM
#include "detectors.h"
#endif

#ifdef fCG
# ifdef AID
#  include "cg_aid.h"
# elif defined GAGAN
#  include "cg_gagan.h"
# elif defined ORTH
#  include "cg_orth.h"
# elif defined SSD
#  include "cg_ssd.h"
# elif defined NEWSUM
#  include "cg_newsum.h"
# elif defined CONVERGE_DATA
#  include "cg_converge.h"
# elif defined ML_PREDICT
#  include "cg_ml.h"
char* sol = "cg";
std::string bfstr = "";
std::string pkl_name = "";
# elif defined COMPONENT_DATA
#  include "cg_component.h"
# elif defined HAMARTIA_DATA
#  include "cg_hamartia.h" 
# elif defined COLLECT_DATA
#  include "cg_collect_vectors.h"
# else
#  include "cg.h"                          // IML++ CG template
# endif

#elif fBICG
# ifdef AID
#  include "bicg_aid.h"
# elif defined GAGAN
#  include "bicg_gagan.h"
# elif defined ORTH
#  include "bicg_orth.h"
# elif defined SSD
#  include "bicg_ssd.h"
# elif defined NEWSUM
#  include "bicg_newsum.h"
# elif defined CONVERGE_DATA
#  include "bicg_converge.h"
# elif defined BASECONV_DATA
#  include "bicg_converge_baseline.h"
# elif defined COLLECT_DATA
#  include "bicg_collect_vectors.h"
# else
#  include "bicg.h"
# endif

#elif fBICGSTA
# ifdef AID
#  include "bicgstab_aid.h"
# elif defined GAGAN
#  include "bicgstab_gagan.h"
# elif defined ORTH
#  error " ORTH not supported"
# elif defined SSD
#  include "bicgstab_ssd.h"
# elif defined NEWSUM
#  include "bicgstab_newsum.h"
# elif defined COLLECT_DATA
#  include "bicgstab_collect_vectors.h"
# else
#  include "bicgstab.h"
# endif

#elif fCGS
# ifdef AID
#  include "cgs_aid.h"
# elif defined GAGAN
#  include "cgs_gagan.h"
# elif defined ORTH
#  error " ORTH not supported"
# elif defined SSD
#  include "cgs_ssd.h"
# elif defined NEWSUM
#  include "cgs_newsum.h"  
# elif defined CONVERGE_DATA
#  include "cgs_converge.h"
# elif defined BASECONV_DATA
#  include "cgs_converge_baseline.h"
# elif defined COLLECT_DATA
#  include "cgs_collect_vectors.h"
# else
#  include "cgs.h"
# endif

#elif fCHEBY
# ifdef AID
#  include "cheby_aid.h"
# elif defined GAGAN
#  include "cheby_gagan.h"
# elif defined ORTH
#  error " ORTH not supported"
# elif defined SSD
#  include "cheby_ssd.h"
# else
#  include "cheby.h"
# endif

#elif fICCG
# include "icpre_double.h"
# ifdef AID
#  include "cg_aid.h"
# elif defined GAGAN
#  include "cg_gagan.h"
# elif defined ORTH
#  include "cg_orth.h"
# elif defined NEWSUM
#  include "cg_newsum.h"  
# elif defined SSD
#  include "cg_ssd.h"
# elif defined CONVERGE_DATA
#  include "cg_converge.h"
# elif defined BASECONV_DATA
#  include "cg_converge_baseline.h"
# elif defined COLLECT_DATA
#  include "cg_collect_vectors.h"
# else
#  include "cg.h"
# endif


#elif fLUGMRE
# include MATRIX_H
# include "ilupre_double.h"
# ifdef AID
#  include "gmres_aid.h"
# elif defined GAGAN
#  include "gmres_gagan.h"
# elif defined ORTH
#  error " ORTH not supported"
# elif defined SSD
#  include "gmres_ssd.h"
//# elif defined NEWSUM
//#  include "gmres_newsum.h"
# else
#  include "gmres.h"
# endif

#elif fIR
# ifdef AID
#  include "ir_aid.h"
# elif defined GAGAN
#  include "ir_gagan.h"
# elif defined ORTH
#  error " ORTH not supported"
# elif defined SSD
#  include "ir_ssd.h"
# else
#  include "ir.h"
# endif


#elif fQMR
# ifdef AID
#  include "qmr_aid.h"
# elif defined GAGAN
#  include "qmr_gagan.h"
# elif defined ORTH
#  error " ORTH not supported"
# elif defined SSD
#  include "qmr_ssd.h"
# elif defined NEWSUM
#  include "qmr_newsum.h"
# elif defined COLLECT_DATA
#  include "qmr_collect_vectors_v2.h"
# else
#  include "qmr.h"
# endif


#elif fCGNR
#ifdef COLLECT_DATA
#include "cgnr_collect_vectors_v2.h"
#else
#include "cgnr.h"
#endif


#elif fCGNE
#ifdef COLLECT_DATA
#include "cgne_collect_vectors_v2.h"
#else
#include "cgne.h"
#endif


#else
#error "Not a correct option defined for the iterative solver"
#endif
/* Joseph (hackish) extensions for RB */
#include "ioRB.h"
/* Joseph (hackish) extensions for RB */

int
main(int argc, char * argv[])
{
   atexit(printLastIter);

#ifdef NO_RHS
# if  !defined(INSERT_SINGLEBF_FAULT) && !defined(INSERT_RANDOM_FAULT) && !defined(INSERT_MULTIBF_FAULT)
#  if defined (GAGAN)
  if(argc < 4)
#  elif defined (ORTH)
  if(argc < 4)
#  elif defined (NEWSUM)
    if(argc < 3)
#  else
  if(argc < 2)
#  endif

# else //!defined(INSERT_SINGLEBF_FAULT) && !defined(INSERT_RANDOM_FAULT) && !defined(INSERT_MULTIBF_FAULT)
#ifdef INSERT_MULTIBF_FAULT
    if(argc < 7)
#else
#  if defined (GAGAN)
  if(argc < 7)
#  elif defined (ORTH)
  if(argc < 7)
#  elif defined (NEWSUM)
    if(argc < 6)
#  else
  if(argc < 5)
#endif
#endif
#endif

#else
# if !defined(INSERT_SINGLEBF_FAULT) && !defined(INSERT_RANDOM_FAULT)
#  if defined (GAGAN)
    if(argc < 5)
#  elif defined (ORTH)
    if(argc < 5)
#  elif defined (NEWSUM)
    if(argc < 4)
#  else
    if(argc < 3)
#  endif

# else
#  if defined (GAGAN)
   if(argc < 9)
#  elif defined (ORTH)
   if(argc < 9)
#  elif defined (NEWSUM)
   if(argc < 8)
#  else
   if(argc < 7)
#  endif
# endif
#endif
  {
    cerr << argc << " Usage: " << argv[0] << " RBfile ";
#ifndef NO_RHS
    cerr << "RBfile_b | RBfile_mtx";
#endif  

# ifdef GAGAN
    cerr << " Window Size; " << "Threshold; ";
# elif ORTH
    cerr << " Optimal Iteration; " << "Threshold; ";
# elif NEWSUM
    cerr << " Threshold; ";
# endif


#if defined(INSERT_SINGLEBF_FAULT) || defined(INSERT_RANDOM_FAULT)
    cerr << "PTIME PPOS PMAG" << endl;
#endif 

#if defined ML_PREDICT
    cerr << "ISINSERT LINE_POSITION VECTOR_NUMBER ITERATION_NUMBER POSITION NUMBITFLIPS BITPOSITIONS PKL_FILE_NAME" << endl;
#elif defined(INSERT_MULTIBF_FAULT)
    cerr << "ISINSERT LINE_POSITION VECTOR_NUMBER ITERATION_NUMBER POSITION NUMBITFLIPS BITPOSITIONS" << endl;
#endif

    cerr << endl;
    exit(-1);
  }


  /* Joseph: Timing characteristics of algorithms */
  unsigned long long elapsed;
/*  struct timeval t0, t1;*/
  /* Joseph */
  double tol = 1.e-6;                    // Convergence tolerance
  int result, maxit = MAX_ITER;               // Maximum iterations
  char line[1024];
  int initial = 0;



#ifdef NO_RHS
  initial = 2;
#else
  initial = 3;
#endif

# if defined (GAGAN)
  int wsize = atoi(argv[initial]);
  double thr = stod(argv[initial+1]);
  initial += 2;
# elif defined (ORTH)
  int optIte = atoi(argv[initial]);
  double thr = stod(argv[initial+1]);
  initial += 2;
# elif defined (NEWSUM)
  double thr = stod(argv[initial]);
  initial += 1; 
# endif


#if defined(INSERT_SINGLEBF_FAULT) || defined(INSERT_RANDOM_FAULT) 
   ptime = atol(argv[initial+0]);  
   ppos = atof(argv[initial+1]);  
   pmag = atol(argv[initial+2]);  
   printf("ptime: %d; ppos: %f; plong: %d\n",  ptime, ppos, pmag);
#endif

#if defined(INSERT_MULTIBF_FAULT)
   isinsert=atoi(argv[initial+0]);
   istatnum=atoi(argv[initial+1]);
   ivecid =atoi(argv[initial+2]);
   iiternum=atol(argv[initial+3]);
   ivpos=atol(argv[initial+4]);
   inumbfs=atoi(argv[initial+5]);
   int arg_num=initial+6;
   int ibf;
   for (ibf=0; ibf < inumbfs; ibf++)
     bfposes[ibf] = atoi(argv[arg_num+ibf]);
   #ifdef ML_PREDICT
   if(argc <= arg_num + ibf){
     cerr << "ISINSERT LINE_POSITION VECTOR_NUMBER ITERATION_NUMBER POSITION NUMBITFLIPS BITPOSITIONS PKL_FILENAME" << endl;
     exit(-1);
   }
     
   pkl_name = argv[arg_num + ibf];
   for (ibf=0; ibf < inumbfs; ibf++){
     bfstr += "_";
     bfstr += argv[arg_num+ibf];
   }
   
   #endif
#endif

#if defined(INSERT_MUTLIBF_VECTORS)
   isinsert=atoi(argv[initial+0]);
   ilinenum =atoi(argv[initial+1]);
   ivecnum =atoi(argv[initial+2]); 
   iiternum=atol(argv[initial+3]);
   ivpos=atol(argv[initial+4]);
   inumbfs=atoi(argv[initial+5]);
   int arg_num=initial+6;
   int ibf;
   for (ibf=0; ibf < inumbfs; ibf++)
     bfposes[ibf] = atoi(argv[arg_num+ibf]);

#endif

  CompCol_Mat_double A;                  // Create a matrix
  readRB_mat(argv[1], &A);               // Read matrix data

  char dataset[2048];
  char name[2048];
  char *tokens;

  strcpy(dataset, argv[1]);

  tokens = strtok(dataset, " /,");
  while (tokens != NULL){
     if(strcmp(tokens, "rb") == 0){
        break;
     }
     strcpy(name, tokens);
     tokens = strtok(NULL, " /,");
  }

  strcpy (dataset, name);
  tokens = strtok(dataset, " .");
  if(tokens != NULL){
     strcpy(name, tokens);
  }
  else{
      printf("Format not correct for the given string: %s\n", tokens);
      exit(111);
  }

  printf ("Using dataset: %s\n", name);
  /*strcat(name, "rb");*/
  strcpy(dsname,name);
#ifdef ML_PREDICT
  if(isinsert){
    //open basefile and go to necessary point if injecting error
    //SRC_HOME/Convergence/baseline/cg/ex13/cg_ex13_baseline.dat    
    string basefilename = "../../Convergence/baseline/" +  std::string(sol) + "/" + dsname + "/" + sol + "_" + name + "_baseline.dat";
    base_f.open(basefilename);
    string s;
    //5 info lines before data starts 
    //if injection+5 is before 21th iteration, we start comparison from 0th iteration
    //*3 for each iteration data we skip because we have 3 lines of data for each iteration
    //-1 so that next getline() will get the right line
    int gotoline = 5 + (iiternum+5-21>=0?iiternum+5-21:0) * 3 - 1;
    for (int i = 0; i <= gotoline; i++)
      std::getline(base_f, s);
  }


#endif
  
  VECTOR_double b, tx(A.dim(1), 1.0), x(A.dim(1), 0.0);
#ifdef NO_RHS   
  b = A * tx;
#else
  strncpy(line, argv[2], 1024);
  char *tok = (char *)strtok(line, ".");
  char *lst = tok;
  while(tok != NULL){
     lst = tok;
     tok = (char *)strtok(NULL, ".");
  }
  if(strncmp(lst, "mtx", 4) == 0)
     readMTX_rhs(argv[2], &b);  
  else if(strncmp(lst, "rb", 3) == 0)
     readRB_rhs(argv[2], &b);               // Read rhs data
  else{
     printf("Unrecognized format file: %s %d\n", __FILE__, __LINE__);
     exit(18);
  }
#endif

   gettimeofday(&t0, 0);
#ifdef fICCG
  ICPreconditioner_double D(A);      
#else
  DiagPreconditioner_double D(A);        // Create diagonal preconditioner
#endif
  gettimeofday(&t1, 0);
  elapsed  = (t1.tv_sec-t0.tv_sec) * 1e6 + t1.tv_usec - t0.tv_usec;
  printf("Preconditioner timing: %lld\n", elapsed);

#if defined(INSERT_SINGLEBF_FAULT) || defined(INSERT_RANDOM_FAULT) || defined(INSERT_MULTIBF_FAULT)
   iters = load_tot_iters(
# ifdef fCG
       "./iters/cg.csv", 
# elif  fBICG
       "./iters/bicg.csv", 
# elif  fBICGSTA
       "./iters/bicgsta.csv", 
# elif  fCGS
       "./iters/cgs.csv", 
# elif  fCHEBY
       "./iters/cheby.csv", 
# elif  fICCG
       "./iters/iccg.csv", 
# elif  fLUGMRE
       "./iters/lugmre.csv", 
# elif  fIR
       "./iters/ir.csv", 
# elif  fQMR
       "./iters/qmr.csv",
# elif  fCGNE
       "./iters/cgne.csv",
#elif  fCGNR
       "./iters/cgnr.csv",
# endif
      name);
   if(iters <= 0){
       printf("Failure in loading the iterations from file: %ld\n", iters);
       exit(11);
   }
#endif

  gettimeofday(&t0, 0);

#ifdef fCG
#if defined(GAGAN)
  result = CG(A, x, b, D, maxit, tol, wsize, thr);   // Solve system
#elif defined(ORTH)
  result = CG(A, x, b, D, maxit, tol, optIte, thr);
#elif defined(NEWSUM)
  result = CG(A, x, b, D, maxit, tol, thr);   // Solve system  
#else
  result = CG(A, x, b, D, maxit, tol);   // Solve system
#endif

  cout << "Iterative method: Diagonal Preconditioned CG" << endl;


#elif fBICG
#if defined(GAGAN)
  result = BiCG(A, x, b, D, maxit, tol,  wsize, thr);   // Solve system
#elif defined(ORTH)
  result = BiCG(A, x, b, D, maxit, tol, optIte, thr);   // Solve system
#elif defined(NEWSUM)
  result = BiCG(A, x, b, D, maxit, tol, thr);   // Solve system 
#else
  result = BiCG(A, x, b, D, maxit, tol);   // Solve system           
#endif

  cout << "Iterative method: Diagonal Preconditioned BiCG" << endl;


#elif fBICGSTA
#if defined(GAGAN)
  result = BiCGSTAB(A, x, b, D, maxit, tol, wsize, thr);   // Solve system
#elif defined(NEWSUM)
  result = BiCGSTAB(A, x, b, D, maxit, tol, thr);   // Solve system      
#else
  result = BiCGSTAB(A, x, b, D, maxit, tol);   // Solve system
#endif
  cout << "Iterative method: Diagonal Preconditioned Stabilized BiCG" << endl;

#elif fCGS
#if defined(GAGAN)
  result = CGS(A, x, b, D, maxit, tol, wsize, thr);   // Solve system
#elif defined(NEWSUM)
  result = CGS(A, x, b, D, maxit, tol, thr);   // Solve system     
#else
  result = CGS(A, x, b, D, maxit, tol);   // Solve system
#endif
  cout << "Iterative method: Diagonal Preconditioned CG Squared" << endl;

#elif fCHEBY
  double mineig = .01;                   // eigenvalue information
  double maxeig = 3;                     // (this info for la2d8 example)
#if defined(GAGAN)
  result = CHEBY(A, x, b, D, maxit, tol, mineig, maxeig, wsize, thr);   // Solve system
#else
  result = CHEBY(A, x, b, D, maxit, tol, mineig, maxeig);   // Solve system
#endif
  cout << "Iterative method: Diagonal Preconditioned Chebyshev Method" << endl;

#elif fICCG
#if defined(GAGAN)
  result = CG(A, x, b, D, maxit, tol, wsize, thr);   // Solve system
#elif defined(ORTH)
  result = CG(A, x, b, D, maxit, tol, optIte, thr);
#elif defined(NEWSUM)
  result = CG(A, x, b, D, maxit, tol, thr);   // Solve system
#else
  result = CG(A, x, b, D, maxit, tol);   // Solve system
#endif
  cout << "Iterative method: Incomplete Cholesky Preconditioned CG" << endl;


#elif fLUGMRE
  int restart = 32;
  MATRIX_double H(restart+1, restart, 0.0); // storage for upper Hessenberg H
  CompCol_ILUPreconditioner_double M(A);    // Create ILU preconditioner
#if defined(GAGAN)
  result = GMRES(A, x, b, M, H, restart, maxit, tol, wsize, thr);   // Solve system
#else
  result = GMRES(A, x, b, M, H, restart, maxit, tol);   // Solve system
#endif
  cout << "Iterative method: Incomplete LU Preconditioned Generalized Mininum Residual Method" << endl;


#elif fIR
#if defined(GAGAN)
  result = IR(A, x, b, D, maxit, tol, wsize, thr);   // Solve system
#else
  result = IR(A, x, b, D, maxit, tol);   // Solve system
#endif
  cout << "Iterative method: Preconditioned Richardson iteration - Iterative refinement" << endl;

#elif fQMR
#if defined(GAGAN)
  result = QMR(A, x, b, D, D, maxit, tol, wsize, thr);
#elif defined(NEWSUM)
  result = QMR(A, x, b, D, D, maxit, tol, thr);   // Solve system        
#else
  result = QMR(A, x, b, D, D, maxit, tol);   // Solve system
#endif
  cout << "Iterative method: Diag Preconditioned Quasi-Minimal Residual Method" << endl;


#elif fCGNR
  ///#if defined(GAGAN)
  //result = CGNR(A, x, b, D, D, maxit, tol, wsize, thr);
  //#elif defined(NEWSUM)
  //result = CGNR(A, x, b, D, D, maxit, tol, thr);   // Solve system             
  //#else
  result = CGNR(A, x, b, D, maxit, tol);   // Solve system                                   
  //#endif
  cout << "Iterative method: CGNR" << endl;

#elif fCGNE
  //#if defined(GAGAN)
  //result = CGNE(A, x, b, D, D, maxit, tol, wsize, thr);
  //#elif defined(NEWSUM)
  //result = CGNE(A, x, b, D, D, maxit, tol, thr);   // Solve system       
  //#else
  result = CGNE(A, x, b, D, maxit, tol);   // Solve system     
  //#endif
  cout << "Iterative method: CGNE" << endl;


#else
#error "Not a correct option defined for the iterative solver"
#endif
  gettimeofday(&t1, 0);
  elapsed  = (t1.tv_sec-t0.tv_sec) * 1e6 + t1.tv_usec - t0.tv_usec;
  long elapsed_until_inj = (tinj.tv_sec-t0.tv_sec) * 1e6 + tinj.tv_usec - t0.tv_usec;
  cout << "flag = " << result << endl;
  cout << "iterations performed: " << maxit << endl;
  cout << "tolerance achieved  : " << tol << endl;
  printf("time for method: %lld\n", elapsed);
  printf("time until injection: %lld\n", elapsed_until_inj);
#ifdef VERIFY
  get_rmsd(tx,x);
#endif
#if defined(INSERT_SINGLEBF_FAULT) || defined(INSERT_RANDOM_FAULT) || defined(INSERT_MULTIBF_FAULT)
  printf("Experimental Parameters:\n");
  print_params();
#endif
  flag = 1;
  return result;
}
