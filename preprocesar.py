import os

# --- Configuración ---
DATA_DIR = "datasets/dblp/"
OUTPUT_TEXT_FILE = "corpus_T.txt"
OUTPUT_POS_FILE = "posiciones.txt"
DELIMITER = "$" # Carácter separador entre documentos
NUM_DOCS = 10
# --------------------

# Lista para almacenar las posiciones de inicio de cada documento
# El primer documento siempre inicia en la posición 0
start_positions = [0] 

# Usaremos 'wb' (write binary) para asegurar que el tamaño sea exacto en bytes, aunque estemos leyendo texto (XML)
with open(OUTPUT_TEXT_FILE, 'wb') as t_file:
    print(f"Concatenando archivos en {OUTPUT_TEXT_FILE}...")
    
    for i in range(1, NUM_DOCS + 1):
        filename = f"{DATA_DIR}dblp5MB_{i}.xml"
        
        if not os.path.exists(filename):
            print(f"Error: Archivo no encontrado: {filename}")
            continue

        with open(filename, 'rb') as doc_file:
            content = doc_file.read()
            t_file.write(content)
            
            # Si no es el último documento, agregamos el separador
            if i < NUM_DOCS:
                # El separador se escribe como byte, asegurando que sea un solo byte
                delimiter_byte = DELIMITER.encode('ascii')
                t_file.write(delimiter_byte)
                
                # La posición de inicio del siguiente documento es:
                # (Tamaño actual del archivo) + (Tamaño del separador)
                next_pos = t_file.tell() # .tell() da el puntero actual (fin del delimitador)
                
                # Registramos la posición de inicio del DOCUMENTO i+1
                start_positions.append(next_pos)
                
                print(f"  > Documento {i} concatenado. D{i+1} inicia en byte {next_pos}.")

# Guardamos las posiciones de inicio en un archivo de texto simple
with open(OUTPUT_POS_FILE, 'w') as pos_file:
    print(f"\nGenerando archivo de posiciones en {OUTPUT_POS_FILE}...")
    for pos in start_positions:
        pos_file.write(str(pos) + "\n")

print("\n¡Preprocesamiento completado exitosamente!")
print(f"Total de documentos indexados: {len(start_positions)}")
