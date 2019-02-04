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
     const Preconditioner &M, int &max_iter, Real &tol)
{
  Real resid;
  Vector rho_1(1), rho_2(1), alpha(1), beta(1);
  Vector z, ztilde, p, ptilde, q, qtilde;
  printf("max iter: %d\n", max_iter);
  printf("injection?: %d\n", iiternum);
  if(iiternum > 0)
    max_iter = iiternum + 25;
  printf("max iter: %d\n", max_iter);
  /****************/
  /*Collect data, Initialization*/
  /*****************/
  double vec_resids[MAX_NUM_ITERATION]; //for SSD                                                                                                    
  double orth_exp1[MAX_NUM_ITERATION]; // for ORTH                                                                                                
  double orth_exp2[MAX_NUM_ITERATION]; // for ORTH                                                                                             
  double newsum_exp1[MAX_NUM_ITERATION]; // for NEWSUM                                                                                             
  double newsum_exp2[MAX_NUM_ITERATION]; // for NEWSUM                                                                                               
  double newsum_scaler[MAX_NUM_ITERATION]; // for NEWSUM     
  /*****************/
  
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
  Vector* inj_vectors[11];                                                                                                      
  inj_vectors[0] = &p;
  inj_vectors[1] = &ptilde;                                                                                                          
  inj_vectors[2] = &q;                                                                                                          
  inj_vectors[3] = &qtilde;                                                                                                         
  inj_vectors[4] = &r;
  inj_vectors[5] = &rtilde;
  inj_vectors[6] = &x;                                                                                                          
  inj_vectors[7] = &z;                                                                                                          
  inj_vectors[8] = &ztilde;    
  inj_vectors[9] = &alpha;
  inj_vectors[10] = &rho_1;
 
  for (int i = 1; i <= max_iter; i++) {
    liter = i;
    insert_fault_vector(inj_vectors,0,i);

    z = M.solve(r);
    //checksum_z = (checksum_r - dot(checksum_M,z)) / d;    ///////////////MISSING///////////////
    checksum_z = dot(c,z); //GK:New-sum//
    insert_fault_vector(inj_vectors,1,i); 

    ztilde = M.trans_solve(rtilde);
    //checksum_ztilde = (checksum_rtilde - dot(checksum_M,ztilde)) / d; /////////MISSING///////////
    checksum_ztilde = dot(c,ztilde); //GK:New-sum// 
    insert_fault_vector(inj_vectors,2,i);

    rho_1(0) = dot(z, rtilde);
    rho_1_(0) = dot(z, rtilde); //GK:New-sum//            
    checksum_scaler +=  rho_1(0) -  rho_1_(0); //GK:New-sum//  
    insert_fault_vector(inj_vectors,3,i);  

    if (rho_1(0) == 0) { 
      tol = norm(r) / normb;
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
      return 2;
    }

    if (i == 1) {
      p = z;
      checksum_p = checksum_z;    //GK:New-sum//    
      insert_fault_vector(inj_vectors,4,i);  

      ptilde = ztilde;
      checksum_ztilde = checksum_ztilde;    //GK:New-sum// 
      insert_fault_vector(inj_vectors,5,i);  
    } else {
      beta(0) = rho_1(0) / rho_2(0);
      beta_(0) = rho_1(0) / rho_2(0);  //GK:New-sum// 
      checksum_scaler +=  beta(0) -  beta_(0); //GK:New-sum//

      p = z + beta(0) * p;
      checksum_p = checksum_z + beta(0) * checksum_p; //GK:New-sum//          
      insert_fault_vector(inj_vectors,4,i);  
      ptilde = ztilde + beta(0) * ptilde;
      checksum_ptilde = checksum_ztilde + beta(0) * checksum_ptilde; //GK:New-sum//         
      insert_fault_vector(inj_vectors,5,i);  
    }


    /****************/
    /*Collect data, Check SDC, orthogonality*/
    /*****************/
    if (i > 1){
      collect_expressions(dot(ptilde,q) / (norm(ptilde) * norm(qtilde)), i-1, orth_exp1);
      collect_expressions(norm(r + A*x -b) / (norm(b) * normM(A)), i-1, orth_exp2);
    }
    else{
      collect_expressions(-1, i-1, orth_exp1);
      collect_expressions(-1, i-1, orth_exp2);
    }
    /*****************/


    q = A * p;
    checksum_q = dot(checksum_A,p) + d * checksum_p; //GK:New-sum//      
    insert_fault_vector(inj_vectors,6,i); 

    qtilde = A.trans_mult(ptilde);
    checksum_qtilde = dot(checksum_A,ptilde) + d * checksum_ptilde; //GK:New-sum// 
    insert_fault_vector(inj_vectors,7,i); 

    alpha(0) = rho_1(0) / dot(ptilde, q);
    alpha_(0) = rho_1(0) / dot(ptilde, q);  //GK:New-sum// 
    checksum_scaler +=  alpha(0) -  alpha_(0); //GK:New-sum//   
    insert_fault_vector(inj_vectors,8,i); 

    x += alpha(0) * p;
    checksum_x += alpha(0) * checksum_p;  //GK:New-sum//  
    insert_fault_vector(inj_vectors,9,i);
 
    r -= alpha(0) * q;
    checksum_r -= alpha(0) * checksum_q; //GK:New-sum//    
    insert_fault_vector(inj_vectors,10,i); 

    rtilde -= alpha(0) * qtilde;
    checksum_rtilde -= alpha(0) * checksum_qtilde; //GK:New-sum// 
    insert_fault_vector(inj_vectors,11,i); 

    rho_2(0) = rho_1(0);
    rho_2_(0) = rho_1(0);  //GK:New-sum//
    checksum_scaler += rho_2(0) - rho_2_(0);


    resid = norm(r) / normb;
    resid_ =  norm(r) / normb;  //GK:New-sum//
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

    if (resid <= tol) {
      r = b - A*x;
      resid = norm(r) / normb;
      if(resid <= tol) {
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
      }else{
	printf("Activation enabled at %d\n", i);
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
  dump_orth_ex1(orth_exp1,max_iter);
  dump_orth_ex2(orth_exp2,max_iter);
  dump_newsum_ex1(newsum_exp1,max_iter);
  dump_newsum_ex2(newsum_exp2,max_iter);
  dump_newsum_scaler(newsum_scaler,max_iter);
  /*****************/
  return 1;
}
  
