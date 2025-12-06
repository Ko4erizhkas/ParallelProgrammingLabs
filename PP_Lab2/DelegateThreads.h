#pragma once
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <random>
#include <math.h>
#include <deque>

void generateOnceFileWithRandomNumbers(std::string nameFile, int count);

class DelegateThreads 
{
private:
	std::mutex mtx;
	std::deque<int> shared_numbers;
public:
	std::vector<int> primeNumber(int number);
	std::vector<std::string> primeNumberV2(std::deque<std::vector<int>> chunk_numbers, int count_threads);
	void working_threads(int count_threads, const std::string nameFile, const std::string nameFileAns);
};