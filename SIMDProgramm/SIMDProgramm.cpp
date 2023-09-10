// SIMDProgramm.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <vector>
#include <chrono>
#include <immintrin.h>

void print_matrix(int** matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}
int main() {
    int size = 4096;
    __int32** Am = new __int32* [size];
    __int32** Bm = new __int32* [size];
    __int32** Rs = new __int32* [size];
    __int32** Rv = new __int32* [size];

    for (int i = 0; i < size; i++) {
        Am[i] = new __int32[size];
        Bm[i] = new __int32[size];
        Rs[i] = new __int32[size];
        Rv[i] = new __int32[size];
        for (int j = 0; j < size; j++) {
            Am[i][j] = rand() % 5;
            Bm[i][j] = rand() % 5;
            Rs[i][j] = 0;
            Rv[i][j] = 0;
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
    //    print_matrix(Rs, size);

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

    bool equal = true;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (Rs[i][j] != Rv[i][j]) {
                equal = false;
            }
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
        delete[] Rs[i];
        delete[] Rv[i];
    }

    delete[] Am;
    delete[] Bm;
    delete[] Rs;
    delete[] Rv;

    return 0;
}

