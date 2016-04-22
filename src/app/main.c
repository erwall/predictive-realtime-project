#include "gui.h"
#include "regulator.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int main()
{
	pthread_t *reg = malloc(sizeof(pthread_t));
	pthread_t *plotter = malloc(sizeof(pthread_t));
	//pthread_t *input = malloc(sizeof(pthread_t)); //Hur vill du göra med input/output trådarna i GUI Charlie?
	//pthread_t *traj = malloc(sizeof(pthread_t));  //TODO
	regul_t *regul = malloc(sizeof(regul_t));

	/* Creating structs */
	regul_t *regul_struct = init_regul();
	data_t *data_struct = init_data();

	printf("Launching application...\n");
	/* Creating Threads*/

	/* Fix input to regul thread*/
	void ** regul_inputs = malloc(sizeof(void *)*2); //TODO högst oklart om detta är korrekt
	*regul_inputs = (void *) regul_struct;
	*(regul_inputs+1) = (void *) data_struct;

	if (pthread_create(reg, NULL, run_regul, (void *) regul_inputs)) {
		printf("Failed to create regulator thread.\n");
		exit(1);
	}

	//pthread_mutex_lock(data_struct->mutex);
	//pthread_mutex_unlock(data_struct->mutex);

	if (pthread_create(plotter, NULL, run_gui, (void *) data_struct)) {
		printf("Failed to create GUI thread.\n");
		exit(1);
	}

	/*if (pthread_create(traj, NULL, run_traj, NULL)) {
		printf("Failed to create trajectory thread.\n");
		exit(1);
	}*/

/*Wait for all threads to run/complete*/
	pthread_join(*plotter, NULL);
	pthread_join(*reg, NULL);
	//pthread_join(*traj, NULL);

free(regul_inputs); //Free vector of pointers, not the pointers themself

/* Free Threads*/
	free(reg);
	free(plotter);
//free(input);
	free(regul);
//	free(traj);

/* Delete structs*/
	destroy_regul(regul_struct);
	destroy_data(data_struct);
	return 0;
}
