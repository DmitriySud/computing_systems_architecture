#include <stdio.h>
#include <omp.h>
#include <vector>
#include <fstream>
#include <string>

#define vec(type_) std::vector<type_>
#define vec2(type_) std::vector<std::vector<type_>>
#define str std::string

// �����, ����������� ���.
vec2(int) forest;

// ���ᨢ ������ �ᥫ ��� ���᪮��� �ᯥ��樨.
vec(int) primes = vec(int)({2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71});

// ������⢮ ��ப, �⮫�殢 � ��⮪��.
int n, m, threads_amt;

// ������祭�� ����� ��� � �����.
int the_Winnie_Pooh = 7;

// ����� ��� ࠡ���.��஥� 䠩� � ��⮬ ��� �⥭��. � 䠩� ��� �뢮��.
std::ifstream fin;
std::ofstream fout;

int main(int argc, char *argv[])
{

	fin = std::ifstream(argv[1]);
	fout = std::ofstream(argv[2]);

	// ��⠥� ࠧ���� ������ � ������⢮ ��⮪��.
	fin >> n >> m >> threads_amt;

	// �������� � �⭥��� ������.
	forest = vec2(int)(n, vec(int)(m, 0));
	for (size_t i = 0; i < n; i++)
	{
		for (size_t j = 0; j < m; j++)
		{
			fin >> forest[i][j];
		}
	}

	omp_set_num_threads(threads_amt);
// ��ॡ��� �� ���⪨(��ப�) ���.
#pragma omp parallel for schedule(dynamic,1)
	for (size_t i = 0; i < n; i++)
	{
#pragma omp critical(cout)
		{
			fout << "--> �祫��� ���� " << omp_get_thread_num() 
			<< "  �뫥⥫ �� ��� ��᫥������� ᥪ�� " << i << std::endl;
		}
	

		// ����� ��ப�, � ����, ������ ����㫨���� ����.
		int idx = i;
		bool find = false;

		// ���᫥��� �������, � ��ப� � ����஬ idx.
		for (int j = 0; j < m; j++)
		{
			// ��� �����⭮, ����� ��� ����� ⮫쪮 � ���⪠�, ������⢮ ������ ����⥫��
			// ������ ࠢ����� ᥬ�. ���⮬� ���᫨� ������⢮ ������ ����⥫��.
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

// �᫨ �� ������ ����� ���, � �㦭� �ந����� ������⥫쭮� ���������.
// �� ��� �⮣� ���� ������� ��㣨� ��⮪�� ��室��� � ����� ���⮪ ����.
#pragma omp critical(cout)
			{
				if (primes_amt == the_Winnie_Pooh)
				{
					find = true;
					fout << "!!!!� ᥪ�� " << idx << " �����㦥� ����� ���" << std::endl;
					while (the_Winnie_Pooh-- > 0)
					{
						fout << "! ! �ந�室�� ������ ����� ���! " << the_Winnie_Pooh << std::endl;
					}
					fout << "!!!!� ᥪ�� " << idx << " �����祭� ������ ����� ���" << std::endl;
				}
			}
		}

// �᫨ ����� ��� �� �뫮 �������, � ᮮ�騬, �� ��� ���.
#pragma omp critical(cout)
		{
			if (!find)
				fout << "....� ᥪ�� " << i << " ����� ��� �� ������..." << std::endl;
			fout << "<-- �祫��� ���� " << omp_get_thread_num() << " ������ � 㫥�" << std::endl;
		}
	}

	return 0;
}