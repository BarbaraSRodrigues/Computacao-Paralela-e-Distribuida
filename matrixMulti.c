/* Nome: Barbara Silveira Rodrigues
   RA: 2021.1.08.042
   Materia: Computacao Paralela e Distribuida
   Professor: Paulo Bressan
*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

/* Terminal:
    - Compile: mpicc matrixMulti.c -o matrixMulti -lm
    - Run: mpiexec -n 3 ./matrixMulti <num_rows_A> <num_cols_B>
*/

/* OBS:
    Using my computer I couldn't run above mpiexec -n 5 ./matrixMulti 4 4
    below that works fine.
*/

int main(int argc, char** argv) {

    // Variables
    int rank, size;
    double *A, *B, *C, *local_A, *local_C;
    int num_rows_A, num_cols_B, i, j, k;

    // Initialize, get rank and size
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // For this problem we need the processes to be equal to 3
    if (argc != 3) {
        if (rank == 0) {
            printf("Error in input arguments!\n");
            printf("Use: mpiexec -n <num_processes> ./program <num_rows_A> <num_cols_B>\n");
        }
        MPI_Finalize();
        exit(1);
    }

    // OBS: I wasn't sure if it was supposed to be random, so I did in a way that the user selects it
    // Get the number of rows and columns
    num_rows_A = atoi(argv[1]);
    num_cols_B = atoi(argv[2]);

    if (size != num_rows_A + 1) {
        if (rank == 0) {
            printf("Number of processes should be number of rows in A + 1 (for the master).\n");
        }
        MPI_Finalize();
        exit(2);
    }

    // Master: Creates and receive
    if (rank == 0) {
        // Allocate memory
        A = (double *) malloc(num_rows_A * num_cols_B * sizeof(double));
        B = (double *) malloc(num_cols_B * num_cols_B * sizeof(double));
        C = (double *) malloc(num_rows_A * num_cols_B * sizeof(double));

        // Initialize matrices with random values
        for (i = 0; i < num_rows_A * num_cols_B; i++) {
            A[i] = rand() % 10;
        }
        for (i = 0; i < num_cols_B * num_cols_B; i++) {
            B[i] = rand() % 10;
        }

        // Send each row of A and all of B to each worker
        printf("Master is sending each row of A and all of B to each worker.\n");
        for (i = 1; i < size; i++) {
            MPI_Send(&A[(i-1) * num_cols_B], num_cols_B, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
            MPI_Send(B, num_cols_B * num_cols_B, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
        }
    } else { // Worker: do the operations
        // Allocate memory
        local_A = (double *) malloc(num_cols_B * sizeof(double));
        local_C = (double *) malloc(num_cols_B * sizeof(double));
        B = (double *) malloc(num_cols_B * num_cols_B * sizeof(double));

        // Receive data from master
        printf("Workers receive data from Master.\n");
        MPI_Recv(local_A, num_cols_B, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(B, num_cols_B * num_cols_B, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Multiply the row of A by B
        for (i = 0; i < num_cols_B; i++) {
            local_C[i] = 0;
            for (j = 0; j < num_cols_B; j++) {
                local_C[i] += local_A[j] * B[j * num_cols_B + i];
            }
        }

        // Send result back to master
        MPI_Send(local_C, num_cols_B, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);

        // Free the memory
        free(local_A);
        free(B);
        free(local_C);
    }

    if (rank == 0) {
        // Master receive the matrix
        printf("Master receive the matrix.\n");
        for (i = 1; i < size; i++) {
            MPI_Recv(&C[(i-1) * num_cols_B], num_cols_B, MPI_DOUBLE, i, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // Output the matrix
        printf("Matrix C:\n");
        for (i = 0; i < num_rows_A; i++) {
            for (j = 0; j < num_cols_B; j++) {
                printf("%6.2f ", C[i * num_cols_B + j]);
            }
            printf("\n");
        }

        // Free the memory allocated for the matrices
        free(A);
        free(B);
        free(C);
    }

    // Finalize
    MPI_Finalize();
    return 0;
}
