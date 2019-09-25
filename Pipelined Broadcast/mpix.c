#include "mpix.h"
#include <stdio.h>
#include <string.h>

int x_stats[100][2] ;
int x_rank ;
int x_p ;
int x_rootp ;

int MPI_xInit(int *argc, char ***argv)
{
    int i, j ;
    int ret ;

    ret = MPI_Init( argc, argv ) ;

    for( i = 0 ; i < 100 ; i++ )
      for( j = 0 ; j < 2 ; j++ )
          x_stats[i][j] = 0 ;

    MPI_Comm_size( MPI_COMM_WORLD, &x_p ) ;
    MPI_Comm_rank( MPI_COMM_WORLD, &x_rank ) ;
    for ( x_rootp = 1 ; x_rootp * x_rootp < x_p  ; x_rootp++ ) ;
    if ( (x_rank == 0) && ( x_rootp * x_rootp != x_p ) )
    {
        printf( "Not a perfect square.\n" );
        MPI_Abort( MPI_COMM_WORLD, 1 ) ;
    }
    if ( (x_rank == 0) && ( x_p > 100 ) )
    {
        printf( "Too many nodes.\n" );
        MPI_Abort( MPI_COMM_WORLD, 1 ) ;
    }

    return ret ;
}

int MPI_xFinalize(void)
{
    int i ;
    char pbuf[1000] ;
    char tbuf[1000] ;

    pbuf[0] = '\0' ;
    for( i = 0 ; i < 100 ; i++ )
    {
        if ( x_stats[i][0] != 0 )
        {
           sprintf( tbuf, "[%d,%d,%d];", i, x_stats[i][0], x_stats[i][1] ) ;
           strcat( pbuf, tbuf ) ;
        }
    }

    printf( "%d: STATS: %s\n", x_rank, pbuf ) ;

    return MPI_Finalize() ;
}

int MPI_xSend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
    int ret ;

    x_stats[dest][0]++ ;
    x_stats[dest][1]+= count ;
    ret = MPI_Send( buf, count, datatype, dest, tag, comm ) ;

    return ret ;
}

int MPI_xIsend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
    int ret ;

    x_stats[dest][0]++ ;
    x_stats[dest][1]+= count ;
    ret = MPI_Isend( buf, count, datatype, dest, tag, comm, request ) ;

    return ret ;
}

int MPI_xRecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
{
    return MPI_Recv( buf, count, datatype, source, tag, comm, status ) ;
}

int MPI_xIrecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
{
    return MPI_Irecv( buf, count, datatype, source, tag, comm, request ) ;
}
