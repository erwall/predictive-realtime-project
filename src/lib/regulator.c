#include "regulator.h"

struct regul_t {
	int pith_ref;
	int yaw_ref;
	pthread_mutex_t *mutex;
} regul_t;

void* run_regul(regul_t *regul)
{
	while(isdone() != 1)
	{
		regul_t.pitch_ref = 0;
		regul_t.yaw_ref = 0;
		/*
		Switch for read_data() from the input here later.
		*/

		pthread_mutex_lock(regul->mutex);

		pthread_mutex_unlock(regul->mutex);
	}
	return NULL;
}

