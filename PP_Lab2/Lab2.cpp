#include "DelegateThreads.h"
int main()
{
	DelegateThreads Dt;
	// –аскомментировать и использовать единожды, если нужно больше чисел в файле, просто измени второй параметр на желаемое число
	//generateOnceFileWithRandomNumbers("ba.txt", 1500000);
	
	std::vector<std::vector<int>> numbers;


	auto start_2 = std::chrono::high_resolution_clock::now();	

	Dt.working_threads(8, "ba.txt", "bans.txt");
	
	auto end_2 = std::chrono::high_resolution_clock::now();
	auto multi_time_2 = std::chrono::duration_cast<std::chrono::milliseconds>(end_2 - start_2);
	std::cout << "\Time: " << multi_time_2.count() << " ms" << std::endl;

	//std::cout << primeNumbers(8);
	return 0;
}