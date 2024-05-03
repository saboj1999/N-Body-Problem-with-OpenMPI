#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

/*

Name: John Sabo
Id: 916454209
Assignment: Final Project

To Compile:
gcc n_body.c -o n_body.o

To Run:
./n_body.o <number of bodies> <number of steps>

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
 * Purpose: Return current time in unit of seconds
 * Parameters: none
 * Returns: a double of the current time in seconds
 */
double gettime(void) {
  struct timeval tval;

  gettimeofday(&tval, NULL);

  return( (double)tval.tv_sec + (double)tval.tv_usec/1000000.0 );
}

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
 * Purpose: Compute each body's new velocity and position
 * Parameters: 
 * bodies -> array of bodies
 * n -> number of bodies
 * Returns: none
 */
void computeForcesAndMove(Body *bodies, int n) {
    for (int i = 0; i < n; i++) {
        double fx = 0.0, fy = 0.0, fz = 0.0;
        for (int j = 0; j < n; j++) {
            if (i != j) {
                double dx = bodies[j].x - bodies[i].x;
                double dy = bodies[j].y - bodies[i].y;
                double dz = bodies[j].z - bodies[i].z;
                double dist = sqrt(dx*dx + dy*dy + dz*dz) + 1e-10; // Avoid division by zero
                double F = G * bodies[i].mass * bodies[j].mass / (dist * dist);
                fx += F * dx / dist;
                fy += F * dy / dist;
                fz += F * dz / dist;
            }
        }
        // Update velocities
        bodies[i].vx += fx / bodies[i].mass * dt;
        bodies[i].vy += fy / bodies[i].mass * dt;
        bodies[i].vz += fz / bodies[i].mass * dt;

        // Update positions
        bodies[i].x += bodies[i].vx * dt;
        bodies[i].y += bodies[i].vy * dt;
        bodies[i].z += bodies[i].vz * dt;
    }
}

int main(int argc, char **argv) {

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <number of bodies> <number of steps>\n", argv[0]);
        return 1;
    }

    int numBodies = atoi(argv[1]);
    int numSteps = atoi(argv[2]);
    srand(time(NULL));

    // Allocate memory for Bodies
    Body *bodies = malloc(numBodies * sizeof(Body));
    if (bodies == NULL) {
        fprintf(stderr, "Unable to allocate memory for bodies.\n");
    }

    // Initialize Bodies
    initializeBodies(bodies, numBodies);
    
    double starttime = gettime();

    // Main simulation loop
    for (int step = 0; step < numSteps; step++) {
        computeForcesAndMove(bodies, numBodies);
    }
    
    double endtime = gettime();
    printf("%lf\n", endtime - starttime);

    // Free memory
    free(bodies);
   
    return 0;
}
