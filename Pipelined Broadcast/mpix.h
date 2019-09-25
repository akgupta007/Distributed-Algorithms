#include "mpi.h"

int MPI_xInit(int *argc, char ***argv);
int MPI_xFinalize(void);
int MPI_xSend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) ;
int MPI_xRecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status) ;
int MPI_xIsend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) ;
int MPI_xIrecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request) ;
