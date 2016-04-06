#ifndef REGULATOR_H_
#define REGULATOR_H_

#include <stdlib.h>
#include <pthread.h>

typedef struct regul_t regul_t;

void* run_regul(regul_t *regul);

#endif /* REGULATOR_H_ */
