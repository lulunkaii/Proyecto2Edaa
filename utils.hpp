#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>

using namespace std;

class DocumentManager {
    vector<uint64_t> limits; // Posiciones de inicio de cada doc

public:
    void load_boundaries(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error abriendo archivo de posiciones." << endl;
            exit(1);
        }
        uint64_t pos;
        while (file >> pos) {
            limits.push_back(pos);
        }
        file.close();
    }

    // Retorna el ID del documento dado una posición en T (bin search)
    int get_doc_id(uint64_t position) {
        // upper_bound retorna el primer elemento mayor a position
        auto it = upper_bound(limits.begin(), limits.end(), position);
        // El documento es el índice anterior al upper_bound
        return (int)distance(limits.begin(), it) - 1;
    }
};

#endif