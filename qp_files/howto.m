Usage C:
 - compile qp_files/QPgen.c qp_files/init_data.c qp_files/free_data.c
 - initialize data (ones before solving problems, input to qp()):
       void init_data(struct DATA *d);
 - free data (ones after solving problems):
       void free_data(struct DATA *d);
 - solve using function qp(d,sol,iter,gt,bt):
       void qp(struct DATA *d,double *sol, int *iter, double *gt,double *bt);