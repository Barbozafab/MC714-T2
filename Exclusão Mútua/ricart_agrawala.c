#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h> // Para a função sleep

#define MAX_TIMESTAMP 1000

int resource = 0; // recurso - controla quantas vezes a SC foi acessada. Deve ser igual ao tamanho

// Função para verificar se vetor contém pelo menos um valor false
bool isFalse(bool *v, int size) {
    for (int i = 0; i < size; ++i) {
        if (!v[i]) {
            return true;
        }
    }
    return false;
}

// Função para gerar timestamps aleatórios para cada processo
void generate_timestamps(int *timestamp, int size) {
    srand(time(NULL));
    for (int i = 0; i < size; ++i) {
        timestamp[i] = rand() % MAX_TIMESTAMP; // Gerando valores aleatórios entre 0 e 999 para timestamps
    }
}

// Função para executar o algoritmo Ricart-Agrawala
void ricart_agrawala(int size, int rank, int *timestamp, bool *permissions) {
    MPI_Status status;
    permissions[rank] = true; // Define sua própria permissão como verdadeira

    printf("Processo %d está requisitando a SC agora.\n", rank);
    for (int i = 0; i < size; i++) {
        if (i != rank) {
            MPI_Send(&timestamp[rank], 1, MPI_INT, i, 0, MPI_COMM_WORLD); // envia seu próprio timestamp para o processo com rank = i
        }
    }
    printf("Processo %d transmitiu uma solicitação com o timestamp %d\n", rank, timestamp[rank]);

    MPI_Barrier(MPI_COMM_WORLD); // barreira - mecanismo essencial aqui - prosseguimos apenas quando todas as solicitações foram transmitidas

    int received_timestamp;
    for (int i = 0; i < size; i++) {
        if (i != rank) {
            MPI_Recv(&received_timestamp, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            printf("Processo %d recebeu uma solicitação com timestamp %d do processo %d\n", rank, received_timestamp, i);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD); // prosseguimos apenas quando todas as solicitações foram recebidas

    bool *permissions_received = (bool *)malloc(size * sizeof(bool));
    for (int i = 0; i < size; i++) {
        permissions_received[i] = false;
    }

    for (int i = 0; i < size; i++) {
        if (i != rank) {
            MPI_Send(&permissions[rank], 1, MPI_C_BOOL, i, 0, MPI_COMM_WORLD); // envia a permissão para todos os outros processos
        }
    }

    printf("Processo %d está na SC.\n", rank);
    resource++;
    sleep(1); // simula algum trabalho
    printf("Processo %d saiu da SC. O recurso foi incrementado.\n", rank);

    free(permissions_received);
}

int main(int argc, char **argv) {
    printf("Valor do recurso no início: %d\n", resource);
    MPI_Init(NULL, NULL);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int *timestamp = (int *)malloc(size * sizeof(int));
    bool *permissions = (bool *)malloc(size * sizeof(bool));

    generate_timestamps(timestamp, size); // Gera timestamps aleatórios para cada processo

    ricart_agrawala(size, rank, timestamp, permissions);

    int total_resource = 0;
    MPI_Allreduce(&resource, &total_resource, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    printf("Valor do recurso após a execução: %d\n", total_resource);

    free(timestamp);
    free(permissions);

    MPI_Finalize();

    return 0;
}
