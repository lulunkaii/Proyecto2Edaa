import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

# --- Configuración para Experimento 3 ---
RESULT_FILE = 'resultados_experimentales3.csv' 
plt.style.use('ggplot') 
# ----------------------------------------

# 1. Cargar y Preprocesar los datos
try:
    df = pd.read_csv(RESULT_FILE)
except FileNotFoundError:
    print(f"Error: Asegúrate de que '{RESULT_FILE}' existe y no está vacío. ¿Ejecutaste 'bash experimento3.sh'? o python preprocesar3.py")
    exit()
except pd.errors.EmptyDataError:
    print(f"Error: El archivo '{RESULT_FILE}' está vacío.")
    exit()

# 2. Conversión de Tipos (Asegúrate que el CSV use el formato: T_Construccion, T_Busqueda, Num_Ocurrencias, Num_Documentos)
try:
    df['Tiempo_Construccion_ms'] = pd.to_numeric(df['Tiempo_Construccion_ms'], errors='coerce')
    df['Tiempo_Busqueda_ms'] = pd.to_numeric(df['Tiempo_Busqueda_ms'], errors='coerce')
    df['Num_Ocurrencias'] = pd.to_numeric(df['Num_Ocurrencias'], errors='coerce')
    df['Documentos_Encontrados'] = pd.to_numeric(df['Documentos_Encontrados'], errors='coerce')
except KeyError as e:
    print(f"Error: Columna {e} no encontrada en el CSV. El formato del CSV es incorrecto.")
    exit()

df = df.dropna(subset=['Tiempo_Construccion_ms', 'Tiempo_Busqueda_ms'])

# Convertir el tiempo de construcción a segundos para mejor visualización
df['Tiempo_Construccion_s'] = df['Tiempo_Construccion_ms'] / 1000.0

# 3. Calcular Promedios
df_promedios = df.groupby(['Estructura', 'Patron'], as_index=False).agg(
    Tiempo_Construccion_s_mean=('Tiempo_Construccion_s', 'mean'),
    Tiempo_Busqueda_ms_mean=('Tiempo_Busqueda_ms', 'mean'),
    Num_Ocurrencias_mean=('Num_Ocurrencias', 'mean'),
    Documentos_Encontrados_mean=('Documentos_Encontrados', 'mean')
)

print("## TABLA DE PROMEDIOS GENERALES POR PATRÓN (EXPERIMENTO 3) ##")
print(df_promedios.to_markdown(index=False))
print("-" * 70)


# 4. Gráfico 1: Comparación de Tiempos de Construcción
df_construccion_promedio = df.groupby('Estructura', as_index=False)['Tiempo_Construccion_s'].mean()

plt.figure(figsize=(8, 5))
sns.barplot(x='Estructura', y='Tiempo_Construccion_s', data=df_construccion_promedio, palette='viridis')
plt.title('Tiempo de Construcción Promedio: FMI vs SA (Exp 3)')
plt.ylabel('Tiempo (segundos)')
plt.xlabel('Estructura')
plt.grid(axis='y', linestyle='--')
plt.savefig('grafico_construccion_final3.png') # Prefijo 3
print("Gráfico de Tiempos de Construcción guardado como 'grafico_construccion_final3.png'")


# 5. Gráfico 2: Comparación de Tiempos de Búsqueda (Consulta) por Patrón
plt.figure(figsize=(15, 7))
sns.barplot(x='Patron', y='Tiempo_Busqueda_ms_mean', hue='Estructura', data=df_promedios, palette='mako')
plt.title('Tiempo Promedio de Búsqueda (Consulta) por Patrón: FM-Index vs Suffix Array (Exp 3)')
plt.ylabel('Tiempo (milisegundos)')
plt.xlabel('Patrón')
plt.legend(title='Estructura')
plt.grid(axis='y', linestyle='--')
plt.savefig('grafico_tiempo_consulta_final3.png') # Prefijo 3
print("Gráfico de Tiempos de Búsqueda guardado como 'grafico_tiempo_consulta_final3.png'")


# 6. Gráfico 3: Comparación de Documentos Encontrados por Patrón
plt.figure(figsize=(15, 7))
sns.barplot(x='Patron', y='Documentos_Encontrados_mean', hue='Estructura', data=df_promedios, palette='rocket')
plt.title('Documentos Únicos Encontrados por Patrón: FM-Index vs Suffix Array (Exp 3)')
plt.ylabel('Documentos Encontrados (Promedio)')
plt.xlabel('Patrón')
plt.legend(title='Estructura')
plt.grid(axis='y', linestyle='--')
plt.savefig('grafico_docs_encontrados_final3.png') # Prefijo 3
print("Gráfico de Documentos Encontrados guardado como 'grafico_docs_encontrados_final3.png'")
