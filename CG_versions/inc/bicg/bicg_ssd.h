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

  /****************/
  /* GK: Error detection, Init*/
  /*****************/
  dInit();
  /****************/

#if defined(SSD_PROTECT_R)
  SDC_Protect("r", &r[0], SDC_DOUBLE, 0, 0, 0, 0, r.dim());
#else
  SDC_Protect("rid", &resid, SDC_DOUBLE, 0, 0, 0, 0, 1);
#endif


  if (normb == 0.0)
    normb = 1;
  
  if ((resid = norm(r) / normb) <= tol) {
    tol = resid;
    max_iter = 0;
    /****************/
    /*GK: Error detection, Finalize*/
    /*****************/
    dFinalize();
    /****************/

    /****************/
    /* GK: Error detection, Print results*/
    /*****************/
    dPrintDetectResults();
    /****************/
    return 0;
  }

  for (int i = 1; i <= max_iter; i++) {
    liter = i;
#if defined INSERT_MULTIBF_FAULT
    insert_fault(r, i);
#else
    insert_fault(r, i, 1);
#endif
    z = M.solve(r);
    ztilde = M.trans_solve(rtilde);

    rho_1(0) = dot(z, rtilde);
    if (rho_1(0) == 0) { 
      tol = norm(r) / normb;
      max_iter = i;
      /****************/
      /*GK: Error detection, Finalize*/
      /*****************/
      dFinalize();
      /****************/

      /****************/
      /* GK: Error detection, Print results*/
      /*****************/
      dPrintDetectResults();
      /****************/
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
    if ((resid = norm(r) / normb) < tol) {
#if defined ACTIVATION
       r = b - A * x;
       if ((resid = norm(r) / normb) <= tol) {
#endif
          tol = resid;
          max_iter = i;
      /****************/
      /*GK: Error detection, Finalize*/
      /*****************/
          dFinalize();
      /****************/

      /****************/
      /* GK: Error detection, Print results*/
      /*****************/
         dPrintDetectResults();
      /****************/
         return 0;
#if defined ACTIVATION
      }
       else{
         printf ("Activated restart: %d\n", i);
       }
#endif

    }
    /****************/
    /*GK: Error detection, Check SDC*/
    /*****************/
    dCheck(i);
    /*****************/
  }

  tol = resid;

  /****************/
  /*GK: Error detection, Finalize*/
  /*****************/
  dFinalize();
  /****************/

  /****************/
  /* GK: Error detection, Print results*/
  /*****************/
  dPrintDetectResults();
  /****************/
  return 1;
}
  
