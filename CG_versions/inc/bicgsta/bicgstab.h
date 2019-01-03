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
         const Preconditioner &M, int &max_iter, Real &tol)
{
  Real resid;
  Vector rho_1(1), rho_2(1), alpha(1), beta(1), omega(1);
  Vector p, phat, s, shat, t, v;

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
  FILE *fp = fopen ("bicgsta_history.txt", "wb+");
  if(fp == NULL){
     fprintf(stderr, "Could not open file correctly\n");
     exit(1);
  }
#endif

  for (int i = 1; i <= max_iter; i++) {
    liter = i;
    //insert_fault(r, i, 1);
    insert_fault(r, i);

    rho_1(0) = dot(rtilde, r);
    if (rho_1(0) == 0) {
      tol = norm(r) / normb;
      return 2;
    }
    if (i == 1)
      p = r;
    else {
      beta(0) = (rho_1(0)/rho_2(0)) * (alpha(0)/omega(0));
      p = r + beta(0) * (p - omega(0) * v);
    }

    phat = M.solve(p);
    v = A * phat;
    alpha(0) = rho_1(0) / dot(rtilde, v);
    s = r - alpha(0) * v;

    if ((resid = norm(s)/normb) < tol) {
      x += alpha(0) * phat;
      tol = resid;
      // Joseph: a zero path that was not accounted
      max_iter = i;
      return 0;
    }

#ifdef HISTORY
    fprintf(fp, "%lf,", resid);
#endif

    shat = M.solve(s);

    t = A * shat;

    omega = dot(t,s) / dot(t,t);
    x += alpha(0) * phat + omega(0) * shat;
    r = s - omega(0) * t;
    rho_2(0) = rho_1(0);

    resid = norm(r) / normb;
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

    if (omega(0) == 0) {
      tol = norm(r) / normb;
      return 3;
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
