#include <stdio.h>
#include <omp.h>
#include <vector>
#include <fstream>
#include <string>

#define vec(type_) std::vector<type_>
#define vec2(type_) std::vector<std::vector<type_>>
#define str std::string

// Матрица, моделирующая лес.
vec2(int) forest;

// Массив простых чисел для поисковой экспедиции.
vec(int) primes = vec(int)({2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71});

// Количество строк, столбцов и потоков.
int n, m, threads_amt;

// Обозначение Винни Пуха в матрице.
int the_Winnie_Pooh = 7;

// Файлы для работы.Откроем файл с тестом для чтения. И файл для вывода.
std::ifstream fin;
std::ofstream fout;

int main(int argc, char *argv[])
{

	fin = std::ifstream(argv[1]);
	fout = std::ofstream(argv[2]);

	// Считаем размеры матрицы и количество потоков.
	fin >> n >> m >> threads_amt;

	// Создание и чтнение матрицы.
	forest = vec2(int)(n, vec(int)(m, 0));
	for (size_t i = 0; i < n; i++)
	{
		for (size_t j = 0; j < m; j++)
		{
			fin >> forest[i][j];
		}
	}

	omp_set_num_threads(threads_amt);
// Переберём все участки(строки) леса.
#pragma omp parallel for schedule(dynamic,1)
	for (size_t i = 0; i < n; i++)
	{
#pragma omp critical(cout)
		{
			fout << "--> Пчелиный отряд " << omp_get_thread_num() 
			<< "  вылетел на для исследования сектора " << i << std::endl;
		}
	

		// Номер строки, в лесу, которую патрулируют пчёлы.
		int idx = i;
		bool find = false;

		// Вычисление медведя, в строке с номером idx.
		for (int j = 0; j < m; j++)
		{
			// Как известно, Винни Пух живёт только в клетках, количество простых делителей
			// которых равняется семи. Поэтому вычислим количество простых делителей.
			int cur_num = forest[idx][j];
			int primes_amt = 0;
			for (int prime_idx = 0; prime_idx < primes.size() && cur_num != 1; prime_idx++)
			{
				while (cur_num % primes[prime_idx] == 0)
				{
					cur_num /= primes[prime_idx];
					primes_amt++;
				}
			}

// Если был найден Винни Пух, то нужно произвести показательное наказание.
// Но для этого надо запретить другим потокам заходить в данный участок кода.
#pragma omp critical(cout)
			{
				if (primes_amt == the_Winnie_Pooh)
				{
					find = true;
					fout << "!!!!В секторе " << idx << " обнаружен Винни Пух" << std::endl;
					while (the_Winnie_Pooh-- > 0)
					{
						fout << "! ! Происходит экзекуция Винни Пуха! " << the_Winnie_Pooh << std::endl;
					}
					fout << "!!!!В секторе " << idx << " закончена экзекуция Винни Пуха" << std::endl;
				}
			}
		}

// Если Винни Пуха не было найдено, то сообщим, что его нет.
#pragma omp critical(cout)
		{
			if (!find)
				fout << "....В секторе " << i << " Винни Пух не найден..." << std::endl;
			fout << "<-- Пчелиный отряд " << omp_get_thread_num() << " вернулся в улей" << std::endl;
		}
	}

	return 0;
}