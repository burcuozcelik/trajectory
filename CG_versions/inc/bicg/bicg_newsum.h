//*****************************************************************
// Iterative template routine -- BiCG
//
// BiCG solves the unsymmetric linear system Ax = b 
// using the Preconditioned BiConjugate Gradient method
//
// BiCG follows the algorithm described on p. 22 of the 
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
BiCG(const Matrix &A, Vector &x, const Vector &b,
     const Preconditioner &M, int &max_iter, Real &tol, double thr)
{
  Real resid;
  Vector rho_1(1), rho_2(1), alpha(1), beta(1);
  Vector z, ztilde, p, ptilde, q, qtilde;

  /****************/
  /* GK: Error detection, Init*/
  /*****************/
  dInit();
  /****************/

  //GK:New-sum// 
  Vector alpha_(1), beta_(1), rho_1_(1), rho_2_(1);
  Real resid_;
  double checksum_scaler;
  Vector checksum_A;
  double checksum_x, checksum_b, checksum_Ax, checksum_r, checksum_rtilde, checksum_z, checksum_ztilde, checksum_p, checksum_ptilde, checksum_q, checksum_qtilde;
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

  if (resid <= tol) {
    tol = resid;
    max_iter = 0;
    return 0;
  }


  Vector checksum_M = M.nsChecksum(d); //GK:New-sum//   

  for (int i = 1; i <= max_iter; i++) {
    liter = i;
#if defined INSERT_MULTIBF_FAULT
    insert_fault(r, i);
#else
    insert_fault(r, i, 1);
#endif

    z = M.solve(r);
    //checksum_z = (checksum_r - dot(checksum_M,z)) / d;    ///////////////MISSING///////////////
    checksum_z = dot(c,z); //GK:New-sum//

    ztilde = M.trans_solve(rtilde);
    //checksum_ztilde = (checksum_rtilde - dot(checksum_M,ztilde)) / d; /////////MISSING///////////
    checksum_ztilde = dot(c,ztilde); //GK:New-sum// 

    rho_1(0) = dot(z, rtilde);
    rho_1_(0) = dot(z, rtilde); //GK:New-sum//            
    checksum_scaler +=  rho_1(0) -  rho_1_(0); //GK:New-sum//  

    if (rho_1(0) == 0) { 
      tol = norm(r) / normb;
      max_iter = i;
      /****************/
      /*GK: Error detection, Finalize*/
      /*****************/
      dFinalize();
      /****************/

      return 2;
    }

    if (i == 1) {
      p = z;
      checksum_p = checksum_z;    //GK:New-sum//      
      ptilde = ztilde;
      checksum_ztilde = checksum_ztilde;    //GK:New-sum//      
    } else {
      beta(0) = rho_1(0) / rho_2(0);
      beta_(0) = rho_1(0) / rho_2(0);  //GK:New-sum// 
      checksum_scaler +=  beta(0) -  beta_(0); //GK:New-sum//

      p = z + beta(0) * p;
      checksum_p = checksum_z + beta(0) * checksum_p; //GK:New-sum//          

      ptilde = ztilde + beta(0) * ptilde;
      checksum_ptilde = checksum_ztilde + beta(0) * checksum_ptilde; //GK:New-sum//          
    }

    q = A * p;
    checksum_q = dot(checksum_A,p) + d * checksum_p; //GK:New-sum//      

    qtilde = A.trans_mult(ptilde);
    checksum_qtilde = dot(checksum_A,ptilde) + d * checksum_ptilde; //GK:New-sum// 

    alpha(0) = rho_1(0) / dot(ptilde, q);
    alpha_(0) = rho_1(0) / dot(ptilde, q);  //GK:New-sum// 
    checksum_scaler +=  alpha(0) -  alpha_(0); //GK:New-sum//   

    x += alpha(0) * p;
    checksum_x += alpha(0) * checksum_p;  //GK:New-sum//  

    r -= alpha(0) * q;
    checksum_r -= alpha(0) * checksum_q; //GK:New-sum//    

    rtilde -= alpha(0) * qtilde;
    checksum_rtilde -= alpha(0) * checksum_qtilde; //GK:New-sum// 

    rho_2(0) = rho_1(0);
    rho_2_(0) = rho_1(0);  //GK:New-sum//
    checksum_scaler += rho_2(0) - rho_2_(0);


    resid = norm(r) / normb;
    resid_ =  norm(r) / normb;  //GK:New-sum//
    checksum_scaler += resid - resid_;  //GK:New-sum//   

    if (resid < tol) {
#if defined ACTIVATION
       r = b - A * x;
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


  }

  tol = resid;
  /****************/
  /*GK: Error detection, Finalize*/
  /*****************/
  dFinalize();
  /****************/

  return 1;
}
  
