#ifndef OUTPUT_H
#define OUTPUT_H

#include <qcustomplot.h>

/*
Main loop to keep the update process alive.
*/
void run_plot();

/*
	Acquire the data that is to be plotted.
*/
float* read_data();

#endif OUTPUT_H