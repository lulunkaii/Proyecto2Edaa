import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

# --- Configuración ---
# Nombre del archivo de resultados generado por experimento.sh
RESULT_FILE = 'resultados_experimentales.csv' 
# Estilo de gráfico
plt.style.use('ggplot') 
# ---------------------

# 1. Cargar y Preprocesar los datos
try:
    df = pd.read_csv(RESULT_FILE)
except FileNotFoundError:
    print(f"Error: Asegúrate de que '{RESULT_FILE}' existe y no está vacío. ¿Ejecutaste 'bash experimento.sh'?")
    exit()
except pd.errors.EmptyDataError:
    print(f"Error: El archivo '{RESULT_FILE}' está vacío.")
    exit()

# 2. Conversión de Tipos y Escalado
# Convertir las columnas de métricas a numérico, manejando errores (coerce = convertir a NaN si falla)
try:
    # Las métricas están en 'ms', las convertiremos después.
    df['Tiempo_Construccion_ms'] = pd.to_numeric(df['Tiempo_Construccion_ms'], errors='coerce')
    df['Tiempo_Busqueda_ms'] = pd.to_numeric(df['Tiempo_Busqueda_ms'], errors='coerce')
    df['Num_Ocurrencias'] = pd.to_numeric(df['Num_Ocurrencias'], errors='coerce')
    df['Documentos_Encontrados'] = pd.to_numeric(df['Documentos_Encontrados'], errors='coerce')
except KeyError as e:
    print(f"Error: Columna {e} no encontrada en el CSV. Asegúrate de que el encabezado de '{RESULT_FILE}' es correcto.")
    exit()

# Eliminar filas con valores no numéricos que no se pudieron convertir
df = df.dropna(subset=['Tiempo_Construccion_ms', 'Tiempo_Busqueda_ms', 'Num_Ocurrencias', 'Documentos_Encontrados'])

# CONVERSIÓN DE MILISEGUNDOS A SEGUNDOS PARA LA CONSTRUCCIÓN (Tabla/Gráfico 1)
df['Tiempo_Construccion_s'] = df['Tiempo_Construccion_ms'] / 1000

# 3. Generar Tabla de Promedios (Agrupando por Estructura y Patrón)
# Se calculan las medias y la desviación estándar para la tabla/cálculos.
df_promedios = df.groupby(['Estructura', 'Patron'], as_index=False).agg(
    Tiempo_Construccion_s_mean=('Tiempo_Construccion_s', 'mean'),
    Tiempo_Busqueda_ms_mean=('Tiempo_Busqueda_ms', 'mean'),
    Tiempo_Busqueda_ms_std=('Tiempo_Busqueda_ms', 'std'), 
    Num_Ocurrencias_mean=('Num_Ocurrencias', 'mean'),
    Documentos_Encontrados_mean=('Documentos_Encontrados', 'mean')
)

# 4. Cálculo de Tasa de Localización (Ocurrencias / Tiempo_Busqueda_ms)
df_promedios['Tasa_Localiz'] = np.where(
    df_promedios['Tiempo_Busqueda_ms_mean'] > 0.001, 
    df_promedios['Num_Ocurrencias_mean'] / df_promedios['Tiempo_Busqueda_ms_mean'],
    0.000 
)

# Preparar la tabla de resultados para impresión
df_tabla = df_promedios.rename(columns={
    'Tiempo_Construccion_s_mean': 'T. Construcción (s)',
    'Tiempo_Busqueda_ms_mean': 'T. Búsqueda (ms)',
    'Num_Ocurrencias_mean': 'Ocurrencias',
    'Documentos_Encontrados_mean': 'Docs Únicos',
    'Tasa_Localiz': 'Tasa Localiz. (Occ/ms)',
    'Estructura': 'Estructura',
    'Patron': 'Patron'
})
df_tabla = df_tabla[[
    'Estructura', 'Patron', 'T. Construcción (s)', 'T. Búsqueda (ms)', 
    'Ocurrencias', 'Docs Únicos', 'Tasa Localiz. (Occ/ms)'
]]
df_tabla = df_tabla.round(3)

