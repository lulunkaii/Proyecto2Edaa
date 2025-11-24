// Construcción del suffix array, la BWT, y búsqueda de patrones por Búsqueda Binaria.
// Incluye la funcionalidad doc_locate para reportar documentos únicos.
//
// Prerrequisitos: Tener la biblioteca SDSL instalada
//
// Compilación: g++ -std=c++11 -DNDEBUG -O3 \
//    -I/home/vt/include -L/home/vt/lib \
//    sa.cpp -o sa -lsdsl -ldivsufsort -ldivsufsort64

#include <sdsl/suffix_arrays.hpp>
#include <sdsl/util.hpp> // Incluye size_in_mega_bytes
#include <string>
#include <iostream>
#include <algorithm>
#include <set> // Para almacenar documentos únicos

using namespace sdsl;
using namespace std;

/**
 * Función auxiliar para mapear una posición en el texto concatenado (T)
 * al ID del documento (D1, D2, D3, etc.) al que pertenece.
 * @param pos La posición de ocurrencia en T (0-based).
 * @param starts Vector con las posiciones de inicio de cada documento en T.
 * @return El ID del documento (1-based, es decir, 1 para D1, 2 para D2, etc.).
 */
size_t map_position_to_document_id(size_t pos, const sdsl::int_vector<>& starts) {
    // upper_bound encuentra la primera posición de inicio que es estrictamente mayor que 'pos'.
    // La distancia desde el inicio (starts.begin()) hasta este elemento nos da el ID del documento (1-based).
    auto it = std::upper_bound(starts.begin(), starts.end(), pos);
    
    // El ID del documento (1-based).
    size_t doc_id = std::distance(starts.begin(), it);
    
    // Control de límites (si la posición es la última y está en el último documento)
    if (doc_id > starts.size() && !starts.empty()) {
        return starts.size();
    }
    
    return doc_id;
}

/**
 * Implementa la función de comparación lexicográfica entre el patrón P y
 * un sufijo T[sa[i]..] para usar en la búsqueda binaria.
 * @param sa_pos La posición de inicio del sufijo en el texto T.
 * @param P El patrón de búsqueda.
 * @param seq El texto concatenado T.
 * @return -1 si P < T[sa_pos..], 0 si P es prefijo/igual a T[sa_pos..], 1 si P > T[sa_pos..]
 */
int compare_pattern_suffix(size_t sa_pos, const string& P, const int_vector<>& seq) {
    size_t m = P.size();
    size_t N = seq.size();
    
    for (size_t i = 0; i < m; ++i) {
        if (sa_pos + i >= N) { 
            // El sufijo es más corto que P. P es lexicográficamente mayor.
            return 1;
        }
        if (P[i] < seq[sa_pos + i]) {
            // P es menor que el sufijo.
            return -1;
        } else if (P[i] > seq[sa_pos + i]) {
            // P es mayor que el sufijo.
            return 1;
        }
    }
    // Si llegamos aquí, P es un prefijo del sufijo o es igual. P es <= sufijo.
    return 0;
}


int main(int argc, char** argv) {
  if (argc !=  2) {
    cout << "Uso: " << argv[0] << " <archivo entrada (texto concatenado T)>" << endl;
    return 1;
  }

  // Leemos el archivo de entrada (debe ser el texto T concatenado)
  string infile(argv[1]);
  
  int_vector<> seq;
  int32_t n;
  {
    // Carga el texto
    load_vector_from_file(seq, infile, 1);
    n = seq.size();
    
    // Añade el caracter de fin de texto ('$' o 0) al final
    seq.resize(n+1);
    n = seq.size();
    seq[n-1] = 0; // Representa el final de texto.
  }

  // ----------------------------------------------------------------------
  // 1. CONSTRUCCIÓN DE SA y BWT
  // ----------------------------------------------------------------------

  cout << "Construyendo el Suffix array ... " << endl;
  
  int_vector<> sa(1, 0, bits::hi(n)+1);
  sa.resize(n);
  // Usa el algoritmo de construcción SDSL (tiempo lineal)
  algorithm::calculate_sa((const unsigned char*)seq.data(), n, sa);
  
  cout << "Construyendo la BWT ..." << endl;
  // BWT es simplemente BWT[i] = T[SA[i]-1]
  int_vector<> bwt(n);
  algorithm::calculate_bwt((const unsigned char*)seq.data(), n, sa, bwt); 

  cout << "Tamaño del SA " << size_in_mega_bytes(sa) << " MB." << endl;
  cout << "Tamaño de la BWT " << size_in_mega_bytes(bwt) << " MB." << endl;
  
  // ----------------------------------------------------------------------
  // 2. LÓGICA AUXILIAR PARA DOC_LOCATE
  // ----------------------------------------------------------------------
  
  // SECCIÓN CRÍTICA DE DATOS: Debe cargar las posiciones reales de inicio.
  // Ejemplo SIMULADO (Asume 10 documentos de ~5MB cada uno + separador)
  size_t num_docs = 10;
  sdsl::int_vector<> start_positions(num_docs); 
  start_positions[0] = 0; // Documento D1 inicia en 0
  
  // REEMPLAZAR esta lógica de inicialización SIMULADA
  // El tamaño real de cada documento (incluyendo el separador '$') DEBE ser calculado
  // a partir de la pre-concatenación.
  size_t simulated_doc_size = (n / num_docs); 
  for (size_t i = 1; i < num_docs; ++i) {
      start_positions[i] = start_positions[i-1] + simulated_doc_size;
  }
  // ----------------------------------------------------------------------

  string patron;
  cout << "\nIngrese un patrón a buscar: ";
  cin >> patron;
  size_t m  = patron.size();

  // ----------------------------------------------------------------------
  // 3. BÚSQUEDA BINARIA (PARA ENCONTRAR EL RANGO [L, R])
  // ----------------------------------------------------------------------
  
  size_t L = 0;             // Límite inferior del rango de búsqueda
  size_t R = sa.size() - 1; // Límite superior del rango de búsqueda
  size_t L_bound = sa.size(); // Primer índice i tal que T[sa[i]..] >= P (Lower Bound)
  
  // BÚSQUEDA PARA LÍMITE INFERIOR (L): Primer sufijo >= P
  size_t l_temp = L, r_temp = R;
  while (l_temp <= r_temp) {
      size_t mid = l_temp + (r_temp - l_temp) / 2;
      int cmp = compare_pattern_suffix(sa[mid], patron, seq);
      
      if (cmp <= 0) { // T[sa[mid]..] >= P
          L_bound = mid;
          r_temp = mid - 1;
      } else { // T[sa[mid]..] < P
          l_temp = mid + 1;
      }
  }

  L = L_bound; // L es el índice del primer sufijo que coincide (o no se encuentra)

  // BÚSQUEDA PARA LÍMITE SUPERIOR (R): Primer sufijo > P
  L_bound = sa.size(); // Primer índice i tal que T[sa[i]..] > P (Upper Bound)
  l_temp = L, r_temp = R; // El rango de búsqueda puede reducirse a [L, R]
  while (l_temp <= r_temp) {
      size_t mid = l_temp + (r_temp - l_temp) / 2;
      int cmp = compare_pattern_suffix(sa[mid], patron, seq);
      
      if (cmp < 0) { // T[sa[mid]..] > P
          L_bound = mid;
          r_temp = mid - 1;
      } else { // T[sa[mid]..] <= P
          l_temp = mid + 1;
      }
  }

  R = L_bound - 1; // R es el índice del último sufijo que coincide

  // ----------------------------------------------------------------------
  // 4. REPORTE DE RESULTADOS (DOC_LOCATE)
  // ----------------------------------------------------------------------
  
  size_t occs = R - L + 1;
  if (R < L) {
      occs = 0; // No se encontró el patrón
  }

  cout << "# de ocurrencias (rango SA): " << occs << endl;
  
  if (occs > 0) {
      // Usamos std::set para almacenar los IDs de documentos únicos (D1, D2, D3, ...)
      std::set<size_t> documentos_encontrados; 
      
      // Iterar sobre las posiciones de ocurrencia en el rango [L, R] del SA
      for (size_t i = L; i <= R; ++i) {
	    size_t pos_en_T = sa[i];
	    
        // Mapear la posición de ocurrencia al ID del documento
	    size_t doc_id = map_position_to_document_id(pos_en_T, start_positions);
        
        if (doc_id >= 1) { // Asegura que no se inserten IDs inválidos
            documentos_encontrados.insert(doc_id);
        }
      }

      cout << "Las ocurrencias se encuentran en los documentos (doc_locate): " << endl;
      
      for (size_t doc_id : documentos_encontrados) {
	    cout << "D" << doc_id << endl;
      }
  }
    
  return 0;
}
