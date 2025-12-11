// mpi_block_distribution_oop.cpp
// C++20 OOP version
#include <mpi.h>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <chrono>

using Matrix = std::vector<double>;

class MPIBlockDistributor {
public:
    MPIBlockDistributor(int N, int M, int prows, int pcols)
        : N(N), M(M), prows(prows), pcols(pcols)
    {
        init_cartesian();
        brow = N / prows;
        bcol = M / pcols;
        localA.resize(brow * bcol);
        localv.resize(brow);
        y_local.resize(brow);
    }

    ~MPIBlockDistributor() {
        MPI_Comm_free(&col_comm);
        MPI_Comm_free(&cart_comm);
    }

    void init_data() {
        if (rank == 0) {
            A.resize(N * M);
            v.resize(N);
            for (int i = 0;i < N;i++) {
                v[i] = 1000 + i;
                for (int j = 0;j < M;j++) A[i * M + j] = i * 100 + j;
            }
        }
    }

    void distribute_matrix() {
        std::vector<double> sendbuf;
        if (rank == 0) {
            sendbuf.resize(prows * pcols * brow * bcol);
            int idx = 0;
            for (int pr = 0; pr < prows; ++pr)
                for (int pc = 0; pc < pcols; ++pc)
                    for (int i = 0;i < brow;i++)
                        for (int j = 0;j < bcol;j++)
                            sendbuf[idx++] = A[(pr * brow + i) * M + (pc * bcol + j)];
        }

        MPI_Scatter(sendbuf.data(), brow * bcol, MPI_DOUBLE,
            localA.data(), brow * bcol, MPI_DOUBLE,
            0, MPI_COMM_WORLD);
    }

    void distribute_vector() {
        int col_rank, col_size;
        MPI_Comm_rank(col_comm, &col_rank);
        MPI_Comm_size(col_comm, &col_size);

        Matrix col_send;
        int col_root_world;
        int root_coords[2] = { 0, pcol };
        MPI_Cart_rank(cart_comm, root_coords, &col_root_world);

        if (col_rank == 0) col_send.resize(col_size * brow);

        if (rank == 0) {
            for (int pc = 0; pc < pcols; ++pc) {
                int rc[2] = { 0,pc };
                int dst; MPI_Cart_rank(cart_comm, rc, &dst);

                Matrix tmp(col_size * brow);
                for (int pr = 0; pr < prows; ++pr)
                    for (int t = 0;t < brow;t++)
                        tmp[pr * brow + t] = v[pr * brow + t];

                if (dst == 0)
                    col_send = tmp;
                else
                    MPI_Send(tmp.data(), tmp.size(), MPI_DOUBLE, dst, 200 + pc, MPI_COMM_WORLD);
            }
        }
        else if (col_rank == 0) {
            MPI_Recv(col_send.data(), col_send.size(), MPI_DOUBLE,
                0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        MPI_Datatype chunk_t;
        MPI_Type_contiguous(brow, MPI_DOUBLE, &chunk_t);
        MPI_Type_commit(&chunk_t);

        MPI_Scatter(col_send.data(), 1, chunk_t,
            localv.data(), brow, MPI_DOUBLE,
            0, col_comm);

        MPI_Type_free(&chunk_t);
    }

    void compute_local() {
        for (int i = 0;i < brow;i++) {
            double s = 0;
            for (int j = 0;j < bcol;j++) s += localA[i * bcol + j] * localv[i];
            y_local[i] = s;
        }
    }

    void gather_results() {
        if (rank == 0) {
            Matrix y(N);
            int off0 = prow * brow;
            for (int i = 0;i < brow;i++) y[off0 + i] = y_local[i];

            for (int r = 1;r < size;r++) {
                int rc[2]; MPI_Cart_coords(cart_comm, r, 2, rc);
                int rprow = rc[0];
                int off = rprow * brow;

                std::vector<double> tmp(brow);
                MPI_Recv(tmp.data(), brow, MPI_DOUBLE, r, 999,
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                for (int i = 0;i < brow;i++) y[off + i] = tmp[i];
            }

            std::cout << "Result y: ";
                for (int i = 0;i < N;i++)
                    std::cout << "y[" << i << "]=" << y[i] << " ";

        }
        else {
            MPI_Send(y_local.data(), brow, MPI_DOUBLE, 0, 999, MPI_COMM_WORLD);
        }
    }

private:
    int N, M, prows, pcols;
    int brow, bcol;

    MPI_Comm cart_comm, col_comm;
    int rank, size;
    int prow, pcol;

    Matrix A, v;
    Matrix localA, localv, y_local;

    void init_cartesian() {
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);

        int dims[2] = { prows,pcols }, periods[2] = { 0,0 };
        MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &cart_comm);

        int coords[2];
        MPI_Cart_coords(cart_comm, rank, 2, coords);
        prow = coords[0];
        pcol = coords[1];

        MPI_Comm_split(cart_comm, pcol, rank, &col_comm);
    }
};

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);


    // Синхронизируем процессы и засекаем стартовое время сразу после инициализации MPI
    MPI_Barrier(MPI_COMM_WORLD);
    auto start_time = std::chrono::high_resolution_clock::now();


    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);


    if (argc < 5) {
        if (rank == 0)
            std::cerr << "Usage: N M prows pcols\n";
        MPI_Finalize();
        return 0;
    }


    int N = std::atoi(argv[1]);
    int M = std::atoi(argv[2]);
    int prows = std::atoi(argv[3]);
    int pcols = std::atoi(argv[4]);


    MPIBlockDistributor dist(N, M, prows, pcols);
    dist.init_data();
    dist.distribute_matrix();
    dist.distribute_vector();
    dist.compute_local();
    dist.gather_results();


    // Синхронизируемся перед измерением конечного времени
    MPI_Barrier(MPI_COMM_WORLD);
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    if (rank == 0) std::cout <<"\nTotal execution time(chrono) : " << elapsed.count() << " sec\n";


    MPI_Finalize();
    return 0;
}
