//*****************************************************************
// Iterative template routine -- Preconditioned Richardson
//
// IR solves the unsymmetric linear system Ax = b using 
// Iterative Refinement (preconditioned Richardson iteration).
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
template < class Matrix, class Vector, class Preconditioner, class Real >
int 
IR(const Matrix &A, Vector &x, const Vector &b,
   const Preconditioner &M, int &max_iter, Real &tol, int msize, double thr)
{
  Real resid;
  Vector z;

  Real normb = norm(b);
  Vector r = b - A*x;

  /****************/
  /* GK: Error detection, Init*/
  /*****************/
  dInit(max_iter, msize, thr);
  /****************/
  if (normb == 0.0) 
    normb = 1;
  
  if ((resid = norm(r) / normb) <= tol) {
    tol = resid;
    max_iter = 0;
    return 0;
  }
  
  for (int i = 1; i <= max_iter; i++) {
    liter = i;
    insert_fault(r, i, 1);

    z = M.solve(r);
    x += z;
    r = b - A * x;

    if ((resid = norm(r) / normb) <= tol) {
      tol = resid;
      max_iter = i;
      /****************/
      /*GK: Error detection, Finalize*/
      /*****************/
      dFinalize();
      /****************/
      return 0;
    }
    /****************/
    /*GK: Error detection, Check SDC*/
    /*****************/
    dCheck(resid, i);
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



