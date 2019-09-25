#include "mpix.h"
#include <iostream>
#include <cmath>
using namespace std;

void broadcast(int* bvalue, int size, int bandwidth, int world_rank, int world_size, MPI_Status &status){
	int n = sqrt(world_size);
	int counter = 0;
	while(counter <= (size - 1)/bandwidth){
		if(world_rank == 0){
			MPI_xSend(&bvalue[counter*bandwidth], bandwidth, MPI_INT, 1, 1, MPI_COMM_WORLD);
			counter++;
		}
		else{
			int q = world_rank / n;
			int r = world_rank % n;
			if( q == 0){
				MPI_xRecv(&bvalue[counter*bandwidth], bandwidth, MPI_INT, world_rank - 1, 1, MPI_COMM_WORLD, &status);
				if(r != n-1){
					MPI_xSend(&bvalue[counter*bandwidth], bandwidth, MPI_INT, world_rank + 1, 1, MPI_COMM_WORLD);
				}
				MPI_xSend(&bvalue[counter*bandwidth], bandwidth, MPI_INT, world_rank + n, 1, MPI_COMM_WORLD);
			}
			else if(r == 0){
				MPI_xRecv(&bvalue[counter*bandwidth], bandwidth, MPI_INT, world_rank + 1, 1, MPI_COMM_WORLD, &status);
			}
			else{
				MPI_xRecv(&bvalue[counter*bandwidth], bandwidth, MPI_INT, world_rank - n, 1, MPI_COMM_WORLD, &status);
				if(q != n-1){
					MPI_xSend(&bvalue[counter*bandwidth], bandwidth, MPI_INT, world_rank + n, 1, MPI_COMM_WORLD);
				}
				if( r == 1 ){	
					MPI_xSend(&bvalue[counter*bandwidth], bandwidth, MPI_INT, world_rank - 1, 1, MPI_COMM_WORLD);
				}
			}
			counter++;
		}
	}
}

void add(int* bvalue, int counter, int bandwidth, int* xrecv_value, int size){
	for(int i = 0; i+counter*bandwidth<size && i < bandwidth; i++){
		bvalue[i + counter*bandwidth] += xrecv_value[i];
	}
}

void reduce(int* bvalue, int size, int bandwidth, int world_rank, int world_size, MPI_Status &status){
	int n = sqrt(world_size);
	int counter = 0;
	int xrecv_value[bandwidth];
	while(counter <= (size - 1)/ bandwidth){
		if( world_rank == 0){
			MPI_xRecv(xrecv_value, bandwidth, MPI_INT, world_rank + n, 1, MPI_COMM_WORLD, &status);
			add(bvalue, counter, bandwidth, xrecv_value, size);
			counter++;
		}
		else{
			int q = world_rank / n;
			int r = world_rank % n;
			if(r == 0){
				MPI_xRecv(xrecv_value, bandwidth, MPI_INT, world_rank + 1, 1, MPI_COMM_WORLD, &status);
				add(bvalue, counter, bandwidth, xrecv_value, size);
				if(q != n-1){
					MPI_xRecv(xrecv_value, bandwidth, MPI_INT, world_rank + n, 1, MPI_COMM_WORLD, &status);
					add(bvalue, counter, bandwidth, xrecv_value, size);
				}
				MPI_xSend(&bvalue[counter*bandwidth], bandwidth, MPI_INT, world_rank - n, 1, MPI_COMM_WORLD);
			}
			else if(q == 0){
				MPI_xSend(&bvalue[counter*bandwidth], bandwidth, MPI_INT, world_rank + n, 1, MPI_COMM_WORLD);
			}
			else{
				if(q == 1){
					MPI_xRecv(xrecv_value, bandwidth, MPI_INT, world_rank - n, 1, MPI_COMM_WORLD, &status);
					add(bvalue, counter, bandwidth, xrecv_value, size);
				}
				if(r != n-1){	
					MPI_xRecv(xrecv_value, bandwidth, MPI_INT, world_rank + 1, 1, MPI_COMM_WORLD, &status);
					add(bvalue, counter, bandwidth, xrecv_value, size);
				}
				MPI_xSend(&bvalue[counter*bandwidth], bandwidth, MPI_INT, world_rank - 1, 1, MPI_COMM_WORLD);

			}
			counter++;
		}
	}
}

void allreduce(int* bvalue, int size, int bandwidth, int world_rank, int world_size, MPI_Status &status){
	reduce(bvalue, size, bandwidth, world_rank, world_size, status);
	broadcast(bvalue, size, bandwidth, world_rank, world_size, status);
}

int main(int argc, char** argv){
	MPI_xInit(&argc, &argv);
	MPI_Status status;
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int size = 5200;
	int bandwidth = 5;
	int bvalue[size];
	for(int i = 0; i< size; i++){
		bvalue[i] = 1;
	}
	broadcast(bvalue, size, bandwidth, world_rank, world_size, status);
	

	cout<< world_rank <<":" << bvalue[0] <<" " << bvalue[size - 1]<< endl;
	// for(int i = 0; i<size; i++){
	// 	cout << " " <<bvalue[i];
	// }
	// cout << endl;
	MPI_xFinalize();
}