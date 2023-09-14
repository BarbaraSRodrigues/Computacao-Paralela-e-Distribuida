/* Nome: Barbara Silveira Rodrigues
   RA: 2021.1.08.042
*/

/* EXERCÍCIOS:
    1 -> Faça um programa com a estrutura mestre/trabalhador, onde:
        - mestre: gera uma quantidade aleatória entre 1000 e 2000 numeros de valores aleatorios entre 0 e 99 e envia para cada trabalhador.
        - trabalhador: recebe os valores gerados pelo mestre, calcula a somatoria dos valores e devolve para o mestre
*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv){

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

    if(world_rank == 0){

        // Variables used for the sum
        int sum = 0, sumproc, localsum = 0;

        // Generate a number in range [1000, 2000]
        srand(time(NULL));
        for(int p = 1; p < world_size; p++){
            // Generate a number in range [1000, 2000]
            number_amount = 1000 + (rand() % (MAX_NUMBERS - 999)); 

            // Populate the array with numbers in range [0, 99]
            for(int i = 0; i < number_amount; i++){
                numbers[i] = rand() % 100;
            }

            MPI_Send(numbers, number_amout, MPI_INT, p, 0, MPI_COMM_WORLD);
            printf("0 sent %d numbers to %d\n", number_amout, p);
            localsum += number_amount;

            for(int p = 1; p < world_size; p++){
                MPI_Recv( &sumproc, 1, MPI_INT, p, 1, MPI_COMM_WORLD, status);
                sum += sumproc;
            }
            printf("The total sum was %d and the local sum was %d\n", sum, localsum);
        }
    } else {
        int sum = 0;

        // Receive at most the max from process zero (master)
        MPI_Recv(numbers, MAX_NUMBERS, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        // Check the status
        MPI_Get_count( &status , MPI_INT , &number_amout);

        // Print so we can visualize the process 
        printf("%d received %d numbers from 0. Message source = %d, tag = %d\n",
           world_rank, number_amount, status.MPI_SOURCE, status.MPI_TAG);
        
        for(int i = 0; i < number_amout; i++){
            soma += numbers[i];
        }
        printf("%d added %d\n", world_rank, sum);
        MPI_Send(&sum, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
}
    