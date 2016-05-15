#ifndef MYMP_H_GUARD
#define MYMP_H_GUARD

#define IS_REAL_2D_FULL_DOUBLE_VECTOR(z) (!mxIsComplex(z) && mxGetNumberOfDimensions(z) == 2 && !mxIsSparse(z) && mxIsDouble(z) && (mxGetN(z) == 1))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define mod(a,b) (a-(b*(a/b)))
#define pow2(a) (a*a)

struct SPARSE_MAT {
int nnz;
int nr;
int *row;
int *col;
double *data;
};

struct DIAG_MAT {
int n;
double *data;
};

struct FULL_MAT {
int n;
int m;
double *data;
};

#endif