#include "gui.h"
#include "regulator.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int main()
{
	pthread_t *reg = malloc(sizeof(pthread_t));
	pthread_t *gui = malloc(sizeof(pthread_t));
	pthread_t *input = malloc(sizeof(pthread_t)); //Hur vill du göra med input/output trådarna i GUI Charlie?
	pthread_t *traj = malloc(sizeof(pthread_t));
	regul_t *regul = malloc(sizeof(regul_t));

	/* Creating structs */
	regul_t *regul_struct = init_regul();
	data_t *data_struct = init_data();

	printf("Launching application...\n");
	/* Creating Threads*/

	/* Fix input to thread*/
	void ** regul_inputs = malloc(sizeof(void *)*2); //TODO högst oklart om detta är korrekt
	*regul_inputs = (void *) regul_struct;
	*(regul_inputs+1) = (void *) data_struct;

	if (pthread_create(reg, NULL, run_regul, (void *) regul_inputs)) {
		printf("Failed to create regulator thread.\n");
		exit(1);
	}

	free(regul_inputs);

	if (pthread_create(gui, NULL, run_gui, NULL)) {
		printf("Failed to create GUI thread.\n");
		exit(1);
	}

	if (pthread_create(traj, NULL, run_traj, NULL)) {
		printf("Failed to create trajectory thread.\n");
		exit(1);
	}

/*Wait for all threads to run*/
	pthread_join(*gui, NULL);
	pthread_join(*reg, NULL);
	pthread_join(*traj, NULL);

/* Free Threads*/
	free(reg);
	free(gui);
	free(input);
	free(regul);
	free(traj);

/* Delete structs*/
	destroy_regul(regul_struct);
	destroy_data(data_struct);
	return 0;
}
