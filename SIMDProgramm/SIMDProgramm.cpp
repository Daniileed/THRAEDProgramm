// SIMDProgramm.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <thread>
#include <chrono>
#include <immintrin.h>


const int MAXTHREADS = 4;
const int size = 4096;

__int32** Am = new __int32* [size];
__int32** Bm = new __int32* [size];
__int32** Rs = new __int32* [size];
__int32** Rv = new __int32* [size];
__int32** ThreadS = new __int32* [size];
__int32** ThreadV = new __int32* [size];

void CalculateThread(int begin, int end) {

    for (int i = begin; i < end; ++i) {
        for (int j = 0; j < size; ++j) {
            for (int k = 0; k < size; ++k) {
                ThreadS[i][j] += Am[i][k]*Bm[k][j];
            }
        }
    }
}

void CalculateVectorThread() {
}

void print_matrix(int** matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

int main() {
   for (int i = 0; i < size; i++) {
        Am[i] = new __int32[size];
        Bm[i] = new __int32[size];
        Rs[i] = new __int32[size];
        Rv[i] = new __int32[size];
        ThreadS[i] = new __int32[size];
        ThreadV[i] = new __int32[size];

        for (int j = 0; j < size; j++) {
            Am[i][j] = rand() % 5;
            Bm[i][j] = rand() % 5;
            Rs[i][j] = 0;
            Rv[i][j] = 0;
            ThreadS[i][j] = 0;
            ThreadV[i][j] = 0;
        }
    }


  std::cout << "Scalar: \n";
   auto start_1 = std::chrono::high_resolution_clock::now();

   for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            for (int k = 0; k < size; ++k) {
               Rs[i][j] += Am[i][k] * Bm[k][j];
            }
       }
    }

 auto end_1 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<float> dur1 = (end_1 - start_1) * 1000;
  std::cout << "Time: " << dur1.count() << " milliseconds\n\n";

    __int32** BT = new __int32* [size];
    for (int i = 0; i < size; i++) {
        BT[i] = new __int32[size];
        for (int j = 0; j < size; j++) {
            BT[i][j] = Bm[j][i];
        }
    }

    std::cout << "Vector: \n";
    auto start_2 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            __m128i sum = _mm_setzero_si128();
            for (int k = 0; k < size; k += 4) {
                __m128i al = _mm_loadu_si128((__m128i*) & Am[i][k]);
                __m128i bl = _mm_loadu_si128((__m128i*) & BT[j][k]);
                __m128i mult = _mm_mullo_epi16(al, bl);
                sum = _mm_add_epi32(mult, sum);
            }
            __m128i line_sum = _mm_hadd_epi32(_mm_hadd_epi32(sum, sum), _mm_hadd_epi32(sum, sum));
            _mm_storeu_si32((__m128i*) & Rv[i][j], line_sum);

        }
    }

    auto end_2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> dur2 = (end_2 - start_2) * 1000;
    std::cout << "Time: " << dur2.count() << " milliseconds\n\n";

    

    std::cout << "Thread scalar: \n";
    std::thread threads[MAXTHREADS];
        int step = size/MAXTHREADS;

        auto start_3 = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < MAXTHREADS; i++) {
         
                threads[i] = std::thread(CalculateThread, step*i, (step*(i+1)));
           
        }

        for (auto i = 0; i < MAXTHREADS; i++) {
            threads[i].join();
        }

        auto end_3 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> dur3 = (end_3 - start_3) * 1000;
        std::cout << "Time: " << dur3.count() << " milliseconds\n\n";

        std::cout << "Thread vector: \n";
        auto start_4 = std::chrono::high_resolution_clock::now();

        auto end_4 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> dur4 = (end_4 - start_4) * 1000;
        std::cout << "Time: " << dur4.count() << " milliseconds\n\n";


        bool equal = true;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if ((Rv[i][j] != Rs[i][j]) or (Rv[i][j] != ThreadS[i][j]))
                equal = false;
            }
        }

        if (equal) {
            std::cout << "Matrix is equal\n";
        }
        else {
            std::cout << "Matrix not equal\n";
        }


        for (int i = 0; i < size; i++) {
            delete[] Am[i];
            delete[] Bm[i];
            delete[] BT[i];
            delete[] Rs[i];
            delete[] Rv[i];
            delete[] ThreadS[i];
            delete[] ThreadV[i];
        }

        delete[] Am;
        delete[] Bm;
        delete[] BT;
        delete[] Rs;
        delete[] Rv;
        delete[] ThreadS;
        delete[] ThreadV;

        return 0;
    }


