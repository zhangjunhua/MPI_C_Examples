/*
 ============================================================================
 Name        : MPI_C_Examples.c
 Author      : Junhua Zhang
 Version     :
 Copyright   : Your copyright notice
 Description : Calculate Pi in MPI
 ============================================================================
 */

#include "simpleExamples.h"



int main(int argc, char *argv[]) {
	worker_init();

//	simpleSendRecv();
	simpleIsendIrecv();

	worker_finalize();
	return 0;
}
