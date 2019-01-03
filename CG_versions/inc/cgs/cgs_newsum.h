//*****************************************************************
// Iterative template routine -- CGS
//
// CGS solves the unsymmetric linear system Ax = b 
// using the Conjugate Gradient Squared method
//
// CGS follows the algorithm described on p. 26 of the 
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
CGS(const Matrix &A, Vector &x, const Vector &b,
    const Preconditioner &M, int &max_iter, Real &tol, double thr)
{
  Real resid;
  Vector rho_1(1), rho_2(1), alpha(1), beta(1);
  Vector p, phat, q, qhat, vhat, u, uhat;

  /****************/
  /* GK: Error detection, Init*/
  /*****************/
  dInit();
  /****************/

  //GK:New-sum//                                                                                                                                                                              
  Vector alpha_(1), beta_(1), rho_1_(1), rho_2_(1);
  Real resid_;
  double checksum_scaler, tol_;
  Vector checksum_A;
  double checksum_x, checksum_b, checksum_Ax, checksum_r, checksum_rtilde, checksum_p, checksum_phat;
  double checksum_q, checksum_qhat, checksum_vhat, checksum_u, checksum_uhat;
  VECTOR_double c = nsGetC(A);
  double d = nsComputeD(A,c);
  checksum_A = nsChecksumM(A,c,d);
  checksum_x = nsChecksumV(x);
  checksum_b = nsChecksumV(b);
  int matrix_size = A.dim(1);
  //GK:New-sum//     

  Real normb = norm(b);
  Real normb_ = norm(b); //GK:New-sum// 
  checksum_scaler += normb - normb_; //GK:New-sum//    

  Vector Ax = A * x;
  checksum_Ax = dot(checksum_A,x) + d * checksum_x; //GK:New-sum//

  Vector r = b - Ax;
  checksum_r = checksum_b - checksum_Ax; //GK:New-sum//    

  Vector rtilde = r;
  checksum_rtilde = checksum_r; //GK:New-sum//


  if (normb == 0.0){
    normb = 1;
    normb_ = 1;  //GK:New-sum//
    checksum_scaler += normb - normb_;  //GK:New-sum// 
  }
  
  resid = norm(r) / normb;
  resid_ =  norm(r) / normb;  //GK:New-sum//                                                                                                                                                 
  checksum_scaler += resid - resid_; //GK:New-sum// 

  if (resid  <= tol) {
    tol = resid;
    max_iter = 0;
    return 0;
  }

#ifdef HISTORY 
  FILE *fp = fopen ("cgs_history.txt", "wb+");
  if(fp == NULL){
    fprintf(stderr, "Could not open file correctly\n");
    exit(1);
  }
#endif
  
  Vector checksum_M = M.nsChecksum(d); //GK:New-sum//   
  for (int i = 1; i <= max_iter; i++) {
    liter = i;
#if defined INSERT_MULTIBF_FAULT
    insert_fault(r, i);
#else
    insert_fault(r, i, 1);
#endif
    
    rho_1(0) = dot(rtilde, r);
    rho_1_(0) = dot(rtilde, r);  //GK:New-sum//    
    checksum_scaler +=  rho_1(0) -  rho_1_(0);  //GK:New-sum//    
    
    if (rho_1(0) == 0) {
      tol = norm(r) / normb;
      tol_ = norm(r) / normb;  //GK:New-sum//    
      checksum_scaler += tol - tol_;  //GK:New-sum//    
      /****************/
      /*GK: Error detection, Finalize*/
      /*****************/
      dFinalize();
      /****************/

      return 2;
    }

    if (i == 1) {
      u = r;
      checksum_u = checksum_r;  //GK:New-sum//    
      p = u;
      checksum_p = checksum_u;  //GK:New-sum//    
      
    } else {
      beta(0) = rho_1(0) / rho_2(0);
      beta_(0) = rho_1(0) / rho_2(0);  //GK:New-sum//    
      checksum_scaler +=  beta(0) -  beta(0);  //GK:New-sum//    

      u = r + beta(0) * q;
      checksum_u = checksum_r + beta(0) * checksum_q;  //GK:New-sum//    

      p = u + beta(0) * (q + beta(0) * p);
      checksum_p = checksum_u + beta(0) * (checksum_q + beta(0) * checksum_p);  //GK:New-sum//    

    }

    phat = M.solve(p);
    //checksum_phat = (checksum_p - dot(checksum_M,phat)) / d;    ///////////////MISSING///////////////                                                                                     
    checksum_phat = dot(c,phat); //GK:New-sum//     

    vhat = A*phat;
    checksum_q = dot(checksum_A,phat) + d * checksum_phat; //GK:New-sum//    

    alpha(0) = rho_1(0) / dot(rtilde, vhat);
    alpha_(0) = rho_1(0) / dot(rtilde, vhat);  //GK:New-sum//    
    checksum_scaler += alpha(0) - alpha_(0);  //GK:New-sum//    

    q = u - alpha(0) * vhat;
    checksum_q = checksum_u - alpha(0) * checksum_vhat;  //GK:New-sum//    

    uhat = M.solve(u + q);
    //checksum_uhat = ((checksum_u + checksum_q) - dot(checksum_M,uhat)) / d;    ///////////////CHECK!!! MISSING///////////////            
    checksum_uhat = dot(c,uhat); //GK:New-sum//

    x += alpha(0) * uhat;
    checksum_x += alpha(0) * checksum_uhat;  //GK:New-sum//    

    qhat = A * uhat;
    checksum_qhat = dot(checksum_A,uhat) + d * checksum_uhat;  //GK:New-sum//    

    r -= alpha(0) * qhat;
    checksum_r -= alpha(0) * checksum_qhat;  //GK:New-sum//    

    rho_2(0) = rho_1(0);
    rho_2_(0) = rho_1(0);  //GK:New-sum//    
    checksum_scaler += rho_2_(0) - rho_2(0);  //GK:New-sum//    


    resid = norm(r) / normb;
    resid_ = norm(r) / normb;  //GK:New-sum//    
    checksum_scaler += resid - resid_;  //GK:New-sum//    

    if (resid  < tol) {
#if defined ACTIVATION
      r = b - A*x;
      if ((resid = norm(r) / normb) <= tol) {
#endif
         tol = resid;
         max_iter = i;

	 /****************/
         /*GK: Error detection, Finalize*/
         /*****************/
         dFinalize();
         /****************/

         return 0;
#if defined ACTIVATION
      }
      else{
         printf ("Activated restart: %d\n", i);
       }
#endif

    }

    /****************/
    /*GK: Error detection, Check SDC*/
    /*****************/
    double ex1 = abs(checksum_r - dot(c,r))/matrix_size;
    double ex2 = abs(checksum_x - dot(c,x))/matrix_size;
    dCheck(thr, ex1, ex2, checksum_scaler, i);
    /*****************/

#ifdef HISTORY
    fprintf(fp, "%lf\n", resid);
#endif    
  }
#ifdef HISTORY
  fclose(fp);
#endif  
  tol = resid;

  /****************/
  /*GK: Error detection, Finalize*/
  /*****************/
  dFinalize();
  /****************/

  return 1;
}

