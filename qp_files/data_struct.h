#ifndef DATA_STRUCT_H_GUARD
#define DATA_STRUCT_H_GUARD

struct DATA {
struct FULL_MAT *M;
struct FULL_MAT *Q1;
struct FULL_MAT *Q2;
struct DIAG_MAT *C;
struct DIAG_MAT *CT;
struct DIAG_MAT *E;
struct DIAG_MAT *Einv;
double *Lb;
double *Ub;
struct SPARSE_MAT *r2;
struct SPARSE_MAT *R;
};

#endif

