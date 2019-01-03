//*****************************************************************
// Iterative template routine -- QMR
//
// QMR.h solves the unsymmetric linear system Ax = b using the
// Quasi-Minimal Residual method following the algorithm as described
// on p. 24 in the SIAM Templates book.
//
//   -------------------------------------------------------------
//   return value     indicates
//   ------------     ---------------------
//        0           convergence within max_iter iterations
//        1           no convergence after max_iter iterations
//                    breakdown in:
//        2             rho
//        3             beta
//        4             gamma
//        5             delta
//        6             ep
//        7             xi
//   -------------------------------------------------------------
//   
// Upon successful return, output arguments have the following values:
//
//        x  --  approximate solution to Ax=b
// max_iter  --  the number of iterations performed before the
//               tolerance was reached
//      tol  --  the residual after the final iteration
//
//*****************************************************************


#include <math.h>

extern long liter;
template < class Matrix, class Vector, class Preconditioner1,
           class Preconditioner2, class Real >
int 
QMR(const Matrix &A, Vector &x, const Vector &b, const Preconditioner1 &M1, 
    const Preconditioner2 &M2, int &max_iter, Real &tol, double thr)
{
  Real resid;
  Vector rho(1), rho_1(1), xi(1), gamma(1), gamma_1(1), theta(1), theta_1(1);
  Vector eta(1), delta(1), ep(1), beta(1);
  Vector r, v_tld, y, w_tld, z;
  Vector v, w, y_tld, z_tld;
  Vector p, q, p_tld, d, s;

  //GK:New-sum//                                                                                                                                                                             
  Vector rho_(1), rho_1_(1), xi_(1), gamma_(1), gamma_1_(1), theta_(1), theta_1_(1);
  Vector eta_(1), delta_(1), ep_(1), beta_(1);
  Real resid_;
  double checksum_scaler;
  Vector checksum_A;
  double checksum_x, checksum_b, checksum_Ax;
  double checksum_r, checksum_v_tld, checksum_y, checksum_w_tld, checksum_z;
  double checksum_v, checksum_w, checksum_y_tld, checksum_z_tld;
  double checksum_p, checksum_q, checksum_p_tld, checksum_d, checksum_s;
  VECTOR_double c = nsGetC(A);
  double d4checksum = nsComputeD(A,c);
  checksum_A = nsChecksumM(A,c,d4checksum);
  checksum_x = nsChecksumV(x);
  checksum_b = nsChecksumV(b);
  int matrix_size = A.dim(1);
  //GK:New-sum//   

  Real normb = norm(b);
  Real normb_ = norm(b);   //GK:New-sum//
  checksum_scaler += normb - normb_;   //GK:New-sum//     
  
  Vector Ax = A * x;
  checksum_Ax = dot(checksum_A,x) + d4checksum * checksum_x; //GK:New-sum//

  r = b - Ax;
  checksum_r = checksum_b - checksum_Ax; //GK:New-sum//  

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

  v_tld = r;
  checksum_v_tld = checksum_r;

  Vector checksum_M1 = M1.nsChecksum(d4checksum); //GK:New-sum//    
  y = M1.solve(v_tld);
  //checksum_y = (checksum_v_tld - dot(checksum_M1,y)) / d4checksum; /////////MISSING///////////                                                                                           
  checksum_y = dot(c,y); //GK:New-sum// 

  rho(0) = norm(y);
  rho_(0) = norm(y);
  checksum_scaler += rho(0) - rho_(0); //GK:New-sum//  

  w_tld = r;
  checksum_w_tld = checksum_r; //GK:New-sum//  

  Vector checksum_M2 = M1.nsChecksum(d4checksum); //GK:New-sum// 
  z = M2.trans_solve(w_tld);
  //checksum_z = (checksum_wtld - dot(checksum_M2,z)) / d4checksum; /////////MISSING/////////// 
  checksum_z = dot(c,z); //GK:New-sum//  

  xi(0) = norm(z);
  xi_(0) = norm(z); //GK:New-sum//  
  checksum_scaler += xi(0) - xi_(0); //GK:New-sum//  

  gamma(0) = 1.0;
  gamma_(0) = 1.0; //GK:New-sum//  
  checksum_scaler += gamma(0) - gamma_(0); //GK:New-sum//  

  eta(0) = -1.0;
  eta_(0) = -1.0; //GK:New-sum//  
  checksum_scaler += eta(0) -  eta_(0) ; //GK:New-sum//  

  theta(0) = 0.0;
  theta_(0) = 0.0; //GK:New-sum//  
  checksum_scaler +=  theta(0) - theta_(0); //GK:New-sum//  

#ifdef HISTORY
  FILE *fp = fopen("qmr_history.txt", "wb+");
  if(fp == NULL){
     printf("File could not be open correctly\n");
     exit(1);
  }
#endif

  for (int i = 1; i <= max_iter; i++) {
    liter = i;
    insert_fault(r, i, 1);

    if (rho(0) == 0.0)
      return 2;                        // return on breakdown

    if (xi(0) == 0.0)
      return 7;                        // return on breakdown

    v = (1. / rho(0)) * v_tld;
    checksum_v = (1. / rho(0)) * checksum_v_tld; //GK:New-sum//  

    y = (1. / rho(0)) * y;
    checksum_y = (1. / rho(0)) * checksum_y; //GK:New-sum//  

    w = (1. / xi(0)) * w_tld;
    checksum_w = (1. / xi(0)) * checksum_w_tld; //GK:New-sum//  

    z = (1. / xi(0)) * z;
    checksum_z = (1. / xi(0)) * checksum_z; //GK:New-sum//  

    delta(0) = dot(z, y);
    delta_(0) = dot(z, y); //GK:New-sum//  
    checksum_scaler += delta(0) - delta_(0); //GK:New-sum//  


    if (delta(0) == 0.0)
      return 5;                        // return on breakdown

    y_tld = M2.solve(y);               // apply preconditioners
    //checksum_y_tld = (checksum_y - dot(checksum_M2,checksum_y_tld)) / d4checksum; /////////MISSING///////////                                                                              
    checksum_y_tld = dot(c,y_tld); //GK:New-sum//  //GK:New-sum//  

    z_tld = M1.trans_solve(z);
    //checksum_z_tld = (checksum_z - dot(checksum_M2,z_tld)) / d4checksum; /////////MISSING///////////
    checksum_z_tld = dot(c,z_tld); //GK:New-sum//  

    if (i > 1) {
      p = y_tld - (xi(0) * delta(0) / ep(0)) * p;
      checksum_p = checksum_y_tld - (xi(0) * delta(0) / ep(0)) * checksum_p; //GK:New-sum//  

      q = z_tld - (rho(0) * delta(0) / ep(0)) * q;
      checksum_q = checksum_z_tld - (rho(0) * delta(0) / ep(0)) * checksum_q; //GK:New-sum//  

    } else {
      p = y_tld;
      checksum_p = checksum_y_tld; //GK:New-sum//  
      q = z_tld;
      checksum_q = checksum_z_tld; //GK:New-sum//  
    }

    p_tld = A * p;
    checksum_p_tld = dot(checksum_A,p) + d4checksum * checksum_p; //GK:New-sum//  

    ep(0) = dot(q, p_tld);
    ep_(0) = dot(q, p_tld); //GK:New-sum//  
    checksum_scaler += ep(0) - ep_(0); //GK:New-sum//  

    if (ep(0) == 0.0)
      return 6;                        // return on breakdown

    beta(0) = ep(0) / delta(0);
    beta_(0) = ep(0) / delta(0); //GK:New-sum//  
    checksum_scaler += beta(0) - beta_(0); //GK:New-sum//  

    if (beta(0) == 0.0)
      return 3;                        // return on breakdown

    v_tld = p_tld - beta(0) * v;
    checksum_v_tld = checksum_p_tld - beta(0) * checksum_v; //GK:New-sum//  


    y = M1.solve(v_tld);
    //checksum_y = (checksum_v_tld - dot(checksum_M1,checksum_y)) / d4checksum;  ///////////////MISSING///////////////
    checksum_y = dot(c,y); //GK:New-sum//  

    rho_1(0) = rho(0);
    rho_1_(0) = rho_(0); //GK:New-sum//  
    checksum_scaler += rho_1(0)  - rho_1_(0); //GK:New-sum//  

    rho(0) = norm(y);
    rho_(0) = norm(y); //GK:New-sum//  
    checksum_scaler += rho(0) - rho_(0); //GK:New-sum//  

    w_tld = A.trans_mult(q) - beta(0) * w;
    checksum_w_tld = (dot(checksum_A,q) + d4checksum * checksum_q) -  beta(0) * checksum_w; //GK:New-sum//     

    z = M2.trans_solve(w_tld);
    //checksum_z = (checksum_w_tld - dot(checksum_M2,checksum_z)) / d4checksum;  ///////////////MISSING///////////////                                                                     
    checksum_z = dot(c,z); //GK:New-sum//  

    xi(0) = norm(z);
    xi_(0) = norm(z); //GK:New-sum//  
    checksum_scaler += xi(0) - xi_(0); //GK:New-sum//  

    gamma_1(0) = gamma(0);
    gamma_1_(0) = gamma(0); //GK:New-sum//  
    checksum_scaler +=  gamma_1(0) - gamma_1_(0); //GK:New-sum//  

    theta_1(0) = theta(0);
    theta_1_(0) = theta(0); //GK:New-sum//  
    checksum_scaler += theta_1(0) -  theta_1_(0); //GK:New-sum//  

    theta(0) = rho(0) / (gamma_1(0) * beta(0));
    theta_(0) = rho(0) / (gamma_1(0) * beta(0)); //GK:New-sum//  
    checksum_scaler += theta(0) - theta_(0); //GK:New-sum//  

    
    gamma(0) = 1.0 / sqrt(1.0 + theta(0) * theta(0));
    gamma_(0) = 1.0 / sqrt(1.0 + theta(0) * theta(0)); //GK:New-sum//  
    checksum_scaler += gamma(0)  -  gamma_(0); //GK:New-sum//  


    if (gamma(0) == 0.0)
      return 4;                        // return on breakdown

    eta_(0) = -eta(0) * rho_1(0) * gamma(0) * gamma(0) /
      (beta(0) * gamma_1(0) * gamma_1(0)); //GK:New-sum//  
    eta(0) = -eta(0) * rho_1(0) * gamma(0) * gamma(0) /
      (beta(0) * gamma_1(0) * gamma_1(0));
    checksum_scaler += eta(0) - eta_(0); //GK:New-sum//  
    

    if (i > 1) {
      d = eta(0) * p + (theta_1(0) * theta_1(0) * gamma(0) * gamma(0)) * d;
      checksum_d = eta(0) * checksum_p + (theta_1(0) * theta_1(0) * gamma(0) * gamma(0)) * checksum_d; //GK:New-sum//  

      s = eta(0) * p_tld + (theta_1(0) * theta_1(0) * gamma(0) * gamma(0)) * s;
      checksum_s = eta(0) * checksum_p_tld + (theta_1(0) * theta_1(0) * gamma(0) * gamma(0)) * checksum_s; //GK:New-sum//  

    } else {
      d = eta(0) * p;
      checksum_d = eta(0) * checksum_p; //GK:New-sum//  

      s = eta(0) * p_tld;
      checksum_s = eta(0) * checksum_p_tld; //GK:New-sum//  

    }

    x += d;                            // update approximation vector
    checksum_x += checksum_d; //GK:New-sum//  
    
    r -= s;                            // compute residual
    checksum_r -= checksum_s; //GK:New-sum//  
    
    resid = norm(r) / normb;
    resid_ = norm(r) / normb; //GK:New-sum//  
    checksum_scaler += resid - resid_; //GK:New-sum//  
    
    if (resid <= tol) {
      tol = resid;
      max_iter = i;
      return 0;
    }

    /****************/
    /*GK: Error detection, Check SDC*/
    /*****************/
    double ex1 = abs(checksum_r - dot(c,r))/matrix_size;
    double ex2 = abs(checksum_x - dot(c,x))/matrix_size;
    dCheck(thr,ex1, ex2, checksum_scaler, i);
    /*****************/

#ifdef HISTORY
    fprintf(fp, "%lf\n", resid);
#endif

  }
#ifdef HISTORY
  fclose(fp);
#endif
  tol = resid;
  return 1;                            // no convergence
}
