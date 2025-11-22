#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <math.h>
#include <numbers>
#include <vector>

#include <mpi.h>


double squareX(double x)
{
    //return pow(x, 2);
    //return sin(x);
    return cos(x);
}
double calculateMidpoint(double a, double b, int intervals)
{
    if (intervals <= 0) return 0.0;

    double step = (b - a) / intervals;
    double sum = 0.0;

    for (int i = 0; i < intervals; i++)
    {
        double x_mid = a + (i + 0.5) * step;
        sum += squareX(x_mid) * step;
    }

    return sum;
}

int main(int argc, char* argv[]) 
{
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const double a = 0.0;
    const double b = std::numbers::pi;
    const int total_intervals = 100000000000;


    if (rank == 0)
    {
        std::cout << "The method of medium triangles\n";
        std::cout << "Function: x^2\n";
        std::cout << "Limits of integration [" << a << ", " << b << "]" << std::endl;
        std::cout << "Number of intervals: " << total_intervals << std::endl;
        std::cout << "Number of processes: " << size << std::endl;
    }
    

    std::vector<double> intervals_data(2 * size);
    double interval_length = (b - a) / size;

    if (rank == 0)
    {
        for (int i = 0; i < size; i++)
        {
            intervals_data[2 * i] = a + i * interval_length; // a
            intervals_data[2 * i + 1] = a + (i + 1) * interval_length; // b
        }
    }

    double local_interval[2];

    MPI_Scatter(intervals_data.data(), 2, MPI_DOUBLE,
                local_interval, 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double local_a = local_interval[0];
    double local_b = local_interval[1];

    int local_intervals = total_intervals / size;
    double local_result = calculateMidpoint(local_a, local_b, local_intervals);

    std::cout << "Process " << rank << ": section [" << local_a << ", " << local_b << "], intervals: " << local_intervals
        << ", result: " << local_result << std::endl;

    std::vector<double> all_results(size);
    MPI_Gather(&local_result, 1, MPI_DOUBLE, all_results.data(), 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        double total_result = 0.0;
        for (int i = 0; i < size; i++)
        {
            std::cout << "Process " << i << ": " << all_results[i] << std::endl;
            total_result += all_results[i];
        }
    }
    MPI_Finalize();
    return 0;
}