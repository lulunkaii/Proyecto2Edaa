#ifndef SEARCH_FM_HPP
#define SEARCH_FM_HPP

#include <sdsl/suffix_arrays.hpp>
#include <string>
#include <vector>
#include <set>
#include "utils.hpp"

using namespace sdsl;
using namespace std;

class SearchFM {
    // Usamos Huffman wavelet tree para compresión, aprovecha textos repetitivos
    csa_wt<wt_huff<>> fm_index; 
    DocumentManager doc_manager;

public:
    SearchFM(const string& text_file, const string& pos_file) {
        construct(fm_index, text_file, 1);
        doc_manager.load_boundaries(pos_file);
    }

    vector<int> search(const string& pattern) {
        // count retorna número de ocurrencias
        size_t occs = count(fm_index, pattern.begin(), pattern.end());
        
        if (occs == 0) return {};

        // locate retorna las posiciones en T
        auto locations = locate(fm_index, pattern.begin(), pattern.end());
        
        set<int> doc_ids;
        for (auto loc : locations) {
            doc_ids.insert(doc_manager.get_doc_id(loc));
        }

        return vector<int>(doc_ids.begin(), doc_ids.end());
    }

    size_t size_in_bytes() { return size_in_mega_bytes(fm_index) * 1024 * 1024; }
};

#endif