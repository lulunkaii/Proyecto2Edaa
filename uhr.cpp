/** uhr: generic time performance tester
 * Author: LELE
 *
 * Things to set up:
 * 0. Includes: include all files to be tested,
 * 1. Time unit: in elapsed_time, (SET TO MICROSECONDS)
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

// Include to be tested files here
#include "search_algorithms.h" // 🚨 ¡IMPORTANTE! Archivo con las 3 implementaciones

// ----------------------------------------------------------------------
// CORRECCIÓN DE ERROR DE ASERCIÓN: IMPLEMENTACIÓN SEGURA DE QUARTILES
// ----------------------------------------------------------------------
inline void quartiles(std::vector<double>& data, std::vector<double>& q)
{
    q.resize(5);
    std::size_t n = data.size();

    if (n < 4) {
        std::cerr << "quartiles needs at least 4 data points." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // 1. Ordenar los datos (ESENCIAL para calcular cuartiles por índice)
    std::sort(data.begin(), data.end());

    // 2. Q0 (Min) y Q4 (Max)
    q[0] = data.front(); // data[0]
    q[4] = data.back();  // data[n-1]

    // 3. Q2 (Mediana)
    if (n % 2 == 1) {
        q[2] = data[n / 2];
    } else {
        q[2] = (data[n / 2 - 1] + data[n / 2]) / 2.0;
    }

    // 4. Q1 y Q3 (Usando el método de interpolación simple)
    double pos_q1 = (n + 1) * 0.25;
    double pos_q3 = (n + 1) * 0.75;
    
    // Q1
    std::size_t idx1 = static_cast<std::size_t>(std::round(pos_q1)) - 1;
    idx1 = std::min(idx1, n - 1); 
    q[1] = data[idx1];

    // Q3
    std::size_t idx3 = static_cast<std::size_t>(std::round(pos_q3)) - 1;
    idx3 = std::min(idx3, n - 1); 
    q[3] = data[idx3];
}
// ----------------------------------------------------------------------

inline void validate_input(int argc, char *argv[], std::int64_t& runs,
    std::int64_t& lower, std::int64_t& upper, std::int64_t& step)
{
    if (argc != 6) {
        std::cerr << "Usage: <filename> <RUNS> <LOWER> <UPPER> <STEP>" << std::endl;
        std::cerr << "<filename> is the name of the file where performance data will be written." << std::endl;
        std::cerr << "It is recommended for <filename> to have .csv extension and it should not previously exist." << std::endl;
        std::cerr << "<RUNS>: numbers of runs per test case: should be >= 32." << std::endl;
        std::cerr << "<LOWER>: lower limit of the loop." << std::endl;
        std::cerr << "<UPPER>: upper limit of the loop." << std::endl;
        std::cerr << "<STEP>: step for the loop." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    try {
        runs = std::stoll(argv[2]);
        lower = std::stoll(argv[3]);
        upper = std::stoll(argv[4]);
        step = std::stoll(argv[5]);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: runs, lower, upper and step must be numbers." << std::endl;
        std::exit(EXIT_FAILURE);
    } catch (const std::out_of_range& e) {
        std::cerr << "Out of range: runs, lower, upper and step must be representable as a 64-bit integer." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    if (runs < 32) {
        std::cerr << "<RUNS> should be >= 32." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    if (lower < 1 or upper < 1 or step < 1 or lower > upper) {
        std::cerr << "Limits and step must be positive and LOWER <= UPPER." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::ifstream file(argv[1]);
    if (file.is_open()) {
        std::cerr << "File '" << argv[1] << "' already exists." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

inline void display_progress(std::int64_t executed_runs, std::int64_t total_runs)
{
    static int bar_width = 70;
    double progress = (double)executed_runs / total_runs;
    int pos = bar_width * progress;

    std::cout << "\r[";
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << (int)(progress * 100.0) << " %";
    std::cout.flush();
}

int main(int argc, char *argv[])
{
    // Define variables
    std::int64_t runs;
    std::int64_t lower;
    std::int64_t upper;
    std::int64_t step;

    // 1. Validate and read input
    validate_input(argc, argv, runs, lower, upper, step);

    // Set up clock variables
    std::int64_t k_pos, i, executed_runs; // k_pos es la posición/valor del objetivo
    std::int64_t total_runs_additive = runs * (((upper - lower) / step) + 1);
    
    // 🚨 CORRECCIÓN: Inicialización de times con el tamaño correcto
    std::vector<double> times(runs); 
    std::vector<double> q;
    
    double mean_time, time_stdev, dev;
    auto begin_time = std::chrono::high_resolution_clock::now();
    auto end_time = std::chrono::high_resolution_clock::now();
    
    // 1. Time unit: microsegundos (std::micro)
    std::chrono::duration<double, std::micro> elapsed_time = end_time - begin_time;

    // Set up output file
    std::ofstream time_data(argv[1]);
    if (!time_data.is_open()) {
        std::cerr << "Error: Could not open file '" << argv[1] << "' for writing." << std::endl;
        return EXIT_FAILURE;
    }

    // 2. CSV header. El eje X es la posición (k)
    time_data << "k,t_mean,t_stdev,Q0,Q1,Q2,Q3,Q4" << std::endl;

    // Begin testing
    std::cout << "\033[0;36mRunning tests...\033[0m" << std::endl << std::endl;
    
    // ----------------------------------------------------------------------
    // 🚨 EXPERIMENTO B: EFECTO DE LA POSICIÓN (k) - SECUENCIAL ACTIVO
    // ----------------------------------------------------------------------
    const std::int64_t n_fixed = 100000; // Tamaño fijo de la secuencia (según plan: 100000)
    executed_runs = 0;

    // Generación de datos FIJA (tamaño 100000)
    std::vector<std::int64_t> data(n_fixed);
    for (std::int64_t val = 0; val < n_fixed; ++val) {
        data[val] = val; // Vector: [0, 1, 2, ..., 99999]
    }
    
    // Bucle Principal: k_pos varía (posición del elemento)
    for (k_pos = lower; k_pos <= upper; k_pos += step) { 
        mean_time = 0;
        time_stdev = 0;

        // --- CONFIGURACIÓN DEL TEST ---
        // 1. Target: Buscamos el valor que está en la posición k_pos.
        // Como data[i] = i, el valor a buscar es k_pos.
        const std::int64_t target = k_pos; 
        
        // Validación de índice (se recomienda mantener)
        assert(k_pos >= 0 && k_pos < n_fixed); 
        // -----------------------------

        // Run to compute elapsed time
        for (i = 0; i < runs; i++) {
            display_progress(++executed_runs, total_runs_additive);

            begin_time = std::chrono::high_resolution_clock::now();
            
            // --- FUNCIÓN A TESTEAR ---
            // 🚨 Primer experimento B: BÚSQUEDA SECUENCIAL
            // sequential_search(data, target); 
            // Para el segundo experimento, cambia a:
            // binary_search_impl(data, target);
            // Para el tercer experimento, cambia a:
            galloping_search(data, target);
            // -------------------------
            
            end_time = std::chrono::high_resolution_clock::now();

            elapsed_time = end_time - begin_time;
            times[i] = elapsed_time.count();
            mean_time += times[i];
        }

        // Compute statistics
        mean_time /= runs;

        for (i = 0; i < runs; i++) {
            dev = times[i] - mean_time;
            time_stdev += dev * dev;
        }

        time_stdev /= runs - 1; 
        time_stdev = std::sqrt(time_stdev);

        quartiles(times, q);

        // 3. Escribe 'k_pos' (posición) como variable independiente
        time_data << k_pos << "," << mean_time << "," << time_stdev << ",";
        time_data << q[0] << "," << q[1] << "," << q[2] << "," << q[3] << "," << q[4] << std::endl;
    }
    // ----------------------------------------------------------------------
    
    // This is to keep loading bar after testing
    std::cout << std::endl << std::endl;

    time_data.close();

    return EXIT_SUCCESS;
}