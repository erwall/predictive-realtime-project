/*
 * inv.c
 *
 * Code generation for function 'inv'
 *
 * C source code generated on: Sun May 15 12:12:16 2016
 *
 */

/* Include files */
#include "rt_nonfinite.h"
#include "Kalman.h"
#include "inv.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

/* Function Definitions */
void inv(const real_T x[4], real_T y[4])
{
  real_T r;
  real_T t;
  if (fabs(x[1]) > fabs(x[0])) {
    r = x[0] / x[1];
    t = 1.0 / (r * x[3] - x[2]);
    y[0] = x[3] / x[1] * t;
    y[1] = -t;
    y[2] = -x[2] / x[1] * t;
    y[3] = r * t;
  } else {
    r = x[1] / x[0];
    t = 1.0 / (x[3] - r * x[2]);
    y[0] = x[3] / x[0] * t;
    y[1] = -r * t;
    y[2] = -x[2] / x[0] * t;
    y[3] = t;
  }
}

/* End of code generation (inv.c) */
