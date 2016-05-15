/*
 * mrdivide.c
 *
 * Code generation for function 'mrdivide'
 *
 * C source code generated on: Sun May 15 12:12:16 2016
 *
 */

/* Include files */
#include "rt_nonfinite.h"
#include "Kalman.h"
#include "mrdivide.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

/* Function Definitions */
void mrdivide(const real_T A[32], const real_T B[4], real_T y[32])
{
  real_T b_A[4];
  int32_T r1;
  int32_T r2;
  real_T b_B[32];
  real_T a21;
  real_T a22;
  real_T Y[32];
  int32_T k;
  for (r1 = 0; r1 < 2; r1++) {
    for (r2 = 0; r2 < 2; r2++) {
      b_A[r2 + (r1 << 1)] = B[r1 + (r2 << 1)];
    }
  }

  for (r1 = 0; r1 < 16; r1++) {
    for (r2 = 0; r2 < 2; r2++) {
      b_B[r2 + (r1 << 1)] = A[r1 + (r2 << 4)];
    }
  }

  if (fabs(b_A[1]) > fabs(b_A[0])) {
    r1 = 1;
    r2 = 0;
  } else {
    r1 = 0;
    r2 = 1;
  }

  a21 = b_A[r2] / b_A[r1];
  a22 = b_A[2 + r2] - a21 * b_A[2 + r1];
  for (k = 0; k < 16; k++) {
    Y[1 + (k << 1)] = (b_B[r2 + (k << 1)] - b_B[r1 + (k << 1)] * a21) / a22;
    Y[k << 1] = (b_B[r1 + (k << 1)] - Y[1 + (k << 1)] * b_A[2 + r1]) / b_A[r1];
  }

  for (r1 = 0; r1 < 2; r1++) {
    for (r2 = 0; r2 < 16; r2++) {
      y[r2 + (r1 << 4)] = Y[r1 + (r2 << 1)];
    }
  }
}

/* End of code generation (mrdivide.c) */
