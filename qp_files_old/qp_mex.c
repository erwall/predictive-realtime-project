#include "mex.h"
#include "QPgen.h"
#include "data_struct.h"


 void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

 /* define variables */
 double *x;
 int *iter;double *gt;
double *bt;

/* check inputs */

 if (!(nrhs == 2)) {
 mexErrMsgTxt("Wrong nbr of inputs");
} 

gt = mxGetPr(prhs[0]);
if (!IS_REAL_2D_FULL_DOUBLE_VECTOR(prhs[0])  || (mxGetM(prhs[0]) != 540)){
 mexErrMsgTxt("Input 1 should be real full vector of size (540,1)");
}

bt = mxGetPr(prhs[1]);

if (!IS_REAL_2D_FULL_DOUBLE_VECTOR(prhs[1])  || (mxGetM(prhs[1]) != 16)){
 mexErrMsgTxt("Input 2 should be real full vector of size (16,1)");
}

/* set output */
 plhs[0] = mxCreateDoubleMatrix(540,1,mxREAL);

x = mxGetPr(plhs[0]);

 /* run main loop */
plhs[1] = mxCreateNumericMatrix(1,1,mxINT32_CLASS,mxREAL);

iter = (int *) mxGetData(plhs[1]);

struct DATA *d = malloc(sizeof(struct DATA));

init_data(d);

qp(d,x,iter,gt,bt);

free_data(d);

}