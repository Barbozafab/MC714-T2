#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SEND 1
#define RECV 2
#define INTERNAL 3

int main(int argc, char** argv) {
    int rank, size, clock = 0, event, source, dest;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    for (int i = 0; i < 10; i++) {
        event = rand() % 3 + 1;
        switch (event) {
            case SEND:
                dest = rand() % size;
                if (dest != rank) {
                    clock++;
                    printf("Process %d: SEND to %d, Clock: %d\n", rank, dest, clock);
                    MPI_Send(&clock, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
                }
                break;
            case RECV:
                source = rand() % size;
                if (source != rank) {
                    int recv_clock;
                    MPI_Recv(&recv_clock, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
                    clock = (clock > recv_clock ? clock : recv_clock) + 1;
                    printf("Process %d: RECV from %d, Clock: %d\n", rank, source, clock);
                }
                break;
            case INTERNAL:
                clock++;
                printf("Process %d: INTERNAL event, Clock: %d\n", rank, clock);
                break;
        }
        usleep(500000);
    }

    MPI_Finalize();
    return 0;
}
