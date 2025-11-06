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

void generateOnceFileWithRandomNumbers(std::string nameFile, int count);

class DelegateThreads 
{
private:
	std::vector<std::string> shared_vector_answers;
	std::mutex mtx;
public:
	std::string primeNumber(int number);
	void working_threads(int count_threads, const std::string nameFile, const std::string nameFileAns);
};