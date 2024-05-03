# N-Body-Problem-with-OpenMPI

1. Run Tests/test to verify identical output between sequential and parallel versions.
   - cd ./Tests/
   - ./test
2. Compile both scripts.
   - cd ..
   - mpicc n_body_mpi.c -o n_body_mpi.o
   - gcc n_body.c -o n_body.o
3. Run the performance test.
   - ./runTimePerformance
   - This should produce a file called mpi_average_outputs.txt
4. Run graphTimePerformance.py
   - python3 graphTimePerformance.py
5. To run the scripts with custom values:
   - Sequential: ./n_body <number_of_bodies> <number_of_steps>
   - Parallel: mpiexec -n <number_of_processes> ./n_body <number_of_bodies> <number_of_steps>

Note: I modified the mpi_average_outputs.txt file so I could lazily update my python code. In order to create the graphs without error, ensure that the mpi_average_outputs.txt file follows the same pattern as the following example:

Average Output for 1 './n_body.o 256 10000 ': 16.9480604000
Average Output for 'mpiexec --oversubscribe -n 2 ./n_body_mpi.o 256 10000 ': 13.3013020000
Average Output for 'mpiexec --oversubscribe -n 4 ./n_body_mpi.o 256 10000 ': 13.9396404000
Average Output for 'mpiexec --oversubscribe -n 8 ./n_body_mpi.o 256 10000 ': 15.0091506000
Average Output for 'mpiexec --oversubscribe -n 16 ./n_body_mpi.o 256 10000 ': 18.6961522000
