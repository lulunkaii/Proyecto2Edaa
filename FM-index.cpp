// Construcción del FM index de un texto con soporte multi-documento
//
// Prerrequisitos: Tener la biblioteca SDSL instalada
//
// Compilación: g++ -std=c++11 -DNDEBUG -O3 \
//    -I/home/vt/include -L/home/vt/lib \
//    FM-index.cpp -o fmi -lsdsl -ldivsufsort -ldivsufsort64

#include <string>
#include <iostream>
#include <algorithm>
#include <set>
#include <fstream> 
#include <vector>  
#include <sdsl/rrr_vector.hpp> 
#include <sdsl/wavelet_trees.hpp>
#include <sdsl/suffix_arrays.hpp>
#include <sdsl/util.hpp> 

using namespace sdsl;
using namespace std;

/**
 * Función auxiliar para mapear una posición en el texto concatenado (T)
 * al ID del documento (D1, D2, D3, etc.) al que pertenece.
 */
size_t map_position_to_document_id(size_t pos, const sdsl::int_vector<>& starts) {
    auto it = std::upper_bound(starts.begin(), starts.end(), pos);
    size_t doc_id = std::distance(starts.begin(), it);
    
    if (doc_id == 0 && !starts.empty()) {
        return 1; 
    } else if (doc_id > starts.size() && !starts.empty()) {
        return starts.size();
    }
    
    return doc_id;
}


int main(int argc, char** argv) {
  if (argc !=  2) {
    cout << "Uso: " << argv[0] << " <archivo entrada (texto concatenado T)>" << endl;
    return 1;
  }

  // ----------------------------------------------------------------------
  // 1. CAMBIO DE ESTRUCTURA COMPACTA (REQUISITO DEL PROYECTO)
  // ----------------------------------------------------------------------

  cout << "Construyendo el FM-index con csa_wt<wt_int<rrr_vector<>>> ..." << endl;
  
  csa_wt<wt_int<rrr_vector<>>> fm_index;
  construct(fm_index, argv[1], 1);
  
  cout << "Tamaño del FM-index " << size_in_mega_bytes(fm_index) << " MB." << endl;

  // ----------------------------------------------------------------------
  // 2. LÓGICA AUXILIAR PARA DOC_LOCATE (CARGA DE POSICIONES REALES)
  // ----------------------------------------------------------------------
  sdsl::int_vector<> start_positions;
  std::vector<size_t> temp_positions;
  size_t pos_val;

  // Cargamos el archivo de posiciones generado por el script preprocesar.py
  std::ifstream pos_file("posiciones.txt");
  if (!pos_file.is_open()) {
      cerr << "ERROR: No se pudo abrir posiciones.txt. Asegúrese de que existe en este directorio." << endl;
      return 1;
  }

  // Leemos cada número del archivo y lo guardamos
  while (pos_file >> pos_val) {
      temp_positions.push_back(pos_val);
  }

  if (temp_positions.empty()) {
      cerr << "ERROR: El archivo posiciones.txt está vacío o mal formateado." << endl;
      return 1;
  }

  size_t num_docs = temp_positions.size();
  start_positions.resize(num_docs); 
  std::copy(temp_positions.begin(), temp_positions.end(), start_positions.begin()); 
  
  cout << "Se cargaron " << num_docs << " posiciones de inicio de documentos." << endl;
  // ----------------------------------------------------------------------

    string patron;
    cout << "\nIngrese un patrón a buscar: ";
    cin >> patron;

    // Buscando las ocurrencias del patrón
    size_t occs = sdsl::count(fm_index, patron.begin(), patron.end());

    cout << "# de ocurrencias: " << occs << endl;
    
    if (occs > 0) {
      // ----------------------------------------------------------------------
      // 3. IMPLEMENTACIÓN DE DOC_LOCATE
      // ----------------------------------------------------------------------
      
      auto posiciones = sdsl::locate(fm_index, patron.begin(), patron.end());
      std::set<size_t> documentos_encontrados; 
      size_t doc_id; 
      
      for (size_t i = 0; i < posiciones.size(); ++i) {
	    size_t pos_en_T = posiciones[i];
	    
	    doc_id = map_position_to_document_id(pos_en_T, start_positions);
        
        if (doc_id >= 1 && doc_id <= num_docs) {
            documentos_encontrados.insert(doc_id);
        }
      }

      cout << "Las ocurrencias se encuentran en los documentos (doc_locate): " << endl;
      
      for (size_t doc_id_reporte : documentos_encontrados) {
	    cout << "D" << doc_id_reporte << endl;
      }
    }
    
    return 0;
}
