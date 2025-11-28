// Este código fue editado para el proyecto 2 de Estructuras de Datos y Algoritmos Avanzados
// Recuperado de https://github.com/jfuentess/edaa/blob/main/experimentos/uhr.cpp


/** uhr: generic time performance tester
 * Author: LELE
 *
 * Things to set up:
 * 0. Includes: include all files to be tested,
 * 1. Time unit: in elapsed_time,
 * 2. What to write on time_data,
 * 3. Data type and distribution of RNG,
 * 4. Additive or multiplicative stepping,
 * 5. The experiments: in outer for loop. */

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include <sdsl/suffix_arrays.hpp>
#include "fm_index.hpp"
#include "sa.hpp"

#ifdef USE_SA
    using SolverType = SearchSA;
#else
    using SolverType = SearchFM;
#endif

inline void validate_input(int argc, char *argv[], std::int64_t& runs,
    std::int64_t& lower, std::int64_t& upper, std::int64_t& step,
    std::string& corpus_file, std::string& pos_file, std::string& dataset_label)
{
    if (argc != 9) {
        std::cerr << "Usage: <output.csv> <RUNS> <LOWER> <UPPER> <STEP> <CORPUS> <POSITIONS> <LABEL>" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    try {
        runs = std::stoll(argv[2]);
        lower = std::stoll(argv[3]);
        upper = std::stoll(argv[4]);
        step = std::stoll(argv[5]);
        corpus_file = std::string(argv[6]);
        pos_file = std::string(argv[7]);
        dataset_label = std::string(argv[8]);
    } catch (...) {
        std::cerr << "Error parsing arguments." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

inline void display_progress(std::int64_t u, std::int64_t v) {
    const double progress = u / double(v);
    const std::int64_t width = 70;
    const std::int64_t p = width * progress;
    std::cout << "\033[1m[";
    for (std::int64_t i = 0; i < width; i++) std::cout << (i < p ? "=" : (i == p ? ">" : " "));
    std::cout << "] " << std::int64_t(progress * 100.0) << "%\r\033[0m";
    std::cout.flush();
}

inline void quartiles(std::vector<double>& data, std::vector<double>& q) {
    q.resize(5);
    std::size_t n = data.size();
    std::sort(data.begin(), data.end());
    if (n < 4) exit(1);
    q[0] = data.front(); q[4] = data.back();
    if (n % 2 == 1) q[2] = data[n / 2];
    else q[2] = (data[n / 2 - 1] + data[n / 2]) / 2.0;
    if (n % 4 >= 2) { q[1] = data[n / 4]; q[3] = data[(3 * n) / 4]; }
    else { std::size_t p = n / 4; q[1] = 0.25 * data[p - 1] + 0.75 * data[p]; p = (3 * n) / 4; q[3] = 0.75 * data[p - 1] + 0.25 * data[p]; }
}

int main(int argc, char *argv[])
{
    std::int64_t runs, lower, upper, step;
    std::string corpus_file, pos_file, dataset_label;
    
    validate_input(argc, argv, runs, lower, upper, step, corpus_file, pos_file, dataset_label);

    std::vector<double> times(runs);
    std::vector<double> q;
    double mean_time, time_stdev, dev;
    auto begin_time = std::chrono::high_resolution_clock::now();
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::nano> elapsed_time = end_time - begin_time;

    std::mt19937_64 rng(42);

    std::ofstream time_data;
    time_data.open(argv[1]);
    time_data << "dataset,n,memory_mb,build_time_ms,t_mean,t_stdev,t_Q0,t_Q1,t_Q2,t_Q3,t_Q4" << std::endl;

    // Carga texto en la memoria
    std::string full_text;
    {
        std::ifstream t(corpus_file, std::ios::binary);
        if (!t) { std::cerr << "Error opening corpus file: " << corpus_file << std::endl; return 1; }
        t.seekg(0, std::ios::end);
        size_t size = t.tellg();
        full_text.resize(size);
        t.seekg(0);
        t.read(&full_text[0], size);
    }

    std::cout << "Building Index for " << corpus_file << "..." << std::endl;
    
    // Define método
    #ifdef USE_SA
        std::string method = "SA";
    #else
        std::string method = "FM";
    #endif

    auto build_start = std::chrono::high_resolution_clock::now();

    SolverType solver(corpus_file, pos_file);

    auto build_end = std::chrono::high_resolution_clock::now();
    double build_ms = std::chrono::duration<double, std::milli>(build_end - build_start).count();
    
    // Calculo de memoria en MB
    double memory_mb = solver.size_in_bytes() / (1024.0 * 1024.0);
    
    std::cout << "Construction Time (" << method << "): " << build_ms << " ms" << std::endl;
    std::cout << "Size in Bytes: " << solver.size_in_bytes() << " (" << memory_mb << " MB)" << std::endl;

    // Begin testing
    std::cout << "\033[0;36mRunning tests...\033[0m" << std::endl << std::endl;
    std::int64_t total_runs = runs * (((upper - lower) / step) + 1);
    std::int64_t executed_runs = 0;

    for (std::int64_t n = lower; n <= upper; n += step) {
        mean_time = 0;
        time_stdev = 0;
        std::uniform_int_distribution<std::int64_t> u_distr(0, full_text.size() - n - 1);

        for (int i = 0; i < runs; i++) {
            display_progress(++executed_runs, total_runs);
            std::int64_t rand_pos = u_distr(rng);
            std::string pattern = full_text.substr(rand_pos, n);

            begin_time = std::chrono::high_resolution_clock::now();
            auto res = solver.search(pattern);
            volatile size_t s = res.size(); (void)s; 
            end_time = std::chrono::high_resolution_clock::now();

            elapsed_time = end_time - begin_time;
            times[i] = elapsed_time.count();
            mean_time += times[i];
        }

        mean_time /= runs;
        for (int i = 0; i < runs; i++) {
            dev = times[i] - mean_time;
            time_stdev += dev * dev;
        }
        time_stdev = std::sqrt(time_stdev / (runs - 1));
        quartiles(times, q);

        time_data << dataset_label << "," 
                  << n << "," 
                  << memory_mb << "," 
                  << build_ms << ","
                  << mean_time << "," 
                  << time_stdev << ","
                  << q[0] << "," << q[1] << "," << q[2] << "," << q[3] << "," << q[4] << std::endl;
    }

    std::cout << std::endl << "\033[1;32mDone!\033[0m" << std::endl;
    time_data.close();

    return 0;
}