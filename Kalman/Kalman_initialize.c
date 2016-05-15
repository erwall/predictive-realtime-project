/*
 * Kalman_initialize.c
 *
 * Code generation for function 'Kalman_initialize'
 *
 * C source code generated on: Sun May 15 12:12:16 2016
 *
 */

/* Include files */
#include "rt_nonfinite.h"
#include "Kalman.h"
#include "Kalman_initialize.h"
#include "Kalman_data.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

/* Function Definitions */
void Kalman_initialize(void)
{
  rt_InitInfAndNaN(8U);
  xhat_not_empty = FALSE;
}

/* End of code generation (Kalman_initialize.c) */
