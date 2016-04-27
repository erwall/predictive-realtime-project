#ifndef RUN_GUI_H_
#define RUN_GUI_H_

#include "regulator.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void *run_gui(void *arg);

#ifdef NO_GUI
void *run_gui(void *arg)
{
	thread_args_t* thread_args = (thread_args_t*) arg;
	thread_args->regul->on = 1;

	return NULL;
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* RUN_GUI_H_ */
