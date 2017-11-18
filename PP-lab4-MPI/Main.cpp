#include "mpi.h"
#include "stdio.h"
#include <iostream>
#include "stdlib.h"
#include <stdio.h>
#include <math.h>
#include <typeinfo>

#define MAX_LENGTH 50001

#pragma runtime_checks( "", off ) 

using namespace std;

class Experiment {
public:
	Experiment(int argc, char* argv[], int messageSize, int passNumber);
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

Experiment::Experiment(int argc, char* argv[], int messageSize, int passNumber)
	:messageSize(messageSize), passNumber(passNumber){
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &procNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
}

Experiment::~Experiment() {
	MPI_Finalize();
}

void Experiment::MessageExchangeSend() {
	MPI_Status status;
	char buffer1[MAX_LENGTH];
	char buffer2[MAX_LENGTH];
	int passes = 0;
	for (int i = 0; i < messageSize; i++) buffer2[i] = 'i';
	for (passes = 0; passes < passNumber; ++passes) {
		if (procRank == 0) {
			MPI_Send(&buffer2, messageSize, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
			MPI_Recv(&buffer1, messageSize, MPI_CHAR, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		} else {
			MPI_Recv(&buffer1, messageSize, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			MPI_Send(&buffer2, messageSize, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
		}
	}
	cout << "Out of send" << endl;
}

void Experiment::MessageExchangeSsend() {
	MPI_Status status;
	char buffer1[MAX_LENGTH];
	char buffer2[MAX_LENGTH];
	int passes = 0;
	for (int i = 0; i < messageSize; i++) buffer2[i] = 'i';
	for (passes = 0; passes < passNumber; ++passes) {
		if (procRank == 0) {
			MPI_Ssend(&buffer2, messageSize, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
			MPI_Recv(&buffer1, messageSize, MPI_CHAR, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		} else {
			MPI_Recv(&buffer1, messageSize, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			MPI_Ssend(&buffer2, messageSize, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
		}
	}
	cout << "Out of Ssend" << endl;
}

void Experiment::MessageExchangeBsend() {
	MPI_Status status;
	char buffer1[MAX_LENGTH];
	char buffer2[MAX_LENGTH];
	for (int i = 0; i < messageSize; i++) buffer2[i] = 'i';
	int passes = 0;
	MPI_Buffer_attach(buffer2, messageSize + MPI_BSEND_OVERHEAD);
	for (passes = 0; passes < passNumber; ++passes) {
		if (procRank == 0) {
			MPI_Bsend(&buffer2, messageSize, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
			MPI_Recv(&buffer1, messageSize, MPI_CHAR, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		}
		else if (procRank == 1) {
			MPI_Recv(&buffer1, messageSize, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			MPI_Bsend(&buffer2, messageSize, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
		}
	}
	MPI_Buffer_detach(buffer2, &messageSize);
	cout << "Out of Bsend" << endl;
}

void Experiment::MessageExchangeRsend() {
	MPI_Status status;
	char buffer1[MAX_LENGTH];
	char buffer2[MAX_LENGTH];
	int passes = 0;
	for (int i = 0; i < messageSize; i++) buffer2[i] = 'i';
	for (passes = 0; passes < passNumber; ++passes) {
		if (procRank == 0) {
			MPI_Rsend(&buffer2, messageSize, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
			MPI_Recv(&buffer1, messageSize, MPI_CHAR, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		}
		else {
			MPI_Recv(&buffer1, messageSize, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			MPI_Rsend(&buffer2, messageSize, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
		}
	}
	cout << "Out of Rsend" << endl;
}



typedef void(Experiment::*IntMethodWithNoParameter) ();

int main(int argc, char* argv[]) {
	double t1, t2;
	int messageSize = 1;
	int numOfPasses = 1000000;

	Experiment exper = Experiment(argc, argv,messageSize, numOfPasses);
	
	IntMethodWithNoParameter functions[] = {
		//&Experiment::MessageExchangeSend,
		//&Experiment::MessageExchangeSsend,
		//&Experiment::MessageExchangeBsend,
		&Experiment::MessageExchangeRsend,
		//&Experiment::MessageExchangeNonBlocking,
		//&Experiment::MessageExchangeSendRecvCombined,
	};

	// evaluating the dependence between 
	// message size and execution time

	for (int messageSize = 10; messageSize < 50000; messageSize*=2) {
		exper.messageSize = messageSize;
		for (int i = 0; i < 1; i++) {
			cout << "Send for message size " << messageSize << " time ";
			t1 = MPI_Wtime();
			(exper.*functions[i])();
			t2 = MPI_Wtime();
			cout << t2 - t1 << endl;
		}
	}
	return 0;
}