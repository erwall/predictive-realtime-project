#ifndef INPUT_H_
#define INPUT_H_

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

/*
Call to check if user has inputted that he wants to exit. return 1 for done, 0 for not done.
*/
int* isdone();

#endif INPUT_H