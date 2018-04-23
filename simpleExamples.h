/*
 * simpleExamples.h
 *
 *  Created on: Apr 21, 2018
 *      Author: giraph
 */

#ifndef SIMPLEEXAMPLES_H_
#define SIMPLEEXAMPLES_H_
#include "header.h"

int my_rank; /* rank of process */
int num_procs; /* number of processes */
int source; /* rank of sender */
int dest = 0; /* rank of receiver */
int tag = 0; /* tag for messages */
#define buffsize 10000
char message[buffsize]; /* storage for message */
MPI_Status status; /* return status for receive */

void worker_init() {
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
}

void worker_finalize() {
	MPI_Finalize();
}
void simpleSendRecv() {

	if (my_rank != 0) { //worker
		sprintf(message, "pid %d greeting!", my_rank);

		MPI_Send(message, strlen(message) + 1, MPI_CHAR, dest, tag,
				MPI_COMM_WORLD);
		log("rank#%d greeting finished!", my_rank);

	} else { //master
		log("Num processes: %d", num_procs);
		for (source = 1; source < num_procs; source++) {
			MPI_Probe(source, tag, MPI_COMM_WORLD, &status);
			int probed_count;
			MPI_Get_count(&status, MPI_CHAR, &probed_count);
			log("rank#%d probed count:%d", my_rank, probed_count);
			sleep(3);

			MPI_Recv(message, probed_count, MPI_CHAR, source, tag,
					MPI_COMM_WORLD, &status);
			int Recvd_count;
			MPI_Get_count(&status, MPI_CHAR, &Recvd_count);
			log("Process 0 received :%s of length %d", message, Recvd_count);
		}
	}
}

void simpleIsendIrecv() {

	if (my_rank != 0) { //worker
		sprintf(message, "pid %d greeting!", my_rank);

		MPI_Request request[2];
		sleep(5);
		MPI_Isend(message, 10000, MPI_CHAR, 0, 0, MPI_COMM_WORLD, request);
		log("rank#%d first send finished", my_rank)
		MPI_Isend(message + 3, buffsize - 3, MPI_CHAR, 0, 0, MPI_COMM_WORLD,
				request + 1);
		log("rank#%d second send finished", my_rank)

		sprintf(message, "modify", my_rank);

		MPI_Wait(request, &status);
		log("rank#%d first wait finished", my_rank)
		MPI_Wait(request + 1, &status);
		log("rank#%d second wait finished", my_rank)

	} else { //master
		log("rank#%d Num processes: %d", my_rank, num_procs);
		int probed_count;
		int Recvd_count;
		MPI_Request request[2];
		for (source = 1; source < num_procs; source++) {

			MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			MPI_Get_count(&status, MPI_CHAR, &probed_count);
			log("rank#%d first probed count:%d (%d,%d)",
					my_rank, probed_count, status.MPI_SOURCE, status.MPI_TAG);

			MPI_Irecv(message, probed_count, MPI_CHAR, status.MPI_SOURCE,
					status.MPI_TAG, MPI_COMM_WORLD, request);
			log("rank#%d first invoke Irecv", my_rank);
			MPI_Wait(request, &status);
			MPI_Get_count(&status, MPI_CHAR, &Recvd_count);
			log("rank#%d first recv count:%d", my_rank, Recvd_count);

			MPI_Probe(source, 0, MPI_COMM_WORLD, &status);
			MPI_Get_count(&status, MPI_CHAR, &probed_count);
			log("rank#%d second probed count:%d (%d,%d)",
					my_rank, probed_count, status.MPI_SOURCE, status.MPI_TAG);

			MPI_Irecv(message + 3, probed_count, MPI_CHAR, status.MPI_SOURCE,
					status.MPI_TAG, MPI_COMM_WORLD, request + 1);
			log("rank#%d second invoke Irecv", my_rank);
			MPI_Wait(request + 1, &status);
			MPI_Get_count(&status, MPI_CHAR, &Recvd_count);
			log("rank#%d second recv count:%d", my_rank, Recvd_count);

			log("rank#%d received :%s of length %d",
					my_rank, message, Recvd_count);
		}
	}

}

void simpleBcast() {

	if (my_rank == 0) {
		sprintf(message, "Hi, how are you?");
	}

	log("msg.len in id %d is %d", my_rank, strlen(message));

	int msglen = strlen(message) + 1;
	MPI_Bcast(&msglen, 1, MPI_INT, dest, MPI_COMM_WORLD);
	MPI_Bcast(message, msglen, MPI_CHAR, dest, MPI_COMM_WORLD);
	if (my_rank != 0) {
		log("Process %d received %s\n", my_rank, message);
	}
	log("finish in pid:%d\n", my_rank);
	MPI_Barrier (MPI_COMM_WORLD);
	log("after Barrier in pid:%d\n", my_rank);
}

void calc_pi(int rank, int num_procs) {
	int i;
	int num_intervals;
	double h;
	double mypi;
	double pi;
	double sum;
	double x;
	if (rank == 0) {
		num_intervals = 100000000;
	}

	MPI_Bcast(&num_intervals, 1, MPI_INT, 0, MPI_COMM_WORLD);

	h = 1.0 / (double) num_intervals;
	sum = 0.0;

	for (i = rank + 1; i <= num_intervals; i += num_procs) {
		x = h * ((double) i - 0.5);
		sum += (4.0 / (1.0 + x * x));
	}

	mypi = h * sum;
	MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		printf("PI is approximately %.16f\n", pi);
	}
}

#endif /* SIMPLEEXAMPLES_H_ */
