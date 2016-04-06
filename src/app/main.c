#include "gui.h"
#include "regulator.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int main()
{
	pthread_t *reg = malloc(sizeof(pthread_t));
	pthread_t *gui = malloc(sizeof(pthread_t));
	regul_t *regul = malloc(sizeof(regul_t));

	pthread_mutex_init(regul->mutex, NULL);

	printf("Launching application...\n");

	if (pthread_create(reg, NULL, run_regul, regul)) {
		printf("Failed to create regulator thread.\n");
		exit(1);
	}

	if (pthread_create(gui, NULL, run_regul, regul)) {
		printf("Failed to create regulator thread.\n");
		exit(1);
	}

	if (pthread_create)

	free(reg);
	free(gui);
}
