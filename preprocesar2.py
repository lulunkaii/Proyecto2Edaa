import os

# --- Configuración 2 (Actualizada) ---
DATA_DIR = "datasets/sources/"
OUTPUT_TEXT_FILE = "corpus_T2.txt"
OUTPUT_POS_FILE = "posiciones2.txt"
DELIMITER = "$" # Carácter separador entre documentos
NUM_DOCS = 10     # ¡Ahora procesamos 10 documentos!
# -------------------------------------

# Lista para almacenar las posiciones de inicio de cada documento
# El primer documento siempre inicia en la posición 0
start_positions = [0] 

# Usaremos 'wb' (write binary) para garantizar un conteo de bytes preciso
with open(OUTPUT_TEXT_FILE, 'wb') as t_file:
    print(f"Concatenando archivos en {OUTPUT_TEXT_FILE}...")
    
    # Bucle para procesar los 10 archivos: sources5MB_1.xml hasta sources5MB_10.xml
    for i in range(1, NUM_DOCS + 1):
        filename = f"{DATA_DIR}sources5MB_{i}.xml"
        
        if not os.path.exists(filename):
            print(f"Error: Archivo no encontrado: {filename}")
            continue

        print(f"  > Procesando {filename}...")
        
        with open(filename, 'rb') as doc_file:
            content = doc_file.read()
            t_file.write(content)
            
            # Si no es el último documento, agregamos el separador
            if i < NUM_DOCS:
                # El separador se escribe como byte
                delimiter_byte = DELIMITER.encode('ascii')
                t_file.write(delimiter_byte)
                
                # La posición de inicio del siguiente documento es la posición actual del puntero de archivo
                next_pos = t_file.tell() 
                
                # Registramos la posición de inicio del DOCUMENTO i+1
                start_positions.append(next_pos)
                
                print(f"    - Documento {i} concatenado. D{i+1} inicia en byte {next_pos}.")

# Guardamos las posiciones de inicio en un archivo de texto simple
with open(OUTPUT_POS_FILE, 'w') as pos_file:
    print(f"\nGenerando archivo de posiciones en {OUTPUT_POS_FILE} con {len(start_positions)} documentos...")
    for pos in start_positions:
        pos_file.write(str(pos) + "\n")

print("\n¡Preprocesamiento 2 completado exitosamente!")
