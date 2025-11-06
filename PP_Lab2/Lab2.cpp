#include "DelegateThreads.h"
int main()
{
	DelegateThreads Dt;
	generateOnceFileWithRandomNumbers("ba.txt", 3894637);
	
	auto start_2 = std::chrono::high_resolution_clock::now();	
	Dt.working_threads(2, "ba.txt", "bans.txt");
	auto end_2 = std::chrono::high_resolution_clock::now();
	auto multi_time_2 = std::chrono::duration_cast<std::chrono::milliseconds>(end_2 - start_2);
	std::cout << "\Time: " << multi_time_2.count() << " ms" << std::endl;

	//std::cout << primeNumbers(8);
	return 0;
}