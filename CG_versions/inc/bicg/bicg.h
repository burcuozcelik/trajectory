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

  Real normb = norm(b);
  Vector r = b - A * x;
  Vector rtilde = r;

  if (normb == 0.0)
    normb = 1;
  
  if ((resid = norm(r) / normb) <= tol) {
    tol = resid;
    max_iter = 0;
    return 0;
  }

#ifdef HISTORY
  FILE *fp = fopen("bicg_history.txt", "wb+");
  if(fp == NULL){
     fprintf(stderr, "Could not open the file correctly\n");
     exit(1);
  }

#endif

  for (int i = 1; i <= max_iter; i++) {
    liter = i;
    //insert_fault(r, i, 1);
    insert_fault(r, i);
    z = M.solve(r);

    ztilde = M.trans_solve(rtilde);

    rho_1(0) = dot(z, rtilde);
    if (rho_1(0) == 0) { 
      tol = norm(r) / normb;
      max_iter = i;
      return 2;
    }
    if (i == 1) {
      p = z;
      ptilde = ztilde;
    } else {
      beta(0) = rho_1(0) / rho_2(0);
      p = z + beta(0) * p;
      ptilde = ztilde + beta(0) * ptilde;
    }

    q = A * p;
    qtilde = A.trans_mult(ptilde);

    alpha(0) = rho_1(0) / dot(ptilde, q);
    x += alpha(0) * p;
    r -= alpha(0) * q;
    rtilde -= alpha(0) * qtilde;

    rho_2(0) = rho_1(0);

    resid = norm(r) / normb;
    if (resid <= tol) {
      r = b - A*x;
      resid = norm(r) / normb;
      if (resid  <= tol) {
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
#ifdef HISTORY
  fclose(fp);
#endif
  tol = resid;
  return 1;
}
  
