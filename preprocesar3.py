import os

# --- Configuración 3 ---
DATA_DIR = "datasets/proteins/"
OUTPUT_TEXT_FILE = "corpus_T3.txt"
OUTPUT_POS_FILE = "posiciones3.txt"
DELIMITER = "$"
NUM_DOCS = 10
# -----------------------

start_positions = [0] 

with open(OUTPUT_TEXT_FILE, 'wb') as t_file:
    print(f"Concatenando archivos en {OUTPUT_TEXT_FILE}...")
    
    for i in range(1, NUM_DOCS + 1):
        # NOTA: Ajustar el nombre del archivo de entrada
        filename = f"{DATA_DIR}proteins5MB_{i}.xml" 
        
        if not os.path.exists(filename):
            print(f"Error: Archivo no encontrado: {filename}")
            continue

        print(f"  > Procesando {filename}...")
        
        with open(filename, 'rb') as doc_file:
            content = doc_file.read()
            t_file.write(content)
            
            if i < NUM_DOCS:
                delimiter_byte = DELIMITER.encode('ascii')
                t_file.write(delimiter_byte)
                
                next_pos = t_file.tell() 
                start_positions.append(next_pos)
                
                print(f"    - Documento {i} concatenado. D{i+1} inicia en byte {next_pos}.")

with open(OUTPUT_POS_FILE, 'w') as pos_file:
    print(f"\nGenerando archivo de posiciones en {OUTPUT_POS_FILE} con {len(start_positions)} documentos...")
    for pos in start_positions:
        pos_file.write(str(pos) + "\n")

print("\n¡Preprocesamiento 3 completado exitosamente!")
