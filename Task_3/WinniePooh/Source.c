#include <stdio.h>
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#include <stdbool.h>
#include <windows.h>

// �������, ������������ ���.
int** forest;

// ������ ������� ����� ��� ��������� ����������.
int primes[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71 };
int primes_size = 20;

// ���������� �����, �������� � �������.
int n, m, threads_amt;
int* thread_free;

// ����������� ����� ���� � �������.
int the_Winnie_Pooh = 7;

// ����� ��� ������.������� ���� � ������ ��� ������. � ���� ��� ������.
FILE* file_in; 
FILE* file_out;

// ��������� ������������� ������� ����.
typedef struct {
	pthread_mutex_t mutex;
	int str_idx;
	int thread_idx;
} bee_patrol_data;

// ��������� ������� ��� ���������� ����.
void* bee_patrol(void* param) {
	bee_patrol_data* data_ptr = (bee_patrol_data*)param;

	// ����� ������, � ����, ������� ����������� �����.
	int idx = data_ptr->str_idx;
	bool find = false;
	// ���������� �������, � ������ � ������� idx.
	for (int j = 0; j < m; j++) {
		// ��� ��������, ����� ��� ���� ������ � �������, ���������� ������� ��������� 
		// ������� ��������� ����. ������� �������� ���������� ������� ���������.
		int cur_num = forest[idx][j];
		int primes_amt = 0;
		for (int prime_idx = 0; prime_idx < primes_size && cur_num != 1; prime_idx++) {
			while (cur_num % primes[prime_idx] == 0) {
				cur_num /= primes[prime_idx];
				primes_amt++;
			}
		}

		// ���� ��� ������ ����� ���, �� ����� ���������� ������������� ���������.
		// �� ��� ����� ���� ��������� ������ ������� �������� � ������ ������� ����.
		pthread_mutex_lock(&(data_ptr->mutex));
		if (primes_amt == the_Winnie_Pooh) {
			find = true;
			fprintf_s(file_out, "!!!!� ������� %d ��������� ����� ���\n", idx);
			while (the_Winnie_Pooh-- > 0) {
				fprintf_s(file_out, "!  !    ���������� ��������� ����� ����! %d\n", the_Winnie_Pooh);
			}
			fprintf_s(file_out, "!!!!� ������� %d ��������� ��������� ����� ����\n", idx);
		}
		pthread_mutex_unlock(&(data_ptr->mutex));

	}

	// ���� ����� ���� �� ���� �������, �� �������, ��� ��� ���.
	pthread_mutex_lock(&(data_ptr->mutex));
	if (!find)
		fprintf_s(file_out, "....� ������� %d ����� ��� �� ������...\n", data_ptr->str_idx);
	fprintf_s(file_out, "<-- �������� ����� %d �������� � ����\n", data_ptr->thread_idx);
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

	// ������� ������� ������� � ���������� �������.
	fscanf_s(file_in, "%d %d %d", &n, &m, &threads_amt);

	// �������� � ������� �������.
	forest = (int**)malloc(n * sizeof(int *));
	for (size_t i = 0; i < n; i++) {
		forest[i] = (int*)malloc(m * sizeof(int));
		for (size_t j = 0; j < m; j++) {
			fscanf_s(file_in, "%d", &forest[i][j]);
		}
	}


	// �������������� ��� �������� �������
	pthreads = (pthread_t*)malloc(threads_amt * sizeof(pthread_t));
	thread_free = (int*)malloc(threads_amt * sizeof(int));
	for (size_t i = 0; i < threads_amt; i++) thread_free[i] = 1;

	// ������� mutex ��� �������.
	pthread_mutex_init(&mutex, NULL);

	// ���������������� ������ � ������� ��� ��������.
	data = (bee_patrol_data*)malloc(threads_amt * sizeof(bee_patrol_data));



	// �������� ��� �������(������) ����.
	for (size_t i = 0; i < n && the_Winnie_Pooh != -1; i++) {
		bool get = false;
		// �������� ��� ������� � ����� ���������.
		for (size_t t_num = 0; t_num < threads_amt && !get; t_num++) {
			if (thread_free[t_num]) {
				// ����������������� ������.
				data[t_num].mutex = mutex;
				data[t_num].str_idx = i;
				data[t_num].thread_idx = t_num;

				// ���������, ��� ������� �����, ������� ��� �� �������, � �������� ��� ����������� ���.
				thread_free[t_num] = 0;
				fprintf_s(file_out, "--> �������� ����� %d ������� �� ��� ������������ ������� %d\n", t_num, i);
				pthread_create(&(pthreads[t_num]), NULL, bee_patrol, &(data[t_num]));
				get = true;;
			}
		}
		// ���� ��� ������� ������ ������� ���������� ������, �� ��������� ����� ��� ���, 
		// ���� �� �����.
		if(!get) i--;
	}

	// ������� ����������� ���� ��������.
	for (size_t i = 0; i < threads_amt; i++) {
		if (!thread_free[i])
 			pthread_join(pthreads[i], NULL);
	}

	// ��������� mutex.
	pthread_mutex_destroy(&mutex);

	// ������� ������.
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