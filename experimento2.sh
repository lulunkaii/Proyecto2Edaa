#!/bin/bash

# Este script automatiza la ejecución de sa y fmi con el dataset 'sources_5MB.xml'.

# Nombre del archivo de resultados.
RESULT_FILE="resultados_experimentales2.csv"

# Nombre de tu archivo de texto concatenado (generado por preprocesar2.py)
TEXTO_CONCATENADO="corpus_T2.txt"

# Número de repeticiones requeridas para cada patrón
REPETICIONES=30

# Definición de los 6 patrones de prueba (los mismos que el original)
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
> salida_SA_raw2.txt
> salida_FMI_raw2.txt

echo "--- Iniciando pruebas para los 6 patrones (${REPETICIONES} repeticiones c/u) en corpus_T2.txt ---"

# -----------------------------------------------------------
# BUCLE PRINCIPAL DE EXPERIMENTACIÓN
# -----------------------------------------------------------

for PATRON in "${PATRONES[@]}"; do
    echo "--- Iniciando pruebas para el patrón: $PATRON ---"
    
    for i in $(seq 1 $REPETICIONES); do
        
        # -----------------------------------------------------------\
        # PRUEBA SA (Asegúrate de que 'sa' y 'fmi' estén compilados para usar posiciones2.txt)
        # -----------------------------------------------------------
        
        # Ejecutamos el programa. 2>&1 asegura que la salida del 'prompt' sea ignorada o capturada
        SA_FULL_OUTPUT=$(echo "$PATRON" | ./sa "$TEXTO_CONCATENADO" 2>&1)
        
        # Capturamos la ÚLTIMA LÍNEA (la línea CSV). Usa SED para eliminar el prompt 'Ingrese un patrón a buscar: '
        SA_METRICS=$(echo "$SA_FULL_OUTPUT" | tail -n 1 | sed 's/.*: //') 

        # Guardamos la salida completa para depuración
        echo "--- SA Repetición $i - Patron $PATRON ---" >> salida_SA_raw2.txt
        echo "$SA_FULL_OUTPUT" >> salida_SA_raw2.txt
        
        # Escribir la línea final en el CSV
        if echo "$SA_METRICS" | grep -q ","; then
            echo "SA,$PATRON,$i,$SA_METRICS" >> $RESULT_FILE
        fi

        # -----------------------------------------------------------
        # PRUEBA FM-INDEX
        # -----------------------------------------------------------
        
        FM_FULL_OUTPUT=$(echo "$PATRON" | ./fmi "$TEXTO_CONCATENADO" 2>&1)
        
        FM_METRICS=$(echo "$FM_FULL_OUTPUT" | tail -n 1 | sed 's/.*: //') 

        echo "--- FMI Repetición $i - Patron $PATRON ---" >> salida_FMI_raw2.txt
        echo "$FM_FULL_OUTPUT" >> salida_FMI_raw2.txt

        if echo "$FM_METRICS" | grep -q ","; then
            echo "FMI,$PATRON,$i,$FM_METRICS" >> $RESULT_FILE
        fi
        
    done
done

echo "--- ¡Pruebas de experimentación 2 finalizadas! Resultados en $RESULT_FILE ---"
