// Author: Wes Kendall
// Copyright 2011 www.mpitutorial.com
// This code is provided freely with the tutorials on mpitutorial.com. Feel
// free to modify it for your own use. Any distribution of the code must
// either provide a link to www.mpitutorial.com or keep this header intact.
//
// An intro MPI hello world program that uses MPI_Init, MPI_Comm_size,
// MPI_Comm_rank, MPI_Finalize, and MPI_Get_processor_name.
//
#include <mpi.h> // Library needed
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    // Message Passing Interface (MPI) is a standardized and portable message-passing standard
    // designed to function on parallel computing architectures.

    // The default communicator is called MPI_COMM_WORLD. It basically groups all the processes when the program started.


    // Initialize the MPI environment. The two arguments to MPI Init are not
    // currently used by MPI implementations, but are there in case future
    // implementations might need the arguments.
    MPI_Init(NULL, NULL); // (int *argc, char ***argv), but they are not used so we can send NULL as a parameter

    // Now I have an MPI environment
    // We can start to do "operations"

    // Comm_size: Determines the size of the group associated with a communicator 
    // comm means communicator, so the size of the group would be the number of processes using by a communicator -> NOT SURE

    // Get the number of processors
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); // MPI_Comm_size(MPI_Comm comm, int *size)


    // In MPI, rank is a logical way of numbering processors. The processor 1 has rank 0,
    // the processor 2 has rank 1, and so on. It's a identifier integer
    // MPI Comm rank - which one am I?
    // Comm_rank: Determines the rank of the calling process in the communicator 

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get_processor_name: Gets the name of the processor

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Print off a hello world message
    printf("Hello world from processor %s, rank %d out of %d processors\n",
            processor_name, world_rank, world_size);

    // allows the calling thread/program to pause its execution for a specified number of seconds.
    if(world_rank == 0) // why world_rank == 0 need to rest??? -> NOT SURE
        sleep(10);

    // Blocks until all processes in the communicator have reached this routine. 
    MPI_Barrier(MPI_COMM_WORLD); 
    printf("Finalizing process %d out of %d processors\n", world_rank, world_size);

    int number = 20;


    // MPI_Send: send a message to a particular process within an MPI communicator
    // a buffer is temporary storage we use when one component feeds data to the other, but their speeds aren’t the same
    // MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
    /*
        buf
            initial address of send buffer (choice)
        count
            number of elements in send buffer (nonnegative integer) 
        datatype
            datatype of each send buffer element (handle) 
        dest
            rank of destination (integer) 
        tag
            message tag (integer) 
        comm
            communicator (handle) 
    */

    // Send a message to a particular process
    MPI_SSend(&number, 1, MPI_INT, 1-world_rank, 0, MPI_COMM_WORLD);
    /*
        &number: 
            This is the starting address of the data you want to send
        1-world_rank: 
            This is the rank (or ID) of the destination process to which the data is being sent.
        0: 
            This is the tag for the message. Tags are useful for distinguishing different types of messages.
        
    */
   /*MPI_SSend(): This is a synchronous send operation. It will block (i.e., not return) until the receiving process has started its matching receive. 
   This ensures that once MPI_SSend() returns, the communication has effectively begun and the send buffer can be safely reused. */



    MPI_Recv(&number, 1, MPI_INT, 1-world_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Process %d received number %d from other process\n", world_rank, number);

    // Finalize the MPI environment. No more MPI calls can be made after this
    MPI_Finalize();
}
