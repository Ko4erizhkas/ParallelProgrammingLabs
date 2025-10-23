#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <fstream>
#include <chrono>
#include <string>
#include <math.h>

std::string primeNumbers(long long number)
{
	std::string answer = std::to_string(number) + " = ";
	int div = 2;
	while (number > 1)
	{
		while (number % div == 0)
		{
			number = number / div;
			answer += std::to_string(div) + ' ';
		}
		div++;
	}
	return answer;
}
void generateOnceFileWithRandomNumbers(std::string nameFile, int count)
{
	std::vector<int> vec;
	vec.reserve(count);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(0, 10000);

	for (int i = 0; i < count; i++)
	{
		vec.push_back(dist(gen));
	}
	std::ofstream file(nameFile, std::ios::out);
	if (file.is_open())
	{
		std::cout << "File is open\n";
		for (int num : vec)
		{
			file << num;
			file << '\n';
		}
	}
	file.close();
}
std::vector<int> readFile(std::string nameFile)
{
	std::string num;
	int s;
	std::ifstream from(nameFile);
	std::vector<int> nums;
	if (from.is_open())
	{
		while (from >> num)
		{
			int numb = std::stoi(num);
			//std::cout << numb << std::endl;
			nums.push_back(numb);
		}
	}
	from.close();
	return nums;
}
void recordingFile(const std::string& nameFile)
{
	std::fstream file(nameFile);
	
	if (!file)
	{
		std::cerr << "Error opening file: " << nameFile << std::endl;
		return;
	}
	
	std::vector<std::string> info;
	std::string line;
 

 
	std::cout << "File is open\n";

	while (std::getline(file, line))
	{
		if (!line.empty())
		{
			info.push_back(line);
		}
	}


	for (auto& line : info)
	{
		try 
		{
			long long num = std::stoll(line);
			line = primeNumbers(num);
		}
		catch (std::exception& error)
		{
			std::cerr << "Error processing line: " << line << error.what() << std::endl;
		}
	}/*
	for (auto& i : info)
	{
		std::cout << i << std::endl;
	}*/
	//std::cout << info.size();
}

// Доделать или переделать!
class DelegateThreads
{
private:
	std::vector<std::thread> threads;
	bool ready[2] = {false, false};
public:
	void delegateToThreads(int count_threads, const std::string& nameFile)
	{
		std::ifstream file(nameFile);
		if (!file.is_open())
		{
			std::cerr << "Cannot open file" << std::endl;
		}

		
	}
};
int main()
{
	DelegateThreads Dt;
	Dt.delegateToThreads(2, "ba.txt");

	auto start_1 = std::chrono::high_resolution_clock::now();
	generateOnceFileWithRandomNumbers("bla-bla-bla-ble-ble-ble-blu-blu-blu.txt", 5000000);
	auto end_1 = std::chrono::high_resolution_clock::now();
	auto multi_time_1 = std::chrono::duration_cast<std::chrono::milliseconds>(end_1 - start_1);
	std::cout << "\nGenerate file with random numbers" << std::endl;
	std::cout << multi_time_1.count() << " ms\n";

	auto start_2 = std::chrono::high_resolution_clock::now();
	recordingFile("bla-bla-bla-ble-ble-ble-blu-blu-blu.txt");
	auto end_2 = std::chrono::high_resolution_clock::now();
	auto multi_time_2 = std::chrono::duration_cast<std::chrono::milliseconds>(end_2 - start_2);
	std::cout << "\nRecording file with answers" << std::endl;
	std::cout << multi_time_2.count() << " ms\n";

	//std::cout << primeNumbers(8);
	return 0;
}