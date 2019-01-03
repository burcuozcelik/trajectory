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

  if (normb == 0.0) 
    normb = 1;
  
  if ((resid = norm(r) / normb) <= tol) {
    tol = resid;
    max_iter = 0;
    return 0;
  }

/*  FILE *fp;
  fp = fopen ("sla_conv.csv", "wb+");
  if(fp == NULL){
     printf("Error in opening the convergance file: %s %d\n", __FILE__, __LINE__);
     exit(19);
  }*/

#ifdef HISTORY
   FILE *fp = fopen("iccg_history.txt", "wb+");
   if(fp == NULL){
     printf("File could not be open correctly\n");
     exit(1);
   }
#endif

  for (int i = 1; i <= max_iter; i++) {
    liter = i;
    //insert_fault(r, i, 1);
    insert_fault(r, i);
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
    resid = norm(r) / normb;
    rho_1(0) = rho(0);

    
    if (resid <= tol) {
      r = b - A*x;
      resid = norm(r) / normb;
      if (resid <= tol) {
        tol = resid;
        max_iter = i;
        return 0;
      }

      else{
	printf ("Activated restart: %d\n", i);
      }
    }

#ifdef HISTORY
    fprintf(fp, "%lf\n", resid);
#endif

  }
  //fclose(fp); 
  tol = resid;
#ifdef HISTORY
  fclose(fp);
#endif
  return 1;
}

