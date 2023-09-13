/*Exercicio 1
    Envie o numero do processo para o processo acima, e o último para o primeiro, para N processos, usando send/recv sincronizados
*/

/*
    send the number of the process to the process above and the last to the first using N processes, use send/recv synch
*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv){

    // Initialize
    MPI_Init(NULL, NULL);

    // Get the size (number of processes)
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the ranks of the processes
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Check the processes
    printf("Hello from processor: %s of rank %d out of %d processors\n", processor_name, world_rank, world_size);

    // Synch
    if(world_rank == 0)
        sleep(10);
    
    MPI_Barrier(MPI_COMM_WORLD); 
    //printf("Finalizing process %d out of %d processors\n", world_rank, world_size);

    // Exercise
    int number;
    number = world_rank;
    if(number == 0){
        MPI_Recv(&number, 1, MPI_INT, world_size-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Processor %d received %d from other process\n", world_rank, number);
        MPI_Send(&number, 1, MPI_INT, world_rank+1, 0, MPI_COMM_WORLD);
    } else {
        MPI_Send(&number, 1, MPI_INT, (world_rank+1)%world_size, 0, MPI_COMM_WORLD);
        MPI_Recv(&number, 1, MPI_INT, world_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Processor %d received %d from other process\n", world_rank, number);
    }

    // Finalize
    MPI_Finalize();
}
