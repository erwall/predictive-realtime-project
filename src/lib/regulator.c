#include "regulator.h"

void* run_regul(regul_t *regul)
{
	while(1) {
		regul->pitch_ref = 0;
		regul->yaw_ref = 0;
		/*
		Switch for read_data() from the input here later.
		*/

		pthread_mutex_lock(regul->mutex);

		pthread_mutex_unlock(regul->mutex);
	}
	return NULL;
}

