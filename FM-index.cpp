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
#include <set> // Para almacenar documentos únicos
#include <sdsl/rrr_vector.hpp> // Nuevo bloque de construcción requerido

using namespace sdsl;
using namespace std;

/**
 * Función auxiliar para mapear una posición en el texto concatenado (T)
 * al ID del documento (D1, D2, D3, etc.) al que pertenece.
 * * @param pos La posición de ocurrencia en T (0-based).
 * @param starts Vector con las posiciones de inicio de cada documento en T.
 * @return El ID del documento (1-based, es decir, 1 para D1, 2 para D2, etc.).
 */
size_t map_position_to_document_id(size_t pos, const sdsl::int_vector<>& starts) {
    // upper_bound encuentra el primer elemento en 'starts' que es estrictamente mayor que 'pos'.
    // La distancia desde el inicio (starts.begin()) hasta este elemento 'it'
    // nos da el ID del documento (1-based, D1, D2, etc.).
    // Ejemplo: Si pos=5 y starts={0, 100, 200}, upper_bound apunta a 100.
    // La distancia es 1, lo que corresponde a D1.
    auto it = std::upper_bound(starts.begin(), starts.end(), pos);
    
    // El ID del documento (1-based)
    size_t doc_id = std::distance(starts.begin(), it);
    
    // En caso de que la búsqueda falle (nunca debería pasar si pos >= starts[0]=0),
    // o si la posición excede el tamaño total del texto T, ajustamos el ID al último documento.
    if (doc_id > starts.size()) {
        return starts.size(); // Retorna el ID del último documento.
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
  
  // Una versión compacta del suffix array (FM-index), implementada en SDSL.
  // Cambiado de csa_wt<wt_int<>> a csa_wt<wt_int<rrr_vector<>>>
  // para cumplir con el requisito del proyecto.
  csa_wt<wt_int<rrr_vector<>>> fm_index;
  construct(fm_index, argv[1], 1);
  
  cout << "Tamaño del FM-index " << size_in_mega_bytes(fm_index) << " MB." << endl;

  // ----------------------------------------------------------------------
  // 2. LÓGICA AUXILIAR PARA DOC_LOCATE
  // ----------------------------------------------------------------------
  
  // *** INICIO DE LA SECCIÓN CRÍTICA DE DATOS ***
  // ** DEBE REEMPLAZAR ESTA INICIALIZACIÓN CON LA LÓGICA DE CARGA/GENERACIÓN REAL **
  // Este vector DEBE contener las posiciones de inicio (0-based) de cada documento 
  // en el texto concatenado T.
  // Ejemplo: Si D1 tiene tamaño 100, D2 tiene tamaño 50, y el separador es 1 byte.
  // starts = {0, 101, 152, ...}
  
  sdsl::int_vector<> start_positions(10); // Asumiendo 10 documentos (D1 a D10)
  start_positions[0] = 0; // Documento 1 (D1) inicia en 0
  
  // ** EJEMPLO SIMULADO (REEMPLAZAR) **
  // Asume que cada documento tiene un tamaño de 100 caracteres + 1 separador ($).
  for (int i = 1; i < 10; ++i) {
      start_positions[i] = start_positions[i-1] + 101; // Posición de inicio de Di+1
  }
  // ** FIN DEL EJEMPLO SIMULADO **
  // ----------------------------------------------------------------------

    string patron;
    cout << "Ingrese un patrón a buscar: ";
    cin >> patron;

    // Buscando las ocurrencias del patrón
    size_t occs = sdsl::count(fm_index, patron.begin(), patron.end());

    cout << "# de ocurrencias: " << occs << endl;
    
    if (occs > 0) {
      // ----------------------------------------------------------------------
      // 3. IMPLEMENTACIÓN DE DOC_LOCATE
      // ----------------------------------------------------------------------
      
      // Obtener todas las posiciones de ocurrencia
      auto posiciones = sdsl::locate(fm_index, patron.begin(), patron.end());
      
      // Usamos std::set para almacenar los IDs de documentos únicos (D1, D2, D3, ...)
      // Esto implementa la funcionalidad doc_locate(T, p)
      std::set<size_t> documentos_encontrados; 

      for (size_t i = 0; i < posiciones.size(); ++i) {
	    size_t pos_en_T = posiciones[i];
	    
        // Mapear la posición de ocurrencia al ID del documento
	    size_t doc_id = map_position_to_document_id(pos_en_T, start_positions);
        
        // El ID del documento debe ser mayor o igual a 1 (D1)
        if (doc_id >= 1) {
            documentos_encontrados.insert(doc_id);
        }
      }

      cout << "Las ocurrencias se encuentran en los documentos (doc_locate): " << endl;
      
      for (size_t doc_id : documentos_encontrados) {
	    cout << "D" << doc_id << endl;
      }

      // Opcionalmente, imprimir todas las posiciones ordenadas (como el código base original)
      // sort(posiciones.begin(), posiciones.end());
      // cout << "\nLas ocurrencias comienzan en las siguientes posiciones: " << endl;
      // for (size_t i = 0; i < posiciones.size(); ++i) {
      // 	cout << posiciones[i] << endl;
      // }
    }
    
    return 0;
}
