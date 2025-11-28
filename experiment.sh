#!/bin/bash

# Parámetros de experimento
RUNS=32                 # Número de repeticiones por longitud, después se promedian
MIN_LEN=5               # Longitud mínima de patrón
MAX_LEN=1000            # Longitud máxima de patrón
STEP=5                  # Incremento de longitud de patrón

# Carpetas de output
RES_DIR="resultados"
LOG_DIR="logs"
DATA_OUT_DIR="datos_procesados"

# Crea directorios si no existen
echo "--- Creando directorios: $RES_DIR, $LOG_DIR, $DATA_OUT_DIR ---"
mkdir -p "$RES_DIR"
mkdir -p "$LOG_DIR"
mkdir -p "$DATA_OUT_DIR"

echo "=== Compilando ==="
# Compilamos usando el macro USE_SA para diferenciar
g++ -O0 -DUSE_SA -o uhr_sa uhr.cpp -lsdsl -ldivsufsort -ldivsufsort64
g++ -O0 -o uhr_fm uhr.cpp -lsdsl -ldivsufsort -ldivsufsort64

# Función para ejecutar un dataset completo
run_dataset() {
                 # ejemplos:
    NAME=$1      # sources
    DIR=$2       # datasets/sources/
    PREFIX=$3    # sources5MB_

    echo "------------------------------------------------"
    echo " Dataset: $NAME"
    echo "------------------------------------------------"

    # Ejecuta preprocesar para generar corpus y posiciones
    python3 preprocesar.py "$DIR" "$PREFIX"

    # Mueve archivos a DATA_OUT_DIR con nombres de cada dataset
    CORPUS="$DATA_OUT_DIR/corpus_${NAME}.txt"
    POS="$DATA_OUT_DIR/posiciones_${NAME}.txt"
    
    mv corpus_T.txt "$CORPUS"
    mv posiciones.txt "$POS"

    # Ejecuta Suffix Array
    # - CSV va a RES_DIR
    # - Inputs vienen de DATA_OUT_DIR
    # - Log se redirige a LOG_DIR
    echo "  >> Ejecutando SA..."
    ./uhr_sa "$RES_DIR/resultados_${NAME}_sa.csv" $RUNS $MIN_LEN $MAX_LEN $STEP "$CORPUS" "$POS" "${NAME}_SA" > "$LOG_DIR/log_${NAME}_sa.txt"

    # Ejecuta FM-Index
    echo "  >> Ejecutando FM-Index..."
    ./uhr_fm "$RES_DIR/resultados_${NAME}_fm.csv" $RUNS $MIN_LEN $MAX_LEN $STEP "$CORPUS" "$POS" "${NAME}_FM" > "$LOG_DIR/log_${NAME}_fm.txt"

    echo "  Terminado $NAME. Logs guardados en $LOG_DIR/."
}

# --- EJECUCIÓN ---

# Dataset 1: sources
run_dataset "sources" "datasets/sources/" "sources5MB_"

# Dataset 2: proteins
run_dataset "proteins" "datasets/proteins/" "proteins5MB_"

# Dataset 3: dblp
run_dataset "dblp" "datasets/dblp/" "dblp5MB_"

echo "=== EXPERIMENTOS COMPLETADOS EXITOSAMENTE ==="
echo "Resultados en: $RES_DIR/"
echo "Logs en:       $LOG_DIR/"
echo "Datos en:      $DATA_OUT_DIR/"