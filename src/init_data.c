#include <stdlib.h>
#include <stdio.h>
#include "QPgen.h"
#include "data_struct.h"

static void read_binary_double(double *data, size_t len, char* file_name) {
FILE *fp = fopen(file_name,"rb");
if (!fp) {perror("File open error");}
fread(data, sizeof(double), len, fp);
fclose(fp);
}

static void read_binary_int(int *data, size_t len, char* file_name) {
FILE *fp = fopen(file_name,"rb");
if (!fp) {perror("File open error");}
fread(data, sizeof(int), len, fp);
fclose(fp);
}

static void read_binary_float(float *data, size_t len, char* file_name) {
FILE *fp = fopen(file_name,"rb");
if (!fp) {perror("File open error");}
fread(data, sizeof(float), len, fp);
fclose(fp);
}

void init_data(struct DATA *d) {

double *Mdata = malloc(3600*sizeof(double));

read_binary_double(Mdata,3600,"qp_files/qp_data/Mdata.bin");

struct FULL_MAT *M = malloc(sizeof(struct FULL_MAT));

M->n = 60;

M->m = 60;

M->data = Mdata;

d->M = M;

double *Q1data = malloc(32400*sizeof(double));

read_binary_double(Q1data,32400,"qp_files/qp_data/Q1data.bin");

struct FULL_MAT *Q1 = malloc(sizeof(struct FULL_MAT));

Q1->n = 60;

Q1->m = 540;

Q1->data = Q1data;

d->Q1 = Q1;

double *Q2data = malloc(960*sizeof(double));

read_binary_double(Q2data,960,"qp_files/qp_data/Q2data.bin");

struct FULL_MAT *Q2 = malloc(sizeof(struct FULL_MAT));

Q2->n = 60;

Q2->m = 16;

Q2->data = Q2data;

d->Q2 = Q2;

double *Cdata = malloc(60*sizeof(double));

read_binary_double(Cdata,60,"qp_files/qp_data/Cdata.bin");

struct DIAG_MAT *C = malloc(sizeof(struct DIAG_MAT));

C->n = 60;

C->data = Cdata;

d->C = C;

double *CTdata = malloc(60*sizeof(double));

read_binary_double(CTdata,60,"qp_files/qp_data/CTdata.bin");

struct DIAG_MAT *CT = malloc(sizeof(struct DIAG_MAT));

CT->n = 60;

CT->data = CTdata;

d->CT = CT;

double *Edata = malloc(60*sizeof(double));

read_binary_double(Edata,60,"qp_files/qp_data/Edata.bin");

struct DIAG_MAT *E = malloc(sizeof(struct DIAG_MAT));

E->n = 60;

E->data = Edata;

d->E = E;

double *Einvdata = malloc(60*sizeof(double));

read_binary_double(Einvdata,60,"qp_files/qp_data/Einvdata.bin");

struct DIAG_MAT *Einv = malloc(sizeof(struct DIAG_MAT));

Einv->n = 60;

Einv->data = Einvdata;

d->Einv = Einv;

double *Lb = malloc(60*sizeof(double));

read_binary_double(Lb,60,"qp_files/qp_data/Lb.bin");

d->Lb = Lb;

double *Ub = malloc(60*sizeof(double));

read_binary_double(Ub,60,"qp_files/qp_data/Ub.bin");

d->Ub = Ub;

double *r2data = malloc(4178*sizeof(double));

read_binary_double(r2data,4178,"qp_files/qp_data/r2data.bin");

int *r2cols = malloc(4178*sizeof(int));

read_binary_int(r2cols,4178,"qp_files/qp_data/r2cols.bin");

int *r2rows = malloc(541*sizeof(int));

read_binary_int(r2rows,541,"qp_files/qp_data/r2rows.bin");

struct SPARSE_MAT *r2 = malloc(sizeof(struct SPARSE_MAT));

r2->nnz = 4178;

r2->nr = 540;

r2->row = r2rows;

r2->col = r2cols;

r2->data = r2data;

d->r2 = r2;

double *Rdata = malloc(9240*sizeof(double));

read_binary_double(Rdata,9240,"qp_files/qp_data/Rdata.bin");

int *Rcols = malloc(9240*sizeof(int));

read_binary_int(Rcols,9240,"qp_files/qp_data/Rcols.bin");

int *Rrows = malloc(541*sizeof(int));

read_binary_int(Rrows,541,"qp_files/qp_data/Rrows.bin");

struct SPARSE_MAT *R = malloc(sizeof(struct SPARSE_MAT));

R->nnz = 9240;

R->nr = 540;

R->row = Rrows;

R->col = Rcols;

R->data = Rdata;

d->R = R;

}

