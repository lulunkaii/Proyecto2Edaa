import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os
import sys

# Directorios
RESULTADOS_DIR = 'resultados'
GRAFICOS_DIR = 'plots'
DATASETS = ['sources', 'proteins', 'dblp']

if not os.path.exists(GRAFICOS_DIR):
    os.makedirs(GRAFICOS_DIR)

sns.set_theme(style="whitegrid", context="talk")

COLOR_SA = "#8e44ad"  # morado
COLOR_FM = "#ff4081"  # rosado

def generar_grafico_dataset(dataset_name):
    # Rutas esperadas
    path_sa = os.path.join(RESULTADOS_DIR, f'resultados_{dataset_name}_sa.csv')
    path_fm = os.path.join(RESULTADOS_DIR, f'resultados_{dataset_name}_fm.csv')

    if not os.path.exists(path_sa) or not os.path.exists(path_fm):
        print(f"Advertencia: No se encontraron resultados para {dataset_name}. Saltando...")
        return

    # Cargamos dataset
    try:
        df_sa = pd.read_csv(path_sa)
        df_fm = pd.read_csv(path_fm)
    except Exception as e:
        print(f"Error de {dataset_name}: {e}")
        return

    # 3 subplots concatenados
    fig, (ax1, ax2, ax3) = plt.subplots(1, 3, figsize=(20, 6))
    fig.suptitle(f'Análisis de rendimiento: dataset {dataset_name.upper()}', fontsize=20, color='#333333')

    # --- GRÁFICO 1: Tiempo de Búsqueda (Escala Logarítmica) ---
    ax1.plot(df_sa['n'], df_sa['t_mean'], label='Suffix Array', color=COLOR_SA, linewidth=2)
    ax1.plot(df_fm['n'], df_fm['t_mean'], label='FM-Index', color=COLOR_FM, linewidth=2)
    
    ax1.fill_between(df_sa['n'], df_sa['t_mean'] - df_sa['t_stdev'], df_sa['t_mean'] + df_sa['t_stdev'], color=COLOR_SA, alpha=0.15)
    ax1.fill_between(df_fm['n'], df_fm['t_mean'] - df_fm['t_stdev'], df_fm['t_mean'] + df_fm['t_stdev'], color=COLOR_FM, alpha=0.15)

    ax1.set_title('Tiempo Promedio de Búsqueda (log)', fontsize=14)
    ax1.set_xlabel('Largo del Patrón (n)', fontsize=12)
    ax1.set_ylabel('Tiempo (ns)', fontsize=12)
    ax1.set_yscale('log')
    ax1.grid(True, which="both", ls="-", alpha=0.3)
    ax1.legend()

    # --- GRÁFICO 2: Uso de Memoria (MB) ---
    mem_sa = df_sa['memory_mb'].iloc[0]
    mem_fm = df_fm['memory_mb'].iloc[0]
    
    bar_labels = ['Suffix Array', 'FM-Index']
    bar_values = [mem_sa, mem_fm]
    colors = [COLOR_SA, COLOR_FM]
    
    sns.barplot(x=bar_labels, y=bar_values, hue=bar_labels, palette=colors, legend=False, ax=ax2)
    
    ax2.set_title('Uso de Memoria RAM', fontsize=14)
    ax2.set_ylabel('Tamaño (MB)', fontsize=12)
    ax2.grid(axis='y', alpha=0.3)
    
    # Tags sobre barras
    for i, v in enumerate(bar_values):
        ax2.text(i, v + (v * 0.02), f'{v:.2f} MB', ha='center', va='bottom', fontsize=11, fontweight='bold', color=colors[i])

    # --- GRÁFICO 3: Tiempo de Construcción (ms) ---
    build_sa = df_sa['build_time_ms'].iloc[0]
    build_fm = df_fm['build_time_ms'].iloc[0]
    
    build_values = [build_sa, build_fm]
    
    sns.barplot(x=bar_labels, y=build_values, hue=bar_labels, palette=colors, legend=False, ax=ax3)
    
    ax3.set_title('Tiempo de Construcción', fontsize=14)
    ax3.set_ylabel('Tiempo (ms)', fontsize=12)
    ax3.grid(axis='y', alpha=0.3)

    for i, v in enumerate(build_values):
        ax3.text(i, v + (v * 0.02), f'{v:.0f} ms', ha='center', va='bottom', fontsize=11, fontweight='bold', color=colors[i])

    # Guardar
    output_path = os.path.join(GRAFICOS_DIR, f'analisis_{dataset_name}.png')
    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    plt.savefig(output_path, dpi=300) # Mayor calidad
    print(f"Gráfico generado: {output_path}")
    plt.close()

if __name__ == "__main__":
    print("Generando gráficos estilizados...")
    for ds in DATASETS:
        generar_grafico_dataset(ds)
    print("¡Listo! Revisa la carpeta 'graficos'.")