#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){

    // Variable used for limit
    const int PING_PONG_LIMIT = 5;

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Find out rank, size
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // We are assuming 2 processes for this task
    if(world_size != 2){
        fprintf(stderr, "World size must be two for %s\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Count how many ping pong actions we had
    int ping_pong_count = 0;

    /*if world_rank is 0:
        partner_rank = (0 + 1) % 2 = 1 % 2 = 1
      If world_rank is 1:
        partner_rank = (1 + 1) % 2 = 2 % 2 = 0
    */
    int partner_rank = (world_rank + 1) % 2;

    // Now we enter the loop for the pingpong "match". It's stops when reaches the limit
    while(ping_pong_count < PING_PONG_LIMIT){
        // If ping_pong_count is even, then the process with rank 0 (even rank) sends the message.
        // If ping_pong_count is odd, then the process with rank 1 (odd rank) sends the message
        if(world_rank == ping_pong_count % 2){
            // Increment the ping pong count before you send it
            ping_pong_count++;
            MPI_Send(&ping_pong_count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD);
            printf("%d sent and incremented ping_pong_count %d to %d\n", world_rank, ping_pong_count, partner_rank);
        } else {
            MPI_Recv(&ping_pong_count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("%d received ping_pong_count %d from %d\n", world_rank, ping_pong_count, partner_rank);
        }
    }

    /* EXAMPLE
        0 sent and incremented ping_pong_count 1 to 1
        1 received ping_pong_count 1 from 0
        1 sent and incremented ping_pong_count 2 to 0
        0 received ping_pong_count 2 from 1
        0 sent and incremented ping_pong_count 3 to 1
        1 received ping_pong_count 3 from 0
        1 sent and incremented ping_pong_count 4 to 0
        0 received ping_pong_count 4 from 1
    */

    // Finalize
    MPI_Finalize();
}