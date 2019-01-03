//*****************************************************************
// Iterative template routine -- BiCGSTAB
//
// BiCGSTAB solves the unsymmetric linear system Ax = b 
// using the Preconditioned BiConjugate Gradient Stabilized method
//
// BiCGSTAB follows the algorithm described on p. 27 of the 
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
BiCGSTAB(const Matrix &A, Vector &x, const Vector &b,
         const Preconditioner &M, int &max_iter, Real &tol, double thr)
{
  Real resid;
  Vector rho_1(1), rho_2(1), alpha(1), beta(1), omega(1);
  Vector p, phat, s, shat, t, v;

  //GK:New-sum//                                                                                                                                                                             
  Vector rho_1_(1), rho_2_(1), alpha_(1), beta_(1), omega_(1);
  Real resid_;
  double checksum_scaler;
  Vector checksum_A;
  double checksum_x, checksum_b, checksum_rtilde, checksum_r, checksum_p, checksum_phat, checksum_Ax, checksum_s, checksum_shat, checksum_t, checksum_v;
  VECTOR_double c = nsGetC(A);
  double d = nsComputeD(A,c);
  checksum_A = nsChecksumM(A,c,d);
  checksum_x = nsChecksumV(x);
  checksum_b = nsChecksumV(b);
  int matrix_size = A.dim(1);
  //GK:New-sum//   

  Real normb = norm(b);
  Real normb_ = norm(b);   //GK:New-sum//                                                                                                                                                    
  checksum_scaler += normb - normb_;   //GK:New-sum//

  Vector Ax = A * x;
  checksum_Ax = dot(checksum_A,x) + d * checksum_x; //GK:New-sum//  

  Vector r = b - Ax;
  checksum_r = checksum_b - checksum_Ax; //GK:New-sum//   

  Vector rtilde = r;
  checksum_rtilde = checksum_r;

  if (normb == 0.0){
    normb = 1;
    normb_ = 1;  //GK:New-sum//
    checksum_scaler += normb - normb_;  //GK:New-sum// 
  }
  
  resid = norm(r) / normb;
  resid_ =  norm(r) / normb;  //GK:New-sum//                                                                                                                                               
  checksum_scaler += resid - resid_; //GK:New-sum// 
  if (resid <= tol) {
    tol = resid;
    max_iter = 0;
    return 0;
  }

#ifdef HISTORY
  FILE *fp = fopen ("bicgstab_history.txt", "wb+");
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
    rho_1_(0) = dot(rtilde, r); //GK:New-sum//                                                                                                                                             
    checksum_scaler +=  rho_1(0) -  rho_1_(0); //GK:New-sum//         

    if (rho_1(0) == 0) {
      tol = norm(r) / normb;
      /****************/
      /*GK: Error detection, Finalize*/
      /*****************/
      dFinalize();
      /****************/

      return 2;
    }

    if (i == 1){
      p = r;
      checksum_p = checksum_r;    //GK:New-sum//     
    }
    else {
      beta(0) = (rho_1(0)/rho_2(0)) * (alpha(0)/omega(0));
      beta_(0) = (rho_1(0)/rho_2(0)) * (alpha(0)/omega(0)); //GK:New-sum//
      checksum_scaler +=  beta_(0) - beta(0); //GK:New-sum//
      p = r + beta(0) * (p - omega(0) * v);
      checksum_p = checksum_r + beta(0) * (checksum_p - omega(0) * checksum_v); //GK:New-sum//   
    }

    phat = M.solve(p);
    //checksum_phat = (checksum_p - dot(checksum_M,phat)) / d;    ///////////////MISSING///////////////                                                                                   
    checksum_phat = dot(c,phat); //GK:New-sum//  

    v = A * phat;
    checksum_v = dot(checksum_A,phat) + d * checksum_phat; //GK:New-sum//

    alpha(0) = rho_1(0) / dot(rtilde, v);
    alpha_(0) = rho_1(0) / dot(rtilde, v); //GK:New-sum// 
    checksum_scaler += alpha_(0) - alpha(0);

    s = r - alpha(0) * v;
    checksum_s = checksum_r - alpha(0) * checksum_v; //GK:New-sum// 

    resid = norm(s)/normb;
    resid_ = norm(s)/normb; //GK:New-sum//    
    checksum_scaler += resid_ - resid; //GK:New-sum//

    if (resid < tol) {
      x += alpha(0) * phat;
      checksum_x += alpha(0) * checksum_phat; //GK:New-sum//

      tol = resid;
      // Joseph: a zero path that was not accounted
      max_iter = i;
      /****************/
      /*GK: Error detection, Finalize*/
      /*****************/
      dFinalize();
      /****************/

      return 0;
    }

#ifdef HISTORY
    fprintf(fp, "%lf,", resid);
#endif

    shat = M.solve(s);
    //checksum_shat = (checksum_s - dot(checksum_M,shat)) / d;    ///////////////MISSING///////////////                                                                                    
    checksum_shat = dot(c,shat); //GK:New-sum//   

    t = A * shat;
    checksum_t = dot(checksum_A,shat) + d * checksum_shat; //GK:New-sum//   

    omega(0) = dot(t,s) / dot(t,t);
    // Ask Joseph,  previous version omega = dot(t,s) / dot(t,t);
    omega_(0) = dot(t,s) / dot(t,t); //GK:New-sum//  
    checksum_scaler += omega_(0) - omega(0); //GK:New-sum//  
   

    x += alpha(0) * phat + omega(0) * shat;
    checksum_x += alpha(0) * checksum_phat + omega(0) * checksum_shat; //GK:New-sum// 


    r = s - omega(0) * t;
    checksum_r = checksum_s - omega(0) * checksum_t; //GK:New-sum//    

    rho_2(0) = rho_1(0);
    rho_2_(0) = rho_1(0); //GK:New-sum//  
    checksum_scaler += rho_2_(0) - rho_2(0);  //GK:New-sum//  
    
    resid = norm(r) / normb;
    resid_ = norm(r) / normb; //GK:New-sum// 
    checksum_scaler += resid_ - resid; //GK:New-sum// 

    if (resid < tol) {
      r = b - A * x;
      if ((resid = norm(r) / normb) < tol) {

         tol = resid;
         max_iter = i;
	 /****************/
	 /*GK: Error detection, Finalize*/
	 /*****************/
	 dFinalize();
	 /****************/

         return 0;
      }
      else{
         printf ("Activated restart: %d\n", i);
       }

    }

    if (omega(0) == 0) {
      tol = norm(r) / normb;
      /****************/
      /*GK: Error detection, Finalize*/
      /*****************/
      dFinalize();
      /****************/

      return 3;
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