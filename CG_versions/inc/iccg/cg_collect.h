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

  /****************/
  /*Collect data, Initialization*/
  /*****************/
  Vector vec_resids(MAX_NUM_ITERATION, 0); //for SSD                                                                                                                     
  Vector orth_exp1(MAX_NUM_ITERATION, 0); // for ORTH                                                                                                                     
  Vector orth_exp2(MAX_NUM_ITERATION, 0); // for ORTH                                                                                                                     
  Vector newsum_exp1(MAX_NUM_ITERATION, 0); // for NEWSUM                                                                                                                 
  Vector newsum_exp2(MAX_NUM_ITERATION, 0); // for NEWSUM                                                                                                                 
  Vector newsum_scaler(MAX_NUM_ITERATION, 0); // for NEWSUM                                                                                                              
  /*****************/

  //GK:New-sum//
  Vector alpha_(1), beta_(1), rho_(1), rho_1_(1);
  Real resid_;
  double checksum_scaler;
  Vector checksum_A;
  double checksum_x, checksum_b, checksum_Ax, checksum_r, checksum_z, checksum_p, checksum_q;
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

  if (normb == 0.0) {
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
    insert_fault(r, i);

    z = M.solve(r);
    
    //GK:New-sum// 
    //checksum_z = (checksum_r - dot(checksum_M,z)) / d; /////////MISSING/////////// 
    checksum_z = dot(c,z); 
    //GK:New-sum// 
    
    rho(0) = dot(r, z);
    rho_(0) = dot(r, z); //GK:New-sum//  
    checksum_scaler +=  rho(0) -  rho_(0); //GK:New-sum//  
    
    if (i == 1){
      p = z;
      checksum_p = checksum_z;    //GK:New-sum//     
    }
    else {
      beta(0) = rho(0) / rho_1(0);
      beta_(0) = rho(0) / rho_1(0);    //GK:New-sum// 
      checksum_scaler +=  beta(0) -  beta_(0);    //GK:New-sum// 

      p = z + beta(0) * p;
      checksum_p = checksum_z + beta(0) * checksum_p; //GK:New-sum//  
    }
    

    /****************/
    /*Collect data, Check SDC, orthogonality*/
    /*****************/
    if (i > 1){
      collect_expressions(dot(p,q) / (norm(p) * norm(q)), i-1, orth_exp1);
      collect_expressions(norm(r + A*x -b) / (norm(b) * normM(A)), i-1, orth_exp2);
    }
    else{
      collect_expressions(-1, i-1, orth_exp1);
      collect_expressions(-1, i-1, orth_exp2);
    }
    /*****************/


    q = A*p;
    checksum_q = dot(checksum_A,p) + d * checksum_p; //GK:New-sum// 

    alpha(0) = rho(0) / dot(p, q);
    alpha_(0) = rho(0) / dot(p, q);  //GK:New-sum// 
    checksum_scaler +=  alpha(0) -  alpha_(0); //GK:New-sum//                 
    
    x += alpha(0) * p;
    checksum_x += alpha(0) * checksum_p;  //GK:New-sum// 

    r -= alpha(0) * q;
    checksum_r -= alpha(0) * checksum_q; //GK:New-sum//

    resid = norm(r) / normb;
    resid_ =  norm(r) / normb;  //GK:New-sum// 
    checksum_scaler += resid - resid_;  //GK:New-sum// 

    rho_1(0) = rho(0);
    rho_1_(0) = rho(0);  //GK:New-sum//                                                                                                               
    checksum_scaler += rho_1(0) - rho_1_(0);  //GK:New-sum//

    /****************/
    /*Collect data, Check SSD, AID and MAD*/
    /*****************/
    dump_residual_vector(r, i); //for SSD                                                                                                                                
    collect_residual(resid, i-1, vec_resids); // for AID and MAD                                                                                                           
    collect_expressions(abs(checksum_r - dot(c,r))/matrix_size, i-1, newsum_exp1);
    collect_expressions(abs(checksum_x - dot(c,x))/matrix_size, i-1, newsum_exp2);
    collect_expressions(checksum_scaler, i-1, newsum_scaler);
    /*****************/

    if (resid <= tol) {
      tol = resid;
      max_iter = i;
      /****************/
      /*Collect data, Check AID, MAD, orthogonality, NEWSUM*/
      /*****************/
      dump_residuals(vec_resids,i);
      dump_orth_ex1(orth_exp1,i);
      dump_orth_ex2(orth_exp2,i);
      dump_newsum_ex1(newsum_exp1,i);
      dump_newsum_ex2(newsum_exp2,i);
      dump_newsum_scaler(newsum_scaler,i);
      /*****************/
      return 0;     
    }

  }
  tol = resid;
  /****************/
  /*Collect data, Check AID, MAD, orthogonality, NEWSUM*/
  /*****************/
  dump_residuals(vec_resids,max_iter);
  dump_orth_ex1(orth_exp1,max_iter);
  dump_orth_ex2(orth_exp2,max_iter);
  dump_newsum_ex1(newsum_exp1,max_iter);
  dump_newsum_ex2(newsum_exp2,max_iter);
  dump_newsum_scaler(newsum_scaler,max_iter);
  /*****************/
  return 1;
}

