#include "output.h"
#include <pthread.h>
#include <stdlib.h>

void run_plot() 
{
	while(true) 
	{
		float* val = read_data();
	}
}

float* read_data()
{
	/*
		Let the GUI handle inputs...
	*/
}

float* send_data() {
	float* data = read_data();
	return data;
}

