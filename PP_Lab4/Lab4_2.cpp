#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <math.h>
#include <numbers>
#include <vector>

#include <mpi.h>

int main(int argc, char* argv[])
{

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double* matrix = NULL;
    double* vector = NULL;

    if (rank == 0)
    {
        matrix = (double*)malloc(16 * sizeof(double));
        vector = (double*)malloc(4 * sizeof(double));

        for (int i = 0; i < 16; i++) matrix[i] = i + 1;
        for (int i = 0; i < 4; i++) vector[i] = 1.0;
    }
    double local_vector[4];
    MPI_Bcast(vector, 4, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    if (rank != 0)
    {
        vector = (double*)malloc(4 * sizeof(double));
    }

    MPI_Finalize();
    return 0;
}