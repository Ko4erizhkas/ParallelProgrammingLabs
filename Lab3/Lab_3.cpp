#include "DelegateThreads.h"
double simpson_in_threads(float a, float b, float n, int num_threads)
{
	double h = (b - a) / n;
	double total_sum = sin(a) + sin(b);

	std::vector<std::thread> threads;
	std::vector<double> thread_sums(num_threads, 0.0);

	auto worker = [&](int thread_id, int start, int end) {
		double sum = 0.0;

		for (int i = start; i < end; i++) {
			double x = a + i * h;
			double coefficient = (i % 2 == 0) ? 2.0 : 4.0;
			sum += coefficient * sin(x);
		}

		thread_sums[thread_id] = sum;
		};

	int iterations_per_thread = n / num_threads;


	for (int i = 0; i < num_threads; i++) {
		int start = i * iterations_per_thread + 1;
		int end = (i == num_threads - 1) ? n : (i + 1) * iterations_per_thread;
		threads.emplace_back(worker, i, start, end);
	}


	for (auto& thread : threads) {
		thread.join();
	}

	for (double sum : thread_sums) {
		total_sum += sum;
	}

	return total_sum * h / 3.0;
}
double simpson_in_threads_with_OpenMP(float a, float b, float n, int num_thread)
{
	double h = (b - a) / n;
	double total_sum = sin(a) + sin(b);

	std::vector<std::thread> threads;
	std::vector<double> thread_sums(num_thread, 0.0);

#pragma omp parallel num_threads(num_thread)
	{
		int thread_id = omp_get_thread_num();
		int num_threads_actual = omp_get_num_threads();

		int iterations_per_thread = n / num_threads_actual;
		int start = thread_id * iterations_per_thread + 1;
		int end = (thread_id == num_threads_actual - 1) ?
			static_cast<int>(n) :
			(thread_id + 1) * iterations_per_thread;

		double sum = 0.0;
		for (int i = start; i < end; i++) {
			double x = a + i * h;
			double coefficient = (i % 2 == 0) ? 2.0 : 4.0;
			sum += coefficient * sin(x);
		}

		thread_sums[thread_id] = sum;
	}


	for (auto& thread : threads) {
		thread.join();
	}

	for (double sum : thread_sums) {
		total_sum += sum;
	}

	return total_sum * h / 3.0;
}
int main()
{

	float a = 0, b = 3.14;
	int n = 10000000;

	std::vector<int> thread_counts = { 1, 2, 3, 4, 5, 6, 7, 8 };

	std::cout << "std::thread" << std::endl;
	for (int num_threads : thread_counts) {
		auto start = std::chrono::high_resolution_clock::now();
		double multi_result = simpson_in_threads(a, b, n, num_threads);
		auto end = std::chrono::high_resolution_clock::now();
		auto multi_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);


		std::cout << "Threads: " << num_threads
			<< ", Result: " << multi_result
			<< ", Time: " << multi_time.count() << " ms"
			<< std::endl;
	}
	std::cout << std::endl;
	std::cout << "OpenMP (std::thread)" << std::endl;
	for (int num_threads : thread_counts) {
		auto start = std::chrono::high_resolution_clock::now();
		double multi_result = simpson_in_threads_with_OpenMP(a, b, n, num_threads);
		auto end = std::chrono::high_resolution_clock::now();
		auto multi_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		std::cout << "Threads: " << num_threads
			<< ", Result: " << multi_result
			<< ", Time: " << multi_time.count() << " ms"
			<< std::endl;
	}

	std::cout << std::endl;


	DelegateThreads Dt;
	
	
	// –аскомментировать и использовать единожды, если нужно больше чисел в файле, просто измени второй параметр на желаемое число
	//generateOnceFileWithRandomNumbers("ba.txt", 1500000);
	std::cout << "OpenMP (class DelegateThreads)" << std::endl;
	for (int num_threads : thread_counts) {
		auto start = std::chrono::high_resolution_clock::now();
		Dt.working_threads_openmp(num_threads, "ba.txt", "bans.txt");
		auto end = std::chrono::high_resolution_clock::now();
		auto multi_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	
		std::cout << "Threads: " << num_threads
			<< ", Time: " << multi_time.count() << " ms"
			<< std::endl;
	}
	return 0;
}