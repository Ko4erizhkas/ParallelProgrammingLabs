#include "DelegateThreads.h"
std::vector<int> DelegateThreads::primeNumber(int number)
{
	std::vector<int> answer;
	int div = 2;
	while (number > 1)
	{
		while (number % div == 0)
		{
			number /= div;
			answer.push_back(div);
		}
		div++;
	}
	return answer;
}
void DelegateThreads::working_threads(int count_threads, const std::string nameFile, const std::string nameFileAns)
{
	// Открываем поток чтения из файла
	std::ifstream file(nameFile, std::ios::in);
	if (!file.is_open())
	{
		std::cerr << "Cannot open file" << std::endl;
		return;
	}

	// Прочитали и закрыли

	std::string str;
	std::vector<int> readNumbers;

	while (file >> str)
	{
		readNumbers.push_back(std::stoi(str));
	}
	file.close();

	std::vector<std::string> answers;

	// Переписать для нормальной работы в многопотоке
	auto primeNumber = [&](std::vector<std::vector<int>> chunk_numbers)
		{
			for (const auto& chunk : chunk_numbers)
			{
				for (auto number : chunk)
				{
					std::string ans = "";
					int div = 2;
					while (number > 1)
					{
						while (number % div == 0)
						{
							number /= div;
							ans += std::to_string(div) + ' ';
						}
						div++;
					}
					answers.push_back(ans);
				}
			}
		};


	std::vector<std::vector<int>> number_chunks;
	auto divisionIntoChunks = [&number_chunks](const std::vector<int>& nums, size_t chunk_size)
		{
			for (size_t i = 0; i < nums.size(); i += chunk_size)
			{
				auto start = nums.begin() + i;
				auto end = (i + chunk_size <= nums.size()) ? nums.begin() + i + chunk_size : nums.end();
				number_chunks.emplace_back(start, end);
			}
		};

	// Поток для разбиения
	std::thread division(divisionIntoChunks, readNumbers, 10);
	std::cout << "Division thread id: " << division.get_id() << std::endl;

	// Поток для расчёта множителей
	std::thread prime(primeNumber, number_chunks);
	std::cout << "Prime thread id: " << prime.get_id() << std::endl;

	division.join();
	prime.join();


	std::ofstream fileAnswers(nameFileAns, std::ios::out);
	if (!fileAnswers.is_open())
	{
		std::cerr << "Cannot open file" << std::endl;
		return;
	}


	for (auto str : answers)
	{
		fileAnswers << str << '\n';
	}
	fileAnswers.close();
}

std::vector<std::vector<int>> DelegateThreads::primeNumberV1(std::vector<std::vector<int>> chunk_numbers, int count_threads)
{
	std::vector<std::vector<int>> chunk_answers;
	chunk_numbers.resize(chunk_numbers.size());

	int chunks_per_thread = chunk_numbers.size() / count_threads;

	for (size_t i = 0; i < count_threads; ++i)
	{
		size_t start_index = i * chunks_per_thread;
		size_t end_index = (i == count_threads - 1) ? chunks_per_thread : start_index + chunks_per_thread;
		
		mtx.lock();
		for (size_t j = start_index; i < end_index; ++i)
		{
			for (int element : chunk_numbers[i])
			{
				chunk_answers.push_back(primeNumber(element));
			}
		}
		mtx.unlock();
	}
	return chunk_answers;
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