/*  Franklin's Algorithm For Undirected Rings
 *  Tasmiyah Iqbal
 */

#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

/*
 * The method aspires to generate a unique random number to be used as process id for each process.
 * P.S. I could have used MPI generated ranks as PIDs but then my logical topology would have been sequential
 * and hence also very predictable.  
 */
int generateRandomNumber(int rank, int size) {
		int s;
		int *rNums;
		rNums = malloc(size * sizeof(int));

		for(int i=0; i<size; i++)
		{
			rNums[i] = rand(); // aspires to be random number
		}

		s = rNums[rank]; 
		free(rNums);
		return s;
}

/*
 * The class representing a process in the system. Holds all necessary process info.
 */
typedef struct {
	int rank; // MPI generated
	int identifier; // assigned by this code
	bool status; // active or passive
} Process;

Process Process_create(int r, int nprocs) {
	Process p;
	p.rank = r;
	p.identifier = generateRandomNumber(r, nprocs);
	p.status = true;
	return p;
}
		
int Process_getRank(Process p) {
	return p.rank;
}

void Process_setRank(Process *p, int r) {
	p->rank = r;
}

int Process_getIdentifier(Process p) {
	return p.identifier;
}

void Process_setIdentifier(Process *p, int id) {
	p->identifier = id;
}

bool Process_getStatus(Process p) {
	return p.status;
}

void Process_setStatus(Process *p, bool s) {
	p->status = s;
}

int main(int argc, char** argv) {
    int myrank, nprocs;
	int my_id, left_id, right_id;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	Process p = Process_create(myrank, nprocs);
	printf("I am process %d of %d. Unique ID = %d\n", Process_getRank(p), nprocs, Process_getIdentifier(p));
	my_id = Process_getIdentifier(p);
	myrank = Process_getRank(p);
    
	while(1) // loop to continue until a leader is elected
	{
		if (Process_getStatus(p) == true) { // If an active node			
			if (myrank == 0) // Special boundary handling for first process in ring
			{
				MPI_Send(&my_id, 1, MPI_INT, myrank+1, 0, MPI_COMM_WORLD); // send my id to next process in ring
				MPI_Recv(&right_id, 1, MPI_INT, nprocs-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // recv from last process in ring

				MPI_Send(&my_id, 1, MPI_INT, nprocs-1, 0, MPI_COMM_WORLD); // send my id to previous neighbour in ring, I'm p0 so my prev is nprocs-1
				MPI_Recv(&left_id, 1, MPI_INT, myrank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // recv from next process in ring
			}
			else if (myrank == nprocs-1) // Special boundary handling for last process in ring
			{
				MPI_Send(&my_id, 1, MPI_INT, 0, 0, MPI_COMM_WORLD); // send to next process in ring, since I am last process therefore my next is p0
				MPI_Recv(&right_id, 1, MPI_INT, myrank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // recv from previous neighbour in ring

				MPI_Send(&my_id, 1, MPI_INT, myrank-1, 0, MPI_COMM_WORLD); // send to previous neighbour in ring
				MPI_Recv(&left_id, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // recv from next process in ring, since I am last process therefore my next is p0
			}
			else // Rest of the nodes (e.g p1,p2..)
			{
				MPI_Send(&my_id, 1, MPI_INT, myrank+1, 0, MPI_COMM_WORLD); // send to next process in ring
				MPI_Recv(&right_id, 1, MPI_INT, myrank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // recv from previous neighbour in ring

				MPI_Send(&my_id, 1, MPI_INT, myrank-1, 0, MPI_COMM_WORLD); // send to previous neighbour in ring
				MPI_Recv(&left_id, 1, MPI_INT, myrank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // recv from previous neighbour in ring
			}

			if (left_id > right_id) { // compare IDs to see if I am the smallest process in my *active* neighbourhood
				if (left_id > my_id) { // Check if left_id is actually smaller than my_id
					Process_setStatus(&p, false); // enter relay mode if my ID is the smallest of the three
				}
			}
			else if (left_id == my_id) // If I am my own left and right neighbour i.e. all others have become passive by now then:
			{
				break; // break out the loop and declare yourself as leader
			}
		}
		
		// If I'm a passive node I will only pass msgs forward and backward in the ring. 
		// Also I will only be triggered by some active node (from the code above) hence I will be waiting for an incoming msg. 
		// Note the order of send() and recv() methods specifically to avoid deadlocked code!
		else if (Process_getStatus(p) == false) { 	
			if (myrank == 0) // Special boundary handling for first process in ring
			{
				MPI_Recv(&right_id, 1, MPI_INT, nprocs-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				MPI_Send(&right_id, 1, MPI_INT, myrank+1, 0, MPI_COMM_WORLD);

				MPI_Recv(&left_id, 1, MPI_INT, myrank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				MPI_Send(&left_id, 1, MPI_INT, nprocs-1, 0, MPI_COMM_WORLD);
			}
			else if (myrank == nprocs-1) // Special boundary handling for last process in ring
			{
				MPI_Recv(&right_id, 1, MPI_INT, myrank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				MPI_Send(&right_id, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

				MPI_Recv(&left_id, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				MPI_Send(&left_id, 1, MPI_INT, myrank-1, 0, MPI_COMM_WORLD);
			}
			else // Rest of the nodes (e.g p1,p2..)
			{
				MPI_Recv(&right_id, 1, MPI_INT, myrank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // recv from previous neighbour
				MPI_Send(&right_id, 1, MPI_INT, myrank+1, 0, MPI_COMM_WORLD); // pass on to next neighbour (relay behaviour)

				MPI_Recv(&left_id, 1, MPI_INT, myrank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // recv from next neighbour
				MPI_Send(&left_id, 1, MPI_INT, myrank-1, 0, MPI_COMM_WORLD); // pass to previous neighbour
			}
		}
	}

    // If any process reaches this area in the code, it has discovered that it is its own left most and right most active neighbour.
	// Broadcast message to all threads to terminate themselves. Declare self as leader.
	printf("Process %d of %d is LEADER with highest Unique ID = %d\n", myrank, nprocs, my_id);
	MPI_Abort(MPI_COMM_WORLD, MPI_SUCCESS); // kill all other processes.
	MPI_Finalize(); // prepare to exit
    return 0;
}