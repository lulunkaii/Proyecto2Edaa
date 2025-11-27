#!/bin/bash

# Este script automatiza la ejecución de sa3 y fmi3 con el dataset 'proteins5MB_*.xml'.

# Nombre del archivo de resultados.
RESULT_FILE="resultados_experimentales3.csv"

# Nombre de tu archivo de texto concatenado (generado por preprocesar3.py)
TEXTO_CONCATENADO="corpus_T3.txt"

# Número de repeticiones requeridas para cada patrón
REPETICIONES=30

# Definición de los 6 patrones de prueba (manteniendo el mismo set de prueba)
PATRONES=(
    "<title>"
    "databa"
    "algorithm"
    "computational"
    "XXYZ123abcXYZ"
    "article$"
)

# -----------------------------------------------------------
# PREPARACIÓN
# -----------------------------------------------------------

# Crear el archivo de resultados con el nuevo encabezado
echo "Estructura,Patron,Repeticion,Tiempo_Construccion_ms,Tiempo_Busqueda_ms,Num_Ocurrencias,Documentos_Encontrados" > $RESULT_FILE

# Limpiar los archivos de salida raw antes de empezar
> salida_SA_raw3.txt
> salida_FMI_raw3.txt

echo "--- Iniciando pruebas para los 6 patrones (${REPETICIONES} repeticiones c/u) en corpus_T3.txt ---"

# -----------------------------------------------------------
# BUCLE PRINCIPAL DE EXPERIMENTACIÓN
# -----------------------------------------------------------

for PATRON in "${PATRONES[@]}"; do
    echo "--- Iniciando pruebas para el patrón: $PATRON ---"
    
    for i in $(seq 1 $REPETICIONES); do
        
        # PRUEBA SA (usando ./sa3)
        SA_FULL_OUTPUT=$(echo "$PATRON" | ./sa3 "$TEXTO_CONCATENADO" 2>&1)
        SA_METRICS=$(echo "$SA_FULL_OUTPUT" | tail -n 1 | sed 's/.*: //') 

        echo "--- SA Repetición $i - Patron $PATRON ---" >> salida_SA_raw3.txt
        echo "$SA_FULL_OUTPUT" >> salida_SA_raw3.txt
        
        if echo "$SA_METRICS" | grep -q ","; then
            echo "SA,$PATRON,$i,$SA_METRICS" >> $RESULT_FILE
        fi

        # PRUEBA FM-INDEX (usando ./fmi3)
        FM_FULL_OUTPUT=$(echo "$PATRON" | ./fmi3 "$TEXTO_CONCATENADO" 2>&1)
        FM_METRICS=$(echo "$FM_FULL_OUTPUT" | tail -n 1 | sed 's/.*: //') 

        echo "--- FMI Repetición $i - Patron $PATRON ---" >> salida_FMI_raw3.txt
        echo "$FM_FULL_OUTPUT" >> salida_FMI_raw3.txt

        if echo "$FM_METRICS" | grep -q ","; then
            echo "FMI,$PATRON,$i,$FM_METRICS" >> $RESULT_FILE
        fi
        
    done
done

echo "--- ¡Pruebas de experimentación 3 finalizadas! Resultados en $RESULT_FILE ---"
