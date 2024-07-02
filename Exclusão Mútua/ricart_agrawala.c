#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>  // para usleep

#define MAX_TIMESTAMP 100

int resource = 0;

bool isFalse(bool permissions[], int size) {
    for (int i = 0; i < size; ++i) {
        if (!permissions[i]) {
            return true;
        }
    }
    return false;
}

void ricart_agrawala(int size, int rank, int timestamps[], bool permissions[]) {
    MPI_Status status;
    permissions[rank] = true;

    printf("Processo %d está solicitando a SE agora.\n", rank);
    for (int i = 0; i < size; ++i) {
        if (i != rank) {
            MPI_Send(&timestamps[rank], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
    printf("Processo %d transmitiu uma solicitação com timestamp %d\n", rank, timestamps[rank]);

    MPI_Barrier(MPI_COMM_WORLD);

    int received_timestamp;
    int received_count = 0;
    int received[size - 1][2];

    for (int i = 0; i < size; ++i) {
        if (i != rank) {
            MPI_Recv(&received_timestamp, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            printf("Processo %d recebeu uma solicitação com timestamp %d do processo %d\n", rank, received_timestamp, i);
            received[received_count][0] = received_timestamp;
            received[received_count][1] = i;
            received_count++;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    int message = 1;
    int flag;

    for (int i = 0; i < received_count; ++i) {
        if (timestamps[rank] > received[i][0] || (timestamps[rank] == received[i][0] && rank > received[i][1])) {
            MPI_Send(&message, 1, MPI_INT, received[i][1], 0, MPI_COMM_WORLD);
            printf("Processo %d enviando permissão para o processo %d\n", rank, received[i][1]);
        }
    }

    int received_message;
    while (isFalse(permissions, size)) {
        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
        if (flag) {
            MPI_Recv(&received_message, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            permissions[status.MPI_SOURCE] = true;
            printf("Processo %d recebeu permissão do processo %d\n", rank, status.MPI_SOURCE);
        }
    }

    printf("Processo %d está na SC.\n", rank);
    resource++;
    usleep(1500000); // simula algum trabalho
    printf("Processo %d saiu da SC. Recurso foi incrementado.\n", rank);

    for (int i = 0; i < received_count; ++i) {
        if (timestamps[rank] < received[i][0] || (timestamps[rank] == received[i][0] && rank < received[i][1])) {
            MPI_Send(&message, 1, MPI_INT, received[i][1], 0, MPI_COMM_WORLD);
        }
    }
}

int main(int argc, char** argv) {
    srand(time(NULL));

    MPI_Init(NULL, NULL);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int timestamps[size];
    bool permissions[size];

    for (int i = 0; i < size; ++i) {
        timestamps[i] = rand() % MAX_TIMESTAMP; // gera timestamps aleatórios entre 0 e MAX_TIMESTAMP-1
        permissions[i] = false;
    }

    ricart_agrawala(size, rank, timestamps, permissions);

    int total_resource = 0;
    MPI_Allreduce(&resource, &total_resource, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    printf("Valor do recurso após a execução: %d\n", total_resource);

    MPI_Finalize();

    return 0;
}
