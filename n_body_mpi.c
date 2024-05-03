#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

/*

Name: John Sabo
Id: 916454209
Assignment: Final Project

To Compile:
mpicc n_body_mpi.c -o n_body_mpi.o

To Run:
mpiexec -n 2 ./n_body_mpi.o <number of bodies> <number of steps>

For process count higher than 2 on my machine (my cpu has 2 cores):
mpiexec --oversubscribe -n 4 ./n_body_mpi.o <number of bodies> <number of steps>

*/

/*
 * Body Struct
 */
typedef struct {
    double x, y, z;
    double vx, vy, vz;
    double mass; 
} Body;

const double G = 6.67430e-11; // Gravitational constant
const double dt = 0.01; // Time step

/*
 * Purpose: Initialize all body structs with random values
 * Parameters: 
 * bodies -> array of bodies
 * n -> number of bodies
 * Returns: none
 */
void initializeBodies(Body *bodies, int n) {
    for (int i = 0; i < n; i++) {
        bodies[i].x = (double)rand() / RAND_MAX * 1000.0 - 500.0;
        bodies[i].y = (double)rand() / RAND_MAX * 1000.0 - 500.0;
        bodies[i].z = (double)rand() / RAND_MAX * 1000.0 - 500.0;
        bodies[i].vx = (double)rand() / RAND_MAX * 10.0 - 5.0;
        bodies[i].vy = (double)rand() / RAND_MAX * 10.0 - 5.0;
        bodies[i].vz = (double)rand() / RAND_MAX * 10.0 - 5.0;
        bodies[i].mass = (double)rand() / RAND_MAX * 1000.0 + 100.0; // Avoid zero mass
    }
}

/*
 * Purpose: Compute each local body's new positions and velocities
 * Parameters: 
 * localBodies -> array of this process's local bodies
 * allBodies -> array of all bodies
 * local_n -> process's number of local bodies
 * total_n -> total number of bodies
 * Returns: none
 */
void computeForcesAndMove(Body *localBodies, Body *allBodies, int local_n, int total_n) {
    for (int i = 0; i < local_n; i++) {
        double fx = 0.0, fy = 0.0, fz = 0.0;
        for (int j = 0; j < total_n; j++) {
            if (allBodies[j].x != localBodies[i].x || allBodies[j].y != localBodies[i].y || allBodies[j].z != localBodies[i].z) {
                double dx = allBodies[j].x - localBodies[i].x;
                double dy = allBodies[j].y - localBodies[i].y;
                double dz = allBodies[j].z - localBodies[i].z;
                double dist = sqrt(dx*dx + dy*dy + dz*dz) + 1e-10; // Avoid division by zero
                double F = G * localBodies[i].mass * allBodies[j].mass / (dist * dist);
                fx += F * dx / dist;
                fy += F * dy / dist;
                fz += F * dz / dist;
            }
        }
        // Update velocities
        localBodies[i].vx += fx / localBodies[i].mass * dt;
        localBodies[i].vy += fy / localBodies[i].mass * dt;
        localBodies[i].vz += fz / localBodies[i].mass * dt;

        // Update positions
        localBodies[i].x += localBodies[i].vx * dt;
        localBodies[i].y += localBodies[i].vy * dt;
        localBodies[i].z += localBodies[i].vz * dt;
    }
}

int main(int argc, char **argv) {
    // Initialize MPI
    MPI_Init(&argc, &argv);
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (argc != 3) {
        if (world_rank == 0) {
            fprintf(stderr, "Usage: %s <number of bodies> <number of steps>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    int numBodies = atoi(argv[1]);
    int numSteps = atoi(argv[2]);
    srand(time(NULL) + world_rank * 123);

    // Calculate send counts and displacements
    int *recvcounts = malloc(world_size * sizeof(int));
    int *displs = malloc(world_size * sizeof(int));
    int sum = 0;
    for (int i = 0; i < world_size; i++) {
        recvcounts[i] = (numBodies / world_size + (i < numBodies % world_size)) * sizeof(Body);
        displs[i] = sum;
        sum += recvcounts[i];
    }

    // Allocate memory for allBodies and localBodies
    int local_n = numBodies / world_size + (world_rank < numBodies % world_size);
    Body *localBodies = malloc(local_n * sizeof(Body));
    Body *allBodies = malloc(numBodies * sizeof(Body));
    if (localBodies == NULL || allBodies == NULL) {
        fprintf(stderr, "Unable to allocate memory for bodies.\n");
        MPI_Abort(MPI_COMM_WORLD, 2);
    }

    // Initialization
    if (world_rank == 0) {
        initializeBodies(allBodies, numBodies);
    }

    double starttime = MPI_Wtime();  

    // Broadcast allBodies to all processes
    MPI_Bcast(allBodies, numBodies * sizeof(Body), MPI_BYTE, 0, MPI_COMM_WORLD);

    // Scatter the bodies to local arrays
    MPI_Scatterv(allBodies, recvcounts, displs, MPI_BYTE, localBodies, local_n * sizeof(Body), MPI_BYTE, 0, MPI_COMM_WORLD);

    // Simulation Loop
    for (int step = 0; step < numSteps; step++) {
        computeForcesAndMove(localBodies, allBodies, local_n, numBodies);
        // Gather all processes' updated localBodies into all processes' allBodies
        MPI_Allgatherv(localBodies, local_n * sizeof(Body), MPI_BYTE, allBodies, recvcounts, displs, MPI_BYTE, MPI_COMM_WORLD);
    }

    double endtime = MPI_Wtime();  

    if(world_rank == 0)
    {
        printf("%lf\n", endtime - starttime);
    }

    // Free memory
    free(localBodies);
    free(allBodies);
    free(recvcounts);
    free(displs);
    MPI_Finalize();
    return 0;
}
