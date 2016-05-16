#include <math.h>
#include "QPgen.h"

#include "data_struct.h"

/* solves Ly=Px where P is permutation matrix defined by vector p */
 static void perm_fwdsolve(struct SPARSE_MAT *L, const int *p, double *x, double *y) {
 int iter_stop;
 int row_idx = 0;
 int jj;
 for (jj = 0; jj <= L->nr-1; jj++) {
 y[jj] = (L->data)[(L->row)[jj+1]-1]*x[p[jj]];;
 for (row_idx = (L->row)[jj] ; row_idx < (L->row)[jj+1]-1 ; row_idx++) {
 y[jj] = y[jj]-(L->data)[row_idx]*y[(L->col)[row_idx]];
 }
 }
 }

/* solves LPy=x */
 static void backsolve_perm(struct SPARSE_MAT *L, const int *p, double *x, double *y) {
 int iter_stop;
 int row_idx = 0;
 int jj;
 for (jj = L->nr-1 ; jj >= 0 ; jj--) {
 y[p[jj]] = (L->data)[(L->row)[jj]]*x[jj];
 for (row_idx = (L->row)[jj]+1 ; row_idx < (L->row)[jj+1] ; row_idx++) {
 y[p[jj]] = y[p[jj]]-(L->data)[row_idx]*y[p[(L->col)[row_idx]]];
 }
 }
 }

static void mat_vec_mult_diag(struct DIAG_MAT *D, double *x, double *y) {
 int jj;
 #pragma omp parallel for
 for (jj = 0; jj <= D->n-1; jj++) {
 y[jj] = (D->data)[jj]*x[jj];
 }
 }

static void mat_vec_mult_sparse(struct SPARSE_MAT *L, double *x, double *y) {
 int row_idx = 0;
 int jj;
 int iter_stop;
 for (jj = 0 ; jj <= L->nr-1 ; jj++) {
 y[jj] = 0;
 for (row_idx = (L->row)[jj] ; row_idx < (L->row)[jj+1] ; row_idx++) {
 y[jj] = y[jj]+(L->data)[row_idx]*x[(L->col)[row_idx]];
 
 }
 }  
}

static void mat_vec_mult_full(struct FULL_MAT *M, double *x, double *y) {
 int jj;
 int kk;
 #pragma omp parallel for
 for (jj = 0; jj <= (M->n)-1; jj++) {
 y[jj] = 0;
 for (kk = 0 ; kk <= (M->m)-1; kk++) {
 y[jj] = y[jj]+(M->data[kk+jj*(M->m)])*x[kk];
 }
 } 
 }

static void stack_vec(double *x, double *y, double *z, int len_x, int len_y) {
 int jj;
 #pragma omp parallel for
 for (jj = 0; jj <= len_x-1 ; jj++) {
 z[jj] = x[jj];
 }
 #pragma omp parallel for
 for (jj = 0;jj <= len_y-1 ; jj++) {
 z[jj+len_x] = y[jj];
 }
 }

static void vec_add(double *x, double *y, double *z, int len) {
 int jj;
 #pragma omp parallel for
 for (jj = 0; jj <= len-1 ; jj++) {
 z[jj] = x[jj]+y[jj];
 }
}

static void vec_sub(double *x, double *y, double *z, int len) {
 int jj;
 #pragma omp parallel for
 for (jj = 0; jj <= len-1 ; jj++) {
 z[jj] = x[jj]-y[jj];
 }
 }

static void clip(double *x, double *l, double *u, int len) {
 int jj;
 #pragma omp parallel for
 for (jj = 0; jj <= len-1; jj++) {
 x[jj] = min(max(x[jj],l[jj]),u[jj]);
 }
 }

static void clip_soft(double *x, double *l, double *u, double *soft, int len) {
 int jj;
 #pragma omp parallel for
 for (jj = 0; jj <= len-1; jj++) {
 if (soft[jj] < 0) {
 x[jj] = min(max(x[jj],l[jj]),u[jj]);
 }
 else {
 if (x[jj] <= l[jj]-soft[jj]) {
 x[jj] = x[jj]+soft[jj];
 }
 else if (x[jj] >= u[jj]+soft[jj]) {
 x[jj] = x[jj]-soft[jj];
 }
 else {
 x[jj] = min(max(x[jj],l[jj]),u[jj]);
 } 
 }
 }
 }

static void copy_vec_part(double *x, double *y, int n){
 int jj;
 #pragma omp parallel for
 for (jj = 0; jj<= n-1 ; jj++) {
 y[jj] = x[jj];
 }
 }

void copy_double_to_float(double *x, float *y, int n){
 int jj;
 #pragma omp parallel for
 for (jj = 0; jj<= n-1 ; jj++) {
 y[jj] = (float) x[jj];
 }
 }

void copy_float_to_double(float *x, double *y, int n){
 int jj;
 #pragma omp parallel for
 for (jj = 0; jj<= n-1 ; jj++) {
 y[jj] = (double) x[jj];
 }
 }

static void copy_vec_part_negate(double *x, double *y, int n){
 int jj;
 #pragma omp parallel for
 for (jj = 0; jj<= n-1 ; jj++) {
 y[jj] = -x[jj];
 }
 }

static void scalar_mult(double alpha, double *x, int n){ 
 int jj;
 #pragma omp parallel for
 for (jj = 0; jj<= n-1 ; jj++) {
 x[jj] = alpha*x[jj];
 }
 }

static double norm_sq(double *x, int n) { int jj;
 double norm = 0;
 #pragma omp parallel for
 for (jj = 0; jj<= n-1 ; jj++) {
 norm += pow(x[jj],2);
 }
 return norm;
}

static double scalar_prod(double *x, double *y, int n) {
 double sum = 0;
 int jj;
 for (jj = 0 ; jj <= n-1 ; jj++) {
 sum += x[jj]*y[jj];
 }
 return sum;
 }

static void restart(double *x, double *x_old, double *y, double *y_old, double *tmp_var_p, double *tmp_var_p2, int n){
 double test;
 vec_sub(y_old,x,tmp_var_p,n);
 vec_sub(x,x_old,tmp_var_p2,n);
 test = scalar_prod(tmp_var_p,tmp_var_p2,n);
 if (test > 0) {
 copy_vec_part(x_old,y,n);
 copy_vec_part(x_old,x,n);
 }
 }

static double check_stop_cond_FGM(struct DIAG_MAT *Einv, double *y, double *v, double *tmp_var_p, double *tmp_var_p2, int p, double tol) {
 double cond_num;
 double cond_den;
 double cond;
 vec_sub(y,v,tmp_var_p,p);
mat_vec_mult_diag(Einv,tmp_var_p,tmp_var_p2);
cond_num = norm_sq(tmp_var_p2,p);
mat_vec_mult_diag(Einv,y,tmp_var_p);
cond_den = norm_sq(tmp_var_p,p);
mat_vec_mult_diag(Einv,v,tmp_var_p);
cond_den = max(max(norm_sq(tmp_var_p,p),cond_den),1e-8);
 cond = pow2(tol)-cond_num/cond_den;
 return cond;
 }

static double check_stop_cond_ADMM(struct DIAG_MAT *CT, struct DIAG_MAT *Einv, double *Cx, double *y, double *y_old, double *lambda, double *tmp_var_p, double *tmp_var_p2, double *tmp_var_n, int n, int p, double tol) {
 double cond_num_p;
 double cond_den_p;
 double cond_p;
 double cond_num_d;
 double cond_den_d;
 double cond_d;

mat_vec_mult_diag(Einv,Cx,tmp_var_p);
cond_den_p = norm_sq(tmp_var_p2,p);
 cond_den_p = max(max(norm_sq(y,n),cond_den_p),1e-8);
 vec_sub(tmp_var_p,y,tmp_var_p,p);
 cond_num_p = norm_sq(tmp_var_p,p);
 cond_p = pow2(tol)/4-cond_num_p/cond_den_p;

mat_vec_mult_diag(CT,lambda,tmp_var_n);
cond_den_d = max(norm_sq(tmp_var_n,n),1e-8);
 vec_sub(y,y_old,tmp_var_p,p);
mat_vec_mult_diag(Einv,tmp_var_p,tmp_var_p2);
mat_vec_mult_diag(CT,tmp_var_p2,tmp_var_n);
cond_num_d = norm_sq(tmp_var_n,n);
 cond_d = pow2(tol)/4-cond_num_d/cond_den_d;
 return min(cond_p,cond_d);
 }

void qp(struct DATA *d, double *x_out, int *iter, double *gt, double *bt) {

/* define data */
double q[60] = {0};

double q1[60] = {0};

double q2[60] = {0};

double l[60] = {0};

double u[60] = {0};

double tmp_var_p[60] = {0};

double tmp_var_p2[60] = {0};

double arg_prox_h[60] = {0};

double tmp_var_n_orig[540] = {0};

double tmp_var_n2_orig[540] = {0};

double r[540] = {0};

double lambda[60] = {0};
double y[60] = {0};

double x[60] = {0};

double y_old[60] = {0};

double Cx[60] = {0};

double tmp_var_n[60] = {0};

double tmp_var_n2[60] = {0};

int jj = 0;

double cond = -1;

mat_vec_mult_full(d->Q1,gt,q1);

mat_vec_mult_full(d->Q2,bt,q2);

vec_add(q1,q2,q,60);

copy_vec_part((double *) d->Lb,l,60);

copy_vec_part((double *) d->Ub,u,60);

mat_vec_mult_sparse(d->r2,bt,tmp_var_n2_orig);

vec_add(tmp_var_n_orig,tmp_var_n2_orig,r,540);

while ((jj < 2000) && (cond < 0)) {

jj++;

mat_vec_mult_diag(d->E,y,tmp_var_p);

vec_sub(lambda,tmp_var_p,tmp_var_p,60);

mat_vec_mult_full(d->M,tmp_var_p,tmp_var_n);

vec_add(tmp_var_n,q,x,60);

copy_vec_part(y,y_old,60);

mat_vec_mult_diag(d->C,x,tmp_var_p);

copy_vec_part(tmp_var_p,Cx,60);

scalar_mult(1.85,tmp_var_p,60);

mat_vec_mult_diag(d->E,y,tmp_var_p2);

scalar_mult(-0.85,tmp_var_p2,60);

vec_add(tmp_var_p,tmp_var_p2,tmp_var_p,60);

vec_add(lambda,tmp_var_p,tmp_var_p,60);

copy_vec_part(tmp_var_p,arg_prox_h,60);

clip(tmp_var_p,l,u,60);

mat_vec_mult_diag(d->Einv,tmp_var_p,y);

vec_sub(arg_prox_h,tmp_var_p,lambda,60);

if (mod(jj,10) == 0) {
 cond = check_stop_cond_ADMM(d->CT,d->Einv,Cx,y,y_old,lambda,tmp_var_p,tmp_var_p2,tmp_var_n,60,60,0.0001);
 }

}

mat_vec_mult_sparse(d->R,x,tmp_var_n_orig);

vec_add(tmp_var_n_orig,r,x_out,540);

*iter = jj;

}