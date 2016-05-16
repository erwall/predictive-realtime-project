#include <stdlib.h>
#include "QPgen.h"
#include "data_struct.h"

 void free_data(struct DATA *d) {

free(d->M->data);

free(d->M);

free(d->Q1->data);

free(d->Q1);

free(d->Q2->data);

free(d->Q2);

free(d->C->data);

free(d->C);

free(d->CT->data);

free(d->CT);

free(d->E->data);

free(d->E);

free(d->Einv->data);

free(d->Einv);

free(d->Lb);

free(d->Ub);

free(d->r2->data);

free(d->r2->row);

free(d->r2->col);

free(d->r2);

free(d->R->data);

free(d->R->row);

free(d->R->col);

free(d->R);

free(d);

}

