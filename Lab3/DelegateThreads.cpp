#include "DelegateThreads.h"
std::vector<int> DelegateThreads::primeNumber(int number)
{
	std::vector<int> answer;

	if (number <= 1) return answer;

	// Обработка делителя 2 отдельно
	while (number % 2 == 0) {
		answer.push_back(2);
		number /= 2;
	}

	// Проверяем только нечётные делители
	for (int div = 3; div * div <= number; div += 2) {
		while (number % div == 0) {
			answer.push_back(div);
			number /= div;
		}
	}

	// Если осталось простое число > 2
	if (number > 1) {
		answer.push_back(number);
	}

	return answer;
}
void DelegateThreads::worker_thread(std::ofstream& output_file, std::vector<std::string>& local_results)
{
	while (true)
	{
		int current_number = 0;
		bool got_number = false;

		{
			std::lock_guard<std::mutex> lock(input_mutex);
			if (!numbers_queue.empty())
			{
				current_number = numbers_queue.front();
				numbers_queue.pop();
				got_number = true;
			}
			else if (all_numbers_processed)
			{
				break;
			}
		}
		if (!got_number)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}
		std::vector<int> factors = primeNumber(current_number);

		std::string result = std::to_string(current_number) + ": ";

		for (size_t i = 0; i < factors.size(); ++i)
		{
			result += std::to_string(factors[i]);
			if (i < factors.size() - 1) result += " ";
		}
		result += '\n';

		local_results.push_back(result);

		if (local_results.size() >= max_batch_size)
		{
			std::lock_guard<std::mutex> lock(output_mutex);
			for (const auto& res : local_results)
			{
				output_file << res;
			}
			local_results.clear();
			output_file.flush();
		}

	}
	if (local_results.empty())
	{
		std::lock_guard<std::mutex> lock(output_mutex);
		for (const auto& res : local_results)
		{
			output_file << res;
		}
		output_file.flush();
	}
}
void DelegateThreads::working_threads_openmp(int count_threads,
	const std::string& input_file, const std::string& output_file)
{
	// ВСЕ как в оригинале, только замена std::thread на OpenMP

	std::ifstream file(input_file);
	if (!file.is_open()) {
		std::cerr << "Cannot open input file" << std::endl;
		return;
	}

	int number;
	while (file >> number) {
		numbers_queue.push(number);
	}
	file.close();

	std::ofstream out_file(output_file);
	if (!out_file.is_open()) {
		std::cerr << "Cannot open output file" << std::endl;
		return;
	}

	std::vector<std::vector<std::string>> thread_results(count_threads);

	// Устанавливаем флаг ДО начала параллельной секции
	all_numbers_processed = true;

	// Каждый поток OpenMP вызывает worker_thread
	#pragma omp parallel num_threads(count_threads)
	{
		int thread_id = omp_get_thread_num();
		worker_thread(out_file, thread_results[thread_id]);
	}

	out_file.close();
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