// 6semEVMLab2.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

unsigned long long length = 1e10;
unsigned long long sum = 0;

const int MAXTHREADS = 4;
unsigned long long sumArr[MAXTHREADS]{};

void Calculate(unsigned long long begin, unsigned long long end, int thread_n) {
	unsigned long long sumA = 0;
	for (auto i = begin; i <= end; i++)  sumA += i;

	sumArr[thread_n] += sumA;

}

int main() {
	// Вычисление суммы ряда без использования потоков

	auto start_1 = std::chrono::high_resolution_clock::now();

	for (unsigned long long i = 1; i <= length; i++) {
		sum = sum + i;
    }

	auto end_1 = std::chrono::high_resolution_clock::now();

	std::chrono::duration<float> dur1 = (end_1 - start_1) * 1000;
	cout <<"No thread sum:" << sum << endl;

	std::cout << "Time: " << dur1.count() << " milliseconds\n" << endl;

	
	//Вычисление суммы ряда с использованием потоков
	auto start_2 = std::chrono::high_resolution_clock::now();

	thread threads[MAXTHREADS];

	for (int i = 0; i < MAXTHREADS; i++) {
		if (i == 0) {
			threads[i] = thread(Calculate, 1,length/MAXTHREADS,i);
		}
		else {
			threads[i] = thread(Calculate, ((length * i) / MAXTHREADS) + 1, ((i+1)*length)/MAXTHREADS,i);
		}
	}

	for (auto i = 0; i < MAXTHREADS; i++) {
		threads[i].join();
	}

	auto end_2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> dur2 = (end_2 - start_2) * 1000;
	
	cout << "thread sum:" << (sumArr[0]+sumArr[1]+sumArr[2]+sumArr[3]) << endl;
	std::cout << "Time: " << dur2.count() << " milliseconds\n" << endl;

	return 0;
}


