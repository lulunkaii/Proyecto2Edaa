// Construcción del suffix array, la BWT, y búsqueda de patrones por Búsqueda Binaria.
// Incluye la funcionalidad doc_locate para reportar documentos únicos.
//
// Prerrequisitos: Tener la biblioteca SDSL instalada
//
// Compilación: g++ -std=c++11 -DNDEBUG -O3 \
//    -I/home/vt/include -L/home/vt/lib \
//    sa.cpp -o sa -lsdsl -ldivsufsort -ldivsufsort64

#include <sdsl/suffix_arrays.hpp>
#include <sdsl/util.hpp> // Incluye size_in_mega_bytes y sdsl::util::assign
#include <string>
#include <iostream>
#include <algorithm> // Necesario para std::copy
#include <set> // Para almacenar documentos únicos
#include <fstream> // Para cargar el archivo de posiciones
#include <vector> // Para cargar datos temporalmente

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

/**
 * Implementa la función de comparación lexicográfica entre el patrón P y
 * un sufijo T[sa[i]..] para usar en la búsqueda binaria.
 */
int compare_pattern_suffix(size_t sa_pos, const string& P, const int_vector<>& seq) {
    size_t m = P.size();
    size_t N = seq.size();
    
    for (size_t i = 0; i < m; ++i) {
        if (sa_pos + i >= N) { 
            return 1;
        }
        if (P[i] < seq[sa_pos + i]) {
            return -1;
        } else if (P[i] > seq[sa_pos + i]) {
            return 1;
        }
    }
    return 0;
}


int main(int argc, char** argv) {
  if (argc !=  2) {
    cout << "Uso: " << argv[0] << " <archivo entrada (texto concatenado T)>" << endl;
    return 1;
  }

  string infile(argv[1]);
  int_vector<> seq;
  int32_t n;
  {
    load_vector_from_file(seq, infile, 1);
    n = seq.size();
    
    seq.resize(n+1);
    n = seq.size();
    seq[n-1] = 0; // Representa el final de texto.
  }

  // ----------------------------------------------------------------------
  // 1. CONSTRUCCIÓN DE SA y BWT
  // ----------------------------------------------------------------------

  cout << "Construyendo el Suffix array ..." << endl;
  
  int_vector<> sa(1, 0, bits::hi(n)+1);
  sa.resize(n);
  // Usa el algoritmo de construcción SDSL (tiempo lineal)
  algorithm::calculate_sa((const unsigned char*)seq.data(), n, sa);
  
  cout << "Tamaño del SA " << size_in_mega_bytes(sa) << " MB." << endl;
  
  // ----------------------------------------------------------------------
  // 2. LÓGICA AUXILIAR PARA DOC_LOCATE (CARGA DE POSICIONES REALES)
  // ----------------------------------------------------------------------
  sdsl::int_vector<> start_positions;
  std::vector<size_t> temp_positions;
  size_t pos_val;

  std::ifstream pos_file("posiciones.txt");
  if (!pos_file.is_open()) {
      cerr << "ERROR: No se pudo abrir posiciones.txt. Asegúrese de que existe en este directorio." << endl;
      return 1;
  }

  while (pos_file >> pos_val) {
      temp_positions.push_back(pos_val);
  }

  if (temp_positions.empty()) {
      cerr << "ERROR: El archivo posiciones.txt está vacío o mal formateado." << endl;
      return 1;
  }

  // CORRECCIÓN FINAL: Redimensionar y usar std::copy
  size_t num_docs = temp_positions.size();
  start_positions.resize(num_docs); 
  std::copy(temp_positions.begin(), temp_positions.end(), start_positions.begin()); 
  
  cout << "Se cargaron " << num_docs << " posiciones de inicio de documentos." << endl;
  // ----------------------------------------------------------------------

  string patron;
  cout << "\nIngrese un patrón a buscar: ";
  cin >> patron;
  size_t m  = patron.size();

  // ----------------------------------------------------------------------
  // 3. BÚSQUEDA BINARIA (PARA ENCONTRAR EL RANGO [L, R])
  // ----------------------------------------------------------------------
  
  size_t L = 0;             
  size_t R = sa.size() - 1; 
  size_t L_bound = sa.size(); 
  
  // BÚSQUEDA PARA LÍMITE INFERIOR (L): Primer sufijo >= P
  size_t l_temp = L, r_temp = R;
  while (l_temp <= r_temp) {
      size_t mid = l_temp + (r_temp - l_temp) / 2;
      int cmp = compare_pattern_suffix(sa[mid], patron, seq);
      
      if (cmp <= 0) { 
          L_bound = mid;
          r_temp = mid - 1;
      } else { 
          l_temp = mid + 1;
      }
  }

  L = L_bound; 

  // BÚSQUEDA PARA LÍMITE SUPERIOR (R): Primer sufijo > P
  L_bound = sa.size(); 
  l_temp = L, r_temp = R; 
  while (l_temp <= r_temp) {
      size_t mid = l_temp + (r_temp - l_temp) / 2;
      int cmp = compare_pattern_suffix(sa[mid], patron, seq);
      
      if (cmp < 0) { 
          L_bound = mid;
          r_temp = mid - 1;
      } else { 
          l_temp = mid + 1;
      }
  }

  R = L_bound - 1; 

  // ----------------------------------------------------------------------
  // 4. REPORTE DE RESULTADOS (DOC_LOCATE)
  // ----------------------------------------------------------------------
  
  size_t occs = R - L + 1;
  if (R < L) {
      occs = 0; 
  }

  cout << "# de ocurrencias (rango SA): " << occs << endl;
  
  if (occs > 0) {
      std::set<size_t> documentos_encontrados; 
      size_t doc_id; 

      for (size_t i = L; i <= R; ++i) {
	    size_t pos_en_T = sa[i];
	    
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
