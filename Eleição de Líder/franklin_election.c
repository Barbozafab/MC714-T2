#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

typedef enum {ACTIVE, PASSIVE} Status;

typedef struct {
    int rank;
    int id;
    Status status;
} Process;

// Função para gerar um ID aleatório para todos os processos
int generateRandomNumber(int rank, int size) {
    int *arrayOfNumbers = malloc(size * sizeof(int));

    for (int i = 0; i < size; i++)
        arrayOfNumbers[i] = rand();

    int number = arrayOfNumbers[rank];
    free(arrayOfNumbers);

    return number;
}

// Função para criar um novo processo
Process createProcess(int rank, int nprocs) {
    Process p;
    p.rank = rank;
    p.id = generateRandomNumber(rank, nprocs);
    p.status = ACTIVE;
    return p;
}

int main(int argc, char** argv) {
    // Inicialização
    int my_rank, nprocs, my_id, left_id, right_id;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    Process p = createProcess(my_rank, nprocs);

    printf("I am process %d of %d. My ID is = %d.\n", p.rank, nprocs, p.id);

    my_rank = p.rank;
    my_id = p.id;

    while (1) { // Repetir até que só reste um processo ativo
        if (p.status == ACTIVE) { // Para os processos ativos
            if (my_rank == 0) { // Corner case - 1º processo da lista
                MPI_Send(&my_id, 1, MPI_INT, nprocs - 1, 0, MPI_COMM_WORLD);
                MPI_Send(&my_id, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);

                MPI_Recv(&left_id, 1, MPI_INT, nprocs - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&right_id, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else if (my_rank == nprocs - 1) { // Cornenr case - último processo da lista
                MPI_Send(&my_id, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD);
                MPI_Send(&my_id, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

                MPI_Recv(&left_id, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&right_id, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else { // Todos os outros processos
                MPI_Send(&my_id, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD);
                MPI_Send(&my_id, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);

                MPI_Recv(&left_id, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&right_id, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }

            if (my_id < left_id && my_id < right_id) { // O processo é o menor de seus vizinhos
                p.status = PASSIVE;
            } else if (left_id == my_id && right_id == my_id) { // Só restou um processo ativo
                break;
            }
        } else { // Para os processos passivos
            if (my_rank == 0) { // Corner case - 1º processo da lista
                MPI_Recv(&left_id, 1, MPI_INT, nprocs - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(&left_id, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);

                MPI_Recv(&right_id, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(&right_id, 1, MPI_INT, nprocs - 1, 0, MPI_COMM_WORLD);
            } else if (my_rank == nprocs - 1) { // Corner case - último processo da lista
                MPI_Recv(&left_id, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(&left_id, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

                MPI_Recv(&right_id, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(&right_id, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD);
            } else { // Todos os outros processos
                MPI_Recv(&left_id, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(&left_id, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);

                MPI_Recv(&right_id, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(&right_id, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD);
            }
        }
    }

    // Apenas o nó com maior ID vai chegar aqui
    printf("New leader is process %d of %d. New leader's ID is %d.\n", p.rank, nprocs, p.id);
    MPI_Abort(MPI_COMM_WORLD, MPI_SUCCESS);
    MPI_Finalize();

    return 0;
}
