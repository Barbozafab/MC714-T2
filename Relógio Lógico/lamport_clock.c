#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MAX_EVENTS 100

int lamport_clock = 0;
int local_events[MAX_EVENTS];
int logical_clocks[MAX_EVENTS];

void send_message(int rank, int dest, MPI_Comm comm) {
    MPI_Status status;
    int msg = lamport_clock;

    MPI_Send(&msg, 1, MPI_INT, dest, 0, comm);
    local_events[lamport_clock] = 1;
    lamport_clock++;

    printf("Process %d: SEND to %d, Clock: %d\n", rank, dest, lamport_clock);
}

void receive_message(int rank, int source, MPI_Comm comm) {
    MPI_Status status;
    int msg, max_clock;

    MPI_Recv(&msg, 1, MPI_INT, source, 0, comm, &status);
    local_events[lamport_clock] = 1;
    lamport_clock++;

    max_clock = (msg > lamport_clock) ? msg + 1 : lamport_clock;
    lamport_clock = max_clock;

    printf("Process %d: RECV from %d, Clock: %d\n", rank, source, lamport_clock);
}

void internal_event(int rank) {
    local_events[lamport_clock] = 1;
    lamport_clock++;

    printf("Process %d: INTERNAL event, Clock: %d\n", rank, lamport_clock);
}

int main(int argc, char **argv) {
    int size, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Simulação de eventos
    if (rank == 0) {
        // Exemplo de sequência de eventos
        internal_event(rank);
        send_message(rank, 1, MPI_COMM_WORLD);
        receive_message(rank, 3, MPI_COMM_WORLD);
        internal_event(rank);
        send_message(rank, 2, MPI_COMM_WORLD);
    } else if (rank == 1) {
        // Processo 1 executa eventos internos e recebe mensagens
        internal_event(rank);
        receive_message(rank, 0, MPI_COMM_WORLD);
        internal_event(rank);
        send_message(rank, 2, MPI_COMM_WORLD);
    } else if (rank == 2) {
        // Processo 2 executa eventos internos e recebe mensagens
        internal_event(rank);
        receive_message(rank, 1, MPI_COMM_WORLD);
        internal_event(rank);
        send_message(rank, 3, MPI_COMM_WORLD);
    } else if (rank == 3) {
        // Processo 3 executa eventos internos e recebe mensagens
        internal_event(rank);
        receive_message(rank, 2, MPI_COMM_WORLD);
        internal_event(rank);
        send_message(rank, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
