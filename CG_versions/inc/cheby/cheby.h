//*****************************************************************
// Iterative template routine -- CHEBY
//
// CHEBY solves the symmetric positive definite linear
// system Ax = b using the Preconditioned Chebyshev Method
//
// CHEBY follows the algorithm described on p. 30 of the 
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

#include "insert.h"
extern long liter;
template < class Matrix, class Vector, class Preconditioner, class Real,
           class Type >
int 
CHEBY(const Matrix &A, Vector &x, const Vector &b,
      const Preconditioner &M, int &max_iter, Real &tol,
      Type eigmin, Type eigmax)
{
  Real resid;
  Type alpha, beta, c, d;
  Vector p, q, z;

  Real normb = norm(b);
  Vector r = b - A * x;

  if (normb == 0.0)
    normb = 1;
  
  if ((resid = norm(r) / normb) <= tol) {
    tol = resid;
    max_iter = 0;
    return 0;
  }

  c = (eigmax - eigmin) / 2.0;
  d = (eigmax + eigmin) / 2.0;

#ifdef HISTORY 
  FILE *fp = fopen("cheby_history.txt", "wb+");
  if(fp == NULL){
     fprintf(stderr, "Could not open file correctly\n");
     exit(1);
  }
#endif
  for (int i = 1; i <= max_iter; i++) {
    liter = i;
    insert_fault(r, i, 1);
//    insert_fault(x, i, 1);

    z = M.solve(r);                 // apply preconditioner

//    insert_fault(z, i, 1);

    if (i == 1) {
      p = z;
      alpha = 2.0 / d;
    } else {
      beta = c * alpha / 2.0;       // calculate new beta
      beta = beta * beta;
      alpha = 1.0 / (d - beta);     // calculate new alpha
      p = z + beta * p;             // update search direction
    }

//    insert_fault(p, i, 1);

    q = A * p;

//    insert_fault(q, i, 1);

    x += alpha * p;                 // update approximation vector
    r -= alpha * q;                 // compute residual

    if ((resid = norm(r) / normb) <= tol) {
      tol = resid;
      max_iter = i;
      return 0;                     // convergence
    }

#ifdef HISTORY 
    fprintf(fp, "%lf\n", resid);
#endif

  }
#ifdef HISTORY
   fclose(fp);
#endif
  tol = resid;
  return 1;                         // no convergence
}
