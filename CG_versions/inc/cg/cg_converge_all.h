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
  printf("max iter: %d\n", max_iter);
  printf("injection?: %d\n", iiternum);
  if(iiternum > 0)
    max_iter = iiternum + 25;
  printf("max iter: %d\n", max_iter);
  /****************/
  /*Collect data, Initialization*/
  /*****************/
  double vec_x[MAX_NUM_ITERATION];
  double vec_p[MAX_NUM_ITERATION];
  double vec_r[MAX_NUM_ITERATION];
/*****************/
  
  Real normb = norm(b);
  Vector Ax = A * x;
  Vector r = b - Ax;

  if (normb == 0.0) {
    normb = 1;
  }
  
  resid = norm(r) / normb;

  if (resid <= tol) {
    tol = resid;
    max_iter = 0;
    return 0;
  }

  Vector* inj_vectors[3];                                                                                                      
  inj_vectors[0] = &p;
  inj_vectors[1] = &r;                                                                                                          
  inj_vectors[2] = &x;                                                                                                         
  for (int i = 1; i <= max_iter; i++) {
    liter = i;
    //0: beginning of the iteration  
    insert_fault_vector(inj_vectors,0,i);
    z = M.solve(r);
    
    //1: after 1 statement, ....                                                                                                
    insert_fault_vector(inj_vectors,1,i); 

    rho(0) = dot(r, z);

    insert_fault_vector(inj_vectors,2,i); 
    
    if (i == 1){
      p = z;
      insert_fault_vector(inj_vectors,3,i);  
    }
    else {
      beta(0) = rho(0) / rho_1(0);
      p = z + beta(0) * p;
      insert_fault_vector(inj_vectors,3,i); 
    }
    
    q = A*p;
    insert_fault_vector(inj_vectors,4,i);  

    alpha(0) = rho(0) / dot(p, q);
    insert_fault_vector(inj_vectors,5,i); 
    
    x += alpha(0) * p;
    insert_fault_vector(inj_vectors,6,i);  

    r -= alpha(0) * q;
    insert_fault_vector(inj_vectors,7,i); 

    resid = norm(r) / normb;
    insert_fault_vector(inj_vectors,8,i); 

    rho_1(0) = rho(0);

    /****************/
    /*Collect data, Check SSD, AID and MAD*/
    /*****************/
    collect_expressions(norm(x), i-1, vec_x);
    collect_expressions(norm(p), i-1, vec_p);
    collect_expressions(norm(r), i-1, vec_r);
    dump_vector(x,i,"x_vector");
    dump_vector(p,i,"p_vector");
    dump_vector(r,i,"r_vector");
    /*****************/

    if (resid <= tol) {
      r = b - A*x;
      resid = norm(r) / normb;
      if (resid <= tol) {
	tol = resid;
	max_iter = i;
	/****************/
	/*Collect data  */
	/****************/
	dump_collection(vec_x,i,"x;");
	dump_collection(vec_p,i,"p;");
	dump_collection(vec_r,i,"r;");
	/*****************/
	return 0;
      }else{
        printf ("Activation enabled at %d\n", i);
	//Update max_iter
	max_iter = iters * ilimit;
      }
    }
  }

  tol = resid;
  /****************/
  /*Collect data  */
  /****************/
    dump_collection(vec_x,max_iter,"x;");
    dump_collection(vec_p,max_iter,"p;");
    dump_collection(vec_r,max_iter,"r;");
  /*****************/

  return 1;
}

