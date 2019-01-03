//*****************************************************************
// Iterative template routine -- CG
//
// CG solves the symmetric positive definite linear
// system Ax=b using the Conjugate Gradient method.
//
// CG follows the algorithm described on p. 15 in the 
// SIAM Templates book.
//
// The return value indicates convergence within max_iter (input)
// iterations (0), or no convergence within max_iter iterations (1).
//
// Upon successful return, output arguments have the following values:
//  
//        x  --  approximate solution to Ax = b
// max_iter  --  the number of iterations performed before the
//               tolerance was reached
//      tol  --  the residual after the final iteration
//  
//*****************************************************************

extern long liter;
template < class Matrix, class Vector, class Preconditioner, class Real >
int 
CG(const Matrix &A, Vector &x, const Vector &b,
   const Preconditioner &M, int &max_iter, Real &tol)
{
  Real resid;
  Vector p, z, q;
  Vector alpha(1), beta(1), rho(1), rho_1(1);

  Real normb = norm(b);
  Vector r = b - A*x;

  /****************/
  /* GK: Error detection, Init*/
  /*****************/
  dInit();
  /****************/

  
#if defined(AID_PROTECT_R)
  SDC_Protect("r", &r[0], SDC_DOUBLE, 0, 0, 0, 0, r.dim());  
#else
  SDC_Protect("rid", &resid, SDC_DOUBLE, 0, 0, 0, 0, 1);   
#endif

  if (normb == 0.0) 
    normb = 1;
  
  
  if ((resid = norm(r) / normb) <= tol) {
    tol = resid;
    max_iter = 0;
    return 0;
  }
 
  //cout << "R: " << r[r.dim()*.4] << " RESID: " << resid << endl;
/*  FILE *fp;
  fp = fopen ("sla_conv.csv", "wb+");
  if(fp == NULL){
     printf("Error in opening the convergance file: %s %d\n", __FILE__, __LINE__);
     exit(19);
  }*/

  for (int i = 1; i <= max_iter; i++) {
    liter = i;
#if defined INSERT_MULTIBF_FAULT
    insert_fault(r, i);
#else
    insert_fault(r, i, 1);
#endif
    z = M.solve(r);
    rho(0) = dot(r, z);
    
    if (i == 1)
      p = z;
    else {
      beta(0) = rho(0) / rho_1(0);
      p = z + beta(0) * p;
    }

    q = A*p;
    alpha(0) = rho(0) / dot(p, q);
    
    x += alpha(0) * p;
    r -= alpha(0) * q;
    
    if ((resid = norm(r) / normb) <= tol) {
#if defined ACTIVATION
      r = b - A*x;
      if ((resid = norm(r) / normb) <= tol) {
#endif
         tol = resid;
         max_iter = i;
	 /****************/
	 /*GK: Error detection, Check SDC*/
	 /*****************/
	 dCheck(i);
	 /*****************/
	 
	 /****************/
	 /*GK: Error detection, Finalize*/
	 /*****************/
         dFinalize();
	 /****************/
	 
	 /****************/
	 /* GK: Error detection, Print results*/
	 /*****************/
         dPrintDetectResults();
	 /****************/
         return 0;
#if defined ACTIVATION
      }     
      else{
         printf ("Activated restart: %d\n", i);
       }
#endif
    }

    //cout << r[r.dim()*.4] << "," << resid << endl;
    rho_1(0) = rho(0);

    /****************/
    /*GK: Error detection, Check SDC*/
    /*****************/
    dCheck(i);
    /*****************/

  }
  //fclose(fp); 
  tol = resid;

  /****************/
  /*GK: Error detection, Finalize*/
  /*****************/
  dFinalize();
  /****************/

  /****************/              
  /* GK: Error detection, Print results*/ 
  /*****************/
  dPrintDetectResults();
  /****************/

  return 1;
}

