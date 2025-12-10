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
#include <queue>

void generateOnceFileWithRandomNumbers(std::string nameFile, int count);

class DelegateThreads 
{
private:
    std::mutex input_mutex;
    std::mutex output_mutex;
    std::queue<int> numbers_queue;
    bool all_numbers_processed = false;
    const int max_batch_size = 10;
public:
	std::vector<int> primeNumber(int number);
	std::vector<std::string> primeNumberV2(std::deque<std::vector<int>> chunk_numbers, int count_threads);
    void worker_thread(std::ofstream& output_file, std::vector<std::string>& local_results);
	void working_threads(int count_threads, const std::string& nameFile, const std::string& nameFileAns);
};