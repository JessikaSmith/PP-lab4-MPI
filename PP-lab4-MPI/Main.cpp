#include "mpi.h"
#include "stdio.h"
#include <iostream>
#include "stdlib.h"
#include <stdio.h>
#include <math.h>
#include <typeinfo>

using namespace std;


class Experiment {
public:
	Experiment(int messageSize, int passNumber);
	int messageSize, passNumber, procRank, procNum, recvRank;
	virtual ~Experiment();
	
	// Program in which 2 processes repeatedly
	// exchange messages of n bytes
	// point-to-point communication

	void MessageExchangeSend();
	void MessageExchangeSsend();
	void MessageExchangeBsend();
	void MessageExchangeRsend();
	void MessageExchangeNonBlocking();
};

Experiment::Experiment(int messageSize, int passNumber)
	:messageSize(messageSize), passNumber(passNumber){
	procNum = 2;
	procRank = 0;
	MPI_Comm_size(MPI_COMM_WORLD, &procNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
}

Experiment::~Experiment() {
	MPI_Finalize();
}

// overload with different types


void Experiment::MessageExchangeSend() {
	MPI_Status status1, status2;
	// buffer sizes are not OK
	int* buffer1 = (int*)(malloc(messageSize));
	int* buffer2 = (int*)(malloc(messageSize));
	int passes = 0;
	while (passes < passNumber) {
		if (procRank == 0) {
			for (int i = 0; i < messageSize; i++) buffer1[i] = i;
			for (int i = 0; i < messageSize; i++) buffer2[i] = -i;
			passes++;
			MPI_Recv(&buffer2, messageSize, MPI_INT, 1, 0, MPI_COMM_WORLD, &status1);
			MPI_Send(&buffer1, messageSize, MPI_INT, 1, 1, MPI_COMM_WORLD);
		}
		else if (procRank == 1) {
			for (int i = 0; i < messageSize; i++) buffer2[i] = i;
			for (int i = 0; i < messageSize; i++) buffer1[i] = -i;
			MPI_Send(&buffer1, messageSize, MPI_INT, 0, 0, MPI_COMM_WORLD);
			MPI_Recv(&buffer2, messageSize, MPI_INT, 0, 1, MPI_COMM_WORLD, &status2);
		}
	}
}

void Experiment::MessageExchangeSsend() {
	MPI_Status status1, status2;
	// buffer sizes are not OK
	int* buffer1 = (int*)(malloc(messageSize));
	int* buffer2 = (int*)(malloc(messageSize));
	int passes = 0;
	while (passes < passNumber) {
		if (procRank == 0) {
			for (int i = 0; i < messageSize; i++) buffer1[i] = i;
			for (int i = 0; i < messageSize; i++) buffer2[i] = -i;
			passes++;
			MPI_Recv(&buffer2, messageSize, MPI_INT, 1, 0, MPI_COMM_WORLD, &status1);
			MPI_Ssend(&buffer1, messageSize, MPI_INT, 1, 1, MPI_COMM_WORLD);
		}
		else if (procRank == 1) {
			for (int i = 0; i < messageSize; i++) buffer2[i] = i;
			for (int i = 0; i < messageSize; i++) buffer1[i] = -i;
			MPI_Ssend(&buffer1, messageSize, MPI_INT, 0, 0, MPI_COMM_WORLD);
			MPI_Recv(&buffer2, messageSize, MPI_INT, 0, 1, MPI_COMM_WORLD, &status2);
		}
	}
}

// add MPI_Buffer_attach
void Experiment::MessageExchangeBsend() {
	MPI_Status status1, status2;
	// buffer sizes are not OK
	int* buffer1 = (int*)(malloc(messageSize));
	int* buffer2 = (int*)(malloc(messageSize));
	int passes = 0;
	while (passes < passNumber) {
		if (procRank == 0) {
			for (int i = 0; i < messageSize; i++) buffer1[i] = i;
			for (int i = 0; i < messageSize; i++) buffer2[i] = -i;
			passes++;
			MPI_Recv(&buffer2, messageSize, MPI_INT, 1, 0, MPI_COMM_WORLD, &status1);
			MPI_Bsend(&buffer1, messageSize, MPI_INT, 1, 1, MPI_COMM_WORLD);
		}
		else if (procRank == 1) {
			for (int i = 0; i < messageSize; i++) buffer2[i] = i;
			for (int i = 0; i < messageSize; i++) buffer1[i] = -i;
			MPI_Bsend(&buffer1, messageSize, MPI_INT, 0, 0, MPI_COMM_WORLD);
			MPI_Recv(&buffer2, messageSize, MPI_INT, 0, 1, MPI_COMM_WORLD, &status2);
		}
	}
}

typedef void(Experiment::*IntMethodWithNoParameter) ();

int main(int argc, char* argv[]) {
	double t1, t2;
	int messageSize = 1;
	int numOfPasses = 1;
	MPI_Init(&argc, &argv);
	Experiment exper = Experiment(messageSize, numOfPasses);
	
	IntMethodWithNoParameter functions[] = {
		&Experiment::MessageExchangeSend,
		&Experiment::MessageExchangeSsend,
		//&Experiment::MessageExchangeBsend,
		//&Experiment::MessageExchangeRsend,
		//&Experiment::MessageExchangeNonBlocking
	};

	// evaluating the dependence between 
	// message size and execution time


	for (int messageSize = 1; messageSize < 3; messageSize++) {
		for (int i = 0; i < 2; i++) {
			cout << "Send for message size " << messageSize << " time ";
			exper.messageSize = messageSize;
			t1 = MPI_Wtime();
			(exper.*functions[i])();
			t2 = MPI_Wtime();
			cout << t2 - t1 << endl;
		}
	}
	return 0;
}
