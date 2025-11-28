#ifndef SEARCH_SA_HPP
#define SEARCH_SA_HPP

#include <sdsl/suffix_arrays.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include "utils.hpp"

using namespace sdsl;
using namespace std;

class SearchSA {
    int_vector<> seq; // El texto T
    int_vector<> sa;  // El arreglo de sufijos
    int32_t n;
    DocumentManager doc_manager;

public:
    SearchSA(const string& text_file, const string& pos_file) {
        // Carga texto
        load_vector_from_file(seq, text_file, 1);
        n = seq.size();
        seq.resize(n + 1); // centinel
        seq[n] = 0;
        n++;

        // construcción SA
        sa.resize(n);
        algorithm::calculate_sa((const unsigned char*)seq.data(), n, sa);

        // limites de documentos
        doc_manager.load_boundaries(pos_file);
    }

    // comparamos patron P con sufijo en sa_index
    // retornará: 0 si iguales, <0 si P < sufijo, >0 si P > sufijo
    int compare_suffix(const string& P, int32_t sa_index) {
        for (size_t i = 0; i < P.size(); ++i) {
            if (sa_index + i >= n) return 1;  // el sufijo es más corto que P
            unsigned char char_text = (unsigned char)seq[sa_index + i];
            if (P[i] < char_text) return -1;
            if (P[i] > char_text) return 1;
        }
        return 0; // match completo
    }

    // Retorna lista documentos que contienen el patrón P
    vector<int> search(const string& P) {
        if (P.size() == 0) return {};

        // Bin search para encontrar rango [L, R] en el SA
        int32_t L = 0, R = n - 1;
        int32_t sp = -1, ep = -1;

        // Lower bound
        while (L <= R) {
            int32_t mid = L + (R - L) / 2;
            int cmp = compare_suffix(P, sa[mid]);
            if (cmp <= 0) { 
                if (cmp == 0) sp = mid;
                R = mid - 1;
            } else {
                L = mid + 1;
            }
        }

        // no encontró match
        if (sp == -1) return {};

        // Upper bound, buscamos al final del rango
        L = sp; R = n - 1;
        while (L <= R) {
            int32_t mid = L + (R - L) / 2;
            int cmp = compare_suffix(P, sa[mid]);
            if (cmp == 0) {
                ep = mid;
                L = mid + 1;
            } else { 
                R = mid - 1;
            }
        }

        // recuperando id de documentos
        set<int> doc_ids;
        for (int i = sp; i <= ep; ++i) {
            doc_ids.insert(doc_manager.get_doc_id(sa[i]));
        }

        return vector<int>(doc_ids.begin(), doc_ids.end());
    }
    
    size_t size_in_bytes() { return size_in_mega_bytes(sa) * 1024 * 1024; }
};

#endif