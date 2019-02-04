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
    const Preconditioner &M, int &max_iter, Real &tol)
{
  Real resid;
  Vector rho_1(1), rho_2(1), alpha(1), beta(1);
  Vector p, phat, q, qhat, vhat, u, uhat;
  printf("max iter: %d\n", max_iter);
  printf("injection?: %d\n", iiternum);
  if(iiternum > 0)
    max_iter = iiternum + 25;
  printf("max iter: %d\n", max_iter);
  /****************/
  /*Collect data, Initialization*/
  /*****************/
  double vec_resids[MAX_NUM_ITERATION]; //for SSD                             
  double newsum_exp1[MAX_NUM_ITERATION]; // for NEWSUM         
  double newsum_exp2[MAX_NUM_ITERATION]; // for NEWSUM                  
  double newsum_scaler[MAX_NUM_ITERATION]; // for NEWSUM   
  /*****************/

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
  
  Vector checksum_M = M.nsChecksum(d); //GK:New-sum//   
  Vector* inj_vectors[12];  
  inj_vectors[0] = &p;
  inj_vectors[1] = &phat;
  inj_vectors[2] = &q;
  inj_vectors[3] = &qhat;
  inj_vectors[4] = &r;
  inj_vectors[5] = &rtilde;
  inj_vectors[6] = &u;
  inj_vectors[7] = &uhat;
  inj_vectors[8] = &vhat;
  inj_vectors[9] = &x;
  inj_vectors[10] = &alpha;
  inj_vectors[11] = &rho_1;
 
  for (int i = 1; i <= max_iter; i++) {
    liter = i;
    insert_fault_vector(inj_vectors,0,i);
    
    rho_1(0) = dot(rtilde, r);
    rho_1_(0) = dot(rtilde, r);  //GK:New-sum//    
    checksum_scaler +=  rho_1(0) -  rho_1_(0);  //GK:New-sum//    
    insert_fault_vector(inj_vectors,1,i);
    if (rho_1(0) == 0) {
      tol = norm(r) / normb;
      tol_ = norm(r) / normb;  //GK:New-sum//    
      checksum_scaler += tol - tol_;  //GK:New-sum//    
      /****************/
      /*Collect data, Check AID, MAD, orthogonality, NEWSUM*/
      /*****************/
      dump_residuals(vec_resids,i);
      dump_newsum_ex1(newsum_exp1,i);
      dump_newsum_ex2(newsum_exp2,i);
      dump_newsum_scaler(newsum_scaler,i);
      /*****************/
      return 2;
    }

    if (i == 1) {
      u = r;
      checksum_u = checksum_r;  //GK:New-sum//    
      insert_fault_vector(inj_vectors,2,i);
      p = u;
      checksum_p = checksum_u;  //GK:New-sum//    
      insert_fault_vector(inj_vectors,3,i);
    } else {
      beta(0) = rho_1(0) / rho_2(0);
      beta_(0) = rho_1(0) / rho_2(0);  //GK:New-sum//    
      checksum_scaler +=  beta(0) -  beta(0);  //GK:New-sum//    

      u = r + beta(0) * q;
      checksum_u = checksum_r + beta(0) * checksum_q;  //GK:New-sum//
      insert_fault_vector(inj_vectors,2,i);
      p = u + beta(0) * (q + beta(0) * p);
      checksum_p = checksum_u + beta(0) * (checksum_q + beta(0) * checksum_p);  //GK:New-sum//
      insert_fault_vector(inj_vectors,3,i);
    }

    phat = M.solve(p);
    //checksum_phat = (checksum_p - dot(checksum_M,phat)) / d;    ///////////////MISSING///////////////          
    checksum_phat = dot(c,phat); //GK:New-sum//     
    insert_fault_vector(inj_vectors,4,i);
    vhat = A*phat;
    checksum_q = dot(checksum_A,phat) + d * checksum_phat; //GK:New-sum//    
    insert_fault_vector(inj_vectors,5,i);
    alpha(0) = rho_1(0) / dot(rtilde, vhat);
    alpha_(0) = rho_1(0) / dot(rtilde, vhat);  //GK:New-sum//    
    checksum_scaler += alpha(0) - alpha_(0);  //GK:New-sum//    
    insert_fault_vector(inj_vectors,6,i);
    q = u - alpha(0) * vhat;
    checksum_q = checksum_u - alpha(0) * checksum_vhat;  //GK:New-sum//    
    insert_fault_vector(inj_vectors,7,i);
    uhat = M.solve(u + q);
    //checksum_uhat = ((checksum_u + checksum_q) - dot(checksum_M,uhat)) / d;    ///////////////CHECK!!! MISSING///////////////            
    checksum_uhat = dot(c,uhat); //GK:New-sum//
    insert_fault_vector(inj_vectors,8,i);
    x += alpha(0) * uhat;
    checksum_x += alpha(0) * checksum_uhat;  //GK:New-sum//    
    insert_fault_vector(inj_vectors,9,i);
    qhat = A * uhat;
    checksum_qhat = dot(checksum_A,uhat) + d * checksum_uhat;  //GK:New-sum//    
    insert_fault_vector(inj_vectors,10,i);
    r -= alpha(0) * qhat;
    insert_fault_vector(inj_vectors,11,i);
    checksum_r -= alpha(0) * checksum_qhat;  //GK:New-sum//    

    rho_2(0) = rho_1(0);
    rho_2_(0) = rho_1(0);  //GK:New-sum//    
    checksum_scaler += rho_2_(0) - rho_2(0);  //GK:New-sum//    


    resid = norm(r) / normb;
    resid_ = norm(r) / normb;  //GK:New-sum//    
    checksum_scaler += resid - resid_;  //GK:New-sum//    
    insert_fault_vector(inj_vectors,12,i);
    /****************/
    /*Collect data, Check SSD, AID and MAD*/
    /*****************/
    dump_residual_vector(r, i); //for SSD                                                                                                                                 
    collect_residual(resid, i-1, vec_resids); // for AID and MAD
    collect_expressions(abs(checksum_r - dot(c,r))/matrix_size, i-1, newsum_exp1);
    collect_expressions(abs(checksum_x - dot(c,x))/matrix_size, i-1, newsum_exp2);
    collect_expressions(checksum_scaler, i-1, newsum_scaler);
    dump_vector(x,i,"x_vector");
    dump_vector(p,i,"p_vector");
    dump_vector(r,i,"r_vector");
    /*****************/
    if(resid <= tol){
      r = b - A*x;
      resid = norm(r) / normb;
      if (resid <= tol) {
	tol = resid;
	max_iter = i;
	/****************/
	/*Collect data, Check AID, MAD, orthogonality, NEWSUM*/
	/*****************/
	dump_residuals(vec_resids,i);
	dump_newsum_ex1(newsum_exp1,i);
	dump_newsum_ex2(newsum_exp2,i);
	dump_newsum_scaler(newsum_scaler,i);
	/*****************/
	return 0;
      }
      else{
	printf ("Activation enabled at %d\n", i);
        //Update max_iter
        max_iter = iters * ilimit;
      }
    }
  }
  
  tol = resid;
  /****************/
  /*Collect data, Check AID, MAD, orthogonality, NEWSUM*/
  /*****************/
  dump_residuals(vec_resids,max_iter);
  dump_newsum_ex1(newsum_exp1,max_iter);
  dump_newsum_ex2(newsum_exp2,max_iter);
  dump_newsum_scaler(newsum_scaler,max_iter);
  /*****************/
  return 1;
}

