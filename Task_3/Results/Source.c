#include <stdio.h>
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#include <stdbool.h>
#include <windows.h>

// Матрица, моделирующая лес.
int** forest;

// Массив простых чисел для поисковой экспедиции.
int primes[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71 };
int primes_size = 20;

// Количество строк, столбцов и потоков.
int n, m, threads_amt;
int* thread_free;

// Обозначение Винни Пуха в матрице.
int the_Winnie_Pooh = 7;

// Файлы для работы.Откроем файл с тестом для чтения. И файл для вывода.
FILE* file_in; 
FILE* file_out;

// Структура прередаваеиая патрулю пчёл.
typedef struct {
	pthread_mutex_t mutex;
	int str_idx;
	int thread_idx;
} bee_patrol_data;

// Стартовая функция для патрульных пчёл.
void* bee_patrol(void* param) {
	bee_patrol_data* data_ptr = (bee_patrol_data*)param;

	// Номер строки, в лесу, которую патрулируют пчёлы.
	int idx = data_ptr->str_idx;
	bool find = false;
	// Вычисление медведя, в строке с номером idx.
	for (int j = 0; j < m; j++) {
		// Как известно, Винни Пух живёт только в клетках, количество простых делителей 
		// которых равняется семи. Поэтому вычислим количество простых делителей.
		int cur_num = forest[idx][j];
		int primes_amt = 0;
		for (int prime_idx = 0; prime_idx < primes_size && cur_num != 1; prime_idx++) {
			while (cur_num % primes[prime_idx] == 0) {
				cur_num /= primes[prime_idx];
				primes_amt++;
			}
		}

		// Если был найден Винни Пух, то нужно произвести показательное наказание.
		// Но для этого надо запретить другим потокам заходить в данный участок кода.
		pthread_mutex_lock(&(data_ptr->mutex));
		if (primes_amt == the_Winnie_Pooh) {
			find = true;
			fprintf_s(file_out, "!!!!В секторе %d обнаружен Винни Пух\n", idx);
			while (the_Winnie_Pooh-- > 0) {
				fprintf_s(file_out, "!  !    Происходит экзекуция Винни Пуха! %d\n", the_Winnie_Pooh);
			}
			fprintf_s(file_out, "!!!!В секторе %d закончена экзекуция Винни Пуха\n", idx);
		}
		pthread_mutex_unlock(&(data_ptr->mutex));

	}

	// Если Винни Пуха не было найдено, то сообщим, что его нет.
	pthread_mutex_lock(&(data_ptr->mutex));
	if (!find)
		fprintf_s(file_out, "....В секторе %d Винни Пух не найден...\n", data_ptr->str_idx);
	fprintf_s(file_out, "<-- Пчелиный отряд %d вернулся в улей\n", data_ptr->thread_idx);
	pthread_mutex_unlock(&(data_ptr->mutex));

	thread_free[data_ptr->thread_idx] = 1;

	return 1;
}

void files_init(char* in, char* out) {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	int res = fopen_s(&file_in, in, "r");
	res = fopen_s(&file_out, out, "w");
}

int main(int argc, char * argv[]) {
	pthread_t* pthreads;
	bee_patrol_data* data;
	pthread_mutex_t mutex;

	files_init(argv[1], argv[2]);

	// Считаем размеры матрицы и количество потоков.
	fscanf_s(file_in, "%d %d %d", &n, &m, &threads_amt);

	// Создание и чтнение матрицы.
	forest = (int**)malloc(n * sizeof(int *));
	for (size_t i = 0; i < n; i++) {
		forest[i] = (int*)malloc(m * sizeof(int));
		for (size_t j = 0; j < m; j++) {
			fscanf_s(file_in, "%d", &forest[i][j]);
		}
	}


	// Идентификаторы для дочерних потоков
	pthreads = (pthread_t*)malloc(threads_amt * sizeof(pthread_t));
	thread_free = (int*)malloc(threads_amt * sizeof(int));
	for (size_t i = 0; i < threads_amt; i++) thread_free[i] = 1;

	// Создать mutex для потоков.
	pthread_mutex_init(&mutex, NULL);

	// Инициализировать массив с данными для патрулей.
	data = (bee_patrol_data*)malloc(threads_amt * sizeof(bee_patrol_data));



	// Переберём все участки(строки) леса.
	for (size_t i = 0; i < n && the_Winnie_Pooh != -1; i++) {
		bool get = false;
		// Переберём все патрули и найдём свободный.
		for (size_t t_num = 0; t_num < threads_amt && !get; t_num++) {
			if (thread_free[t_num]) {
				// Проинициализируем данные.
				data[t_num].mutex = mutex;
				data[t_num].str_idx = i;
				data[t_num].thread_idx = t_num;

				// Обозначим, что патруль занят, сообщим что он вылетел, и запустим его исследовать лес.
				thread_free[t_num] = 0;
				fprintf_s(file_out, "--> Пчелиный отряд %d вылетел на для исследования сектора %d\n", t_num, i);
				pthread_create(&(pthreads[t_num]), NULL, bee_patrol, &(data[t_num]));
				get = true;;
			}
		}
		// Если для сектора небыло найдено свободного отряда, то попробуем найти ещё раз, 
		// пока не найдём.
		if(!get) i--;
	}

	// Дождёмся возвращения всех патрулей.
	for (size_t i = 0; i < threads_amt; i++) {
		if (!thread_free[i])
 			pthread_join(pthreads[i], NULL);
	}

	// Освободим mutex.
	pthread_mutex_destroy(&mutex);

	// Очистим память.
	free(pthreads);
	free(thread_free);
	for (size_t i = 0; i < n; i++) {
		free(forest[i]);	
	}
	free(forest);
	free(data);

	fclose(file_in);
	fclose(file_out);
	return 0;
}