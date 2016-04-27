#include "regulator.h"
#include "run_gui.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int main()
{
	pthread_t regul_thread;
	pthread_t gui_thread;

	/* Creating structs */
	thread_args_t *thread_args = (thread_args_t*) malloc(
			sizeof(thread_args_t));
	thread_args->regul = init_regul();
	thread_args->data = init_data();
	thread_args->run = 1;

	printf("Launching application...\n");
	/* Creating Threads*/

	if (pthread_create(&regul_thread, NULL, run_regul,
			(void*) thread_args)) {
		printf("Failed to create regulator thread.\n");
		exit(1);
	}

	if (pthread_create(&gui_thread, NULL, run_gui, (void*) thread_args)) {
		printf("Failed to create GUI thread.\n");
		exit(1);
	}

	/* Wait for GUI thread to finish, which turns regulator off */
	pthread_join(gui_thread, NULL);
	/* Wait for regulator to finish last loop */
	pthread_join(regul_thread, NULL);

	/* Free structs*/
	free_regul(thread_args->regul);
	free_data(thread_args->data);
	free(thread_args);

	return 0;
}
