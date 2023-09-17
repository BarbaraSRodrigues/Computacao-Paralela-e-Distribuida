/* Nome: Barbara Silveira Rodrigues
   RA: 2021.1.08.042
   Materia: Computacao Paralela e Distribuida
   Professor: Paulo Bressan
*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Aux function used in qsort
int compare(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    MPI_Status status;

    // Get the size and rank
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Variables to generate the numbers
    const int MAX_NUMBERS = 2000;
    int numbers[MAX_NUMBERS];
    int number_amount;

    if (world_rank == 0) { // Master Process
        srand(time(NULL));

        // Generate the amount of numbers
        for (int p = 1; p < world_size; p++) {
            number_amount = 1000 + (rand() % (MAX_NUMBERS - 999));
            // Generate the numbers inside the array
            for (int i = 0; i < number_amount; i++) {
                numbers[i] = rand() % 100;
            }

            // Send number_amount to the worker
            MPI_Send(&number_amount, 1, MPI_INT, p, 4, MPI_COMM_WORLD); // Use tag 4 for sending number_amount

            // Get the tag for the operation
            int tag = rand() % 4; // Generate random tag between 0 and 3

            // Send the array with the operation tag
            MPI_Send(numbers, number_amount, MPI_INT, p, tag, MPI_COMM_WORLD);
            printf("Master sent %d numbers to worker %d with operation tag %d\n", number_amount, p, tag);
        }

        for (int p = 1; p < world_size; p++) {
            float result;
            MPI_Recv(&result, 1, MPI_FLOAT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            int source = status.MPI_SOURCE;
            int tag = status.MPI_TAG;

            switch (tag) {
                case 0: printf("Received sum from worker %d: %f\n", source, result); break;
                case 1: printf("Received average from worker %d: %f\n", source, result); break;
                case 2: printf("Received max value from worker %d: %f\n", source, result); break;
                case 3: printf("Received median from worker %d: %f\n", source, result); break;
            }
        }

        // After sending data to all workers, send termination signal
        for (int p = 1; p < world_size; p++) {
            MPI_Send(NULL, 0, MPI_INT, p, 10, MPI_COMM_WORLD); // tag 10 signals termination
        }

    } else { // Workers Processes
        while (1) { // Keep workers working until tag 10

            // Receive the number_amount first
            MPI_Recv(&number_amount, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            // Termination signal received
            if (status.MPI_TAG == 10) {
                printf("Worker %d received termination signal.\n", world_rank);
                break;
            }

            // Now receive the actual array of numbers
            MPI_Recv(numbers, number_amount, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            float result; // Use float to cover all types of results
            switch (status.MPI_TAG) {
                case 0: { // Sum
                    result = 0;
                    for(int i = 0; i < number_amount; i++) {
                        result += numbers[i];
                    }
                    MPI_Send(&result, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD); // send the result to master
                    break;
                }
                case 1: { // Average
                    result = 0;
                    for(int i = 0; i < number_amount; i++) {
                        result += numbers[i];
                    }
                    result /= number_amount;
                    MPI_Send(&result, 1, MPI_FLOAT, 0, 1, MPI_COMM_WORLD);
                    break;
                }
                case 2: { // Max
                    result = numbers[0];
                    for(int i = 1; i < number_amount; i++) {
                        if(numbers[i] > result) {
                            result = numbers[i];
                        }
                    }
                    MPI_Send(&result, 1, MPI_FLOAT, 0, 2, MPI_COMM_WORLD);
                    break;
                }
                case 3: { // Median
                    qsort(numbers, number_amount, sizeof(int), compare);
                    if(number_amount % 2 == 0) {
                        result = (numbers[number_amount/2 - 1] + numbers[number_amount/2]) / 2.0;
                    } else {
                        result = numbers[number_amount/2];
                    }
                    MPI_Send(&result, 1, MPI_FLOAT, 0, 3, MPI_COMM_WORLD);
                    break;
                }
            }
        }
    }

    MPI_Finalize();
    return 0;
}
