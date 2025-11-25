#!/bin/bash

# Nombre del archivo de resultados. Usamos un formato CSV para facilitar el análisis.
RESULT_FILE="resultados_experimentales.csv"

# Nombre de tu archivo de texto concatenado
TEXTO_CONCATENADO="corpus_T.txt"

# Patrón de prueba a analizar (EJEMPLO: Cambia esto para cada ejecución)
PATRON="databa"

# Número de repeticiones requeridas
REPETICIONES=30

# Crear el archivo de resultados con los encabezados (si no existe)
if [ ! -f "$RESULT_FILE" ]; then
    echo "Estructura,Patron,Repeticion,Tiempo_Construccion_Real,Num_Ocurrencias,Documentos_Encontrados" > $RESULT_FILE
fi

echo "--- Iniciando pruebas para el patrón: $PATRON ---"

for i in $(seq 1 $REPETICIONES); do
    echo "Ejecutando repetición $i de $REPETICIONES..."
    
    # -----------------------------------------------------------
    # PRUEBA SA
    # time -f captura el tiempo real de ejecucion (Construcción + Búsqueda)
    # -----------------------------------------------------------
    
    # La salida incluye el tiempo real de ejecucion (Time) y el output del programa (SA Output)
    SA_OUTPUT=$( { time echo "$PATRON" | ./sa "$TEXTO_CONCATENADO"; } 2>&1 )
    
    # Por simplicidad, añadimos toda la salida a un archivo separado, junto con el tiempo externo
    echo "--- SA Repetición $i - Patron $PATRON ---" >> salida_SA_raw.txt
    echo "$SA_OUTPUT" >> salida_SA_raw.txt
    
    # -----------------------------------------------------------
    # PRUEBA FM-INDEX
    # -----------------------------------------------------------
    
    FM_OUTPUT=$( { time echo "$PATRON" | ./fmi "$TEXTO_CONCATENADO"; } 2>&1 )

    echo "--- FMI Repetición $i - Patron $PATRON ---" >> salida_FMI_raw.txt
    echo "$FM_OUTPUT" >> salida_FMI_raw.txt
    
done

echo "--- Pruebas finalizadas. Resultados brutos guardados en salida_SA_raw.txt y salida_FMI_raw.txt ---"