print("######################################################################")
print("## TABLA DE PROMEDIOS DE RESULTADOS EXPERIMENTALES ##")
print(df_tabla.to_markdown(index=False))
print("######################################################################")

# 5. Gráfico 1: Comparación de Tiempos de Construcción
# Agrupamos solo por estructura (promedio de todas las repeticiones y patrones)
df_construccion_promedio = df.groupby('Estructura', as_index=False)['Tiempo_Construccion_s'].mean()

plt.figure(figsize=(8, 5))
# El FutureWarning sugiere no usar palette sin hue, pero lo ignoramos por ahora
sns.barplot(x='Estructura', y='Tiempo_Construccion_s', data=df_construccion_promedio, palette='viridis')
plt.title('Tiempo de Construcción Promedio: FMI vs SA')
plt.ylabel('Tiempo (segundos)')
plt.xlabel('Estructura')

plt.grid(axis='y', linestyle='--')
plt.savefig('grafico_construccion_final.png')
print("Gráfico de Tiempos de Construcción guardado como 'grafico_construccion_final.png'")


# 6. Gráfico 2: Comparación de Tiempos de Búsqueda (Consulta) por Patrón (CORRECCIÓN FINAL)
plt.figure(figsize=(15, 7))
# CORRECCIÓN: Usamos el DataFrame ORIGINAL (df) y dejamos que seaborn calcule la media (barra) y la SD (errorbar)
# Esto soluciona el conflicto de dimensiones (12 vs 6) al usar yerr=... y hue=...
sns.barplot(
    x='Patron', 
    y='Tiempo_Busqueda_ms', # <-- Columna de datos RAW
    hue='Estructura', 
    data=df, # <-- DataFrame ORIGINAL (RAW)
    palette='mako',
    errorbar='sd' # <-- Indicamos a seaborn que calcule la Desviación Estándar para las barras de error
)
plt.title('Tiempo Promedio de Búsqueda (Consulta) por Patrón: FM-Index vs Suffix Array')
plt.ylabel('Tiempo (milisegundos)')
plt.xlabel('Patrón')
plt.legend(title='Estructura')
plt.xticks(rotation=15)
plt.grid(axis='y', linestyle='--')
plt.tight_layout()
plt.savefig('grafico_tiempo_consulta_final.png')
print("Gráfico de Tiempos de Búsqueda guardado como 'grafico_tiempo_consulta_final.png'")


# 7. Gráfico 3: Comparación de Tasa de Localización (Usa df_promedios, está correcto)
plt.figure(figsize=(15, 7))
# Aquí no necesitamos barras de error, por lo que df_promedios es adecuado
sns.barplot(x='Patron', y='Tasa_Localiz', hue='Estructura', data=df_promedios, palette='rocket')
plt.title('Tasa Promedio de Localización (Ocurrencias/ms)')
plt.ylabel('Ocurrencias por milisegundo')
plt.xlabel('Patrón')
plt.legend(title='Estructura')
plt.xticks(rotation=15)
plt.grid(axis='y', linestyle='--')
plt.tight_layout()
plt.savefig('grafico_tasa_localizacion_final.png')
print("Gráfico de Tasa de Localización guardado como 'grafico_tasa_localizacion_final.png'")


# 8. Gráfico 4: Documentos Encontrados por Patrón (Usa df_promedios, está correcto)
plt.figure(figsize=(15, 7))
sns.barplot(x='Patron', y='Documentos_Encontrados_mean', hue='Estructura', data=df_promedios, palette='cubehelix')
plt.title('Documentos Únicos Encontrados por Patrón')
plt.ylabel('Número de Documentos Únicos')
plt.xlabel('Patrón')
plt.legend(title='Estructura')
plt.xticks(rotation=15)
plt.grid(axis='y', linestyle='--')
plt.tight_layout()
plt.savefig('grafico_docs_encontrados_final.png')
print("Gráfico de Documentos Encontrados guardado como 'grafico_docs_encontrados_final.png'")
