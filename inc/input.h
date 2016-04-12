#ifndef OUTPUT_H
#define OUTPUT_H

/*
Main loop to keep the update process alive. This contains the GUI which the user will communicate with.
*/
void run_plot() {};

/*
Acquire the data that is to be plotted. Must contain blocks for invalid input values.
*/
float* read_data();

/*
Function to call from other programs which sends the acquired input data. 
*/
float* send_data();

#endif OUTPUT_H