import os
import sys

# Uso: python3 preprocesar.py <carpeta_dataset> <prefijo_archivo>

if len(sys.argv) < 3:
    print("Uso: python3 preprocesar.py <carpeta_dataset> <prefijo_archivo>")
    sys.exit(1)

DATASET_FOLDER = sys.argv[1]
FILE_PREFIX = sys.argv[2]

OUTPUT_TEXT_FILE = "corpus_T.txt"
OUTPUT_POS_FILE = "posiciones.txt"
DELIMITER = "$" 
NUM_DOCS = 10

if not DATASET_FOLDER.endswith('/'):
    DATASET_FOLDER += '/'

start_positions = [0] 

with open(OUTPUT_TEXT_FILE, 'wb') as t_file:
    print(f"Procesando {DATASET_FOLDER}...")
    
    for i in range(1, NUM_DOCS + 1):
        found = False
        target_base = f"{FILE_PREFIX}{i}"
        
        # buscamos xml, txt, cualquiera
        for file in os.listdir(DATASET_FOLDER):
            if file.startswith(target_base):
                filename = os.path.join(DATASET_FOLDER, file)
                found = True
                break
        
        if not found:
            print(f"Error: No se encontró patrón {target_base} en {DATASET_FOLDER}")
            continue

        with open(filename, 'rb') as doc_file:
            content = doc_file.read()
            t_file.write(content)
            
            if i < NUM_DOCS:
                delimiter_byte = DELIMITER.encode('ascii')
                t_file.write(delimiter_byte)
                next_pos = t_file.tell()
                start_positions.append(next_pos)

with open(OUTPUT_POS_FILE, 'w') as pos_file:
    for pos in start_positions:
        pos_file.write(str(pos) + "\n")

print("Generados corpus_T.txt y posiciones.txt correctamente.")