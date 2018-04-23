/*
 * header.h
 *
 *  Created on: Apr 21, 2018
 *      Author: giraph
 */

#ifndef HEADER_H_
#define HEADER_H_
#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define log(format, ...){\
fprintf(stdout, "[%s.%s()#%d] " format "\n", __FILE__, __func__,  __LINE__, ##__VA_ARGS__ );\
}

#endif /* HEADER_H_ */
