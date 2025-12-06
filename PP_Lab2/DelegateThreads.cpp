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

	std::vector<std::string> ans = primeNumberV2(readNumbers, count_threads);

	std::ofstream fileAnswers(nameFileAns);
	if (!fileAnswers.is_open())
	{
		std::cerr << "Cannot open file" << std::endl;
		return;
	}

	for (auto str : ans)
	{
		fileAnswers << str;
	}
	fileAnswers.close();
}
std::vector<std::string> DelegateThreads::primeNumberV2(std::vector<int> numbers, int count_threads)
{
	std::vector<std::string> answers;
	std::vector<std::thread> threads;

	int chunks_per_thread = numbers.size() / count_threads;

	for (size_t i = 0; i < count_threads; ++i)
	{
		size_t start_index = i * chunks_per_thread;
		size_t end_index = (i == count_threads - 1) ? numbers.size() : start_index + chunks_per_thread;

		threads.emplace_back([&, start_index, end_index]() {

			// Локальный вектор для результатов этого потока
			std::vector<std::string> local_answers;

			for (size_t j = start_index; j < end_index; ++j)
			{
				int number = numbers[j];
				std::vector<int> ans_vec = primeNumber(numbers[j]);
				std::string ans = "";
				for (int num : ans_vec)
				{
					ans += std::to_string(num) + ' ';
				}
				ans += '\n';
				local_answers.push_back(ans);
			}

			// Блокируем мьютекс только для объединения результатов
			std::lock_guard<std::mutex> lock2(mtx2);
			answers.insert(answers.end(), local_answers.begin(), local_answers.end());
			});
	}

	for (auto& thread : threads)
	{
		thread.join();
	}
	return answers;
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