# Proyecto2Edaa

## Descripción
Este proyecto implementa estructuras y algoritmos para trabajar con índices y arreglos de sufijos, utilizando la biblioteca SDSL. Incluye funcionalidades como la construcción de FM-Index y Suffix Array, búsqueda de patrones y localización de documentos.

---

## Estructura del Proyecto

```
Proyecto2Edaa/
├── analisis.py          # Script para análisis de datos
├── experimento.sh       # Script de experimentación
├── FM-index.cpp         # Implementación del FM-Index
├── sa.cpp               # Implementación del Suffix Array
├── plots.ipynb          # Notebook para visualización de datos
├── posiciones.txt       # Archivo con posiciones de inicio de documentos
├── preprocesar.py       # Script para concatenar documentos y generar índices de mapeo
├── README.md            # Instrucciones y documentación
├── datasets/            # Carpeta con datasets de prueba
│   ├── dblp/            # Datos bibliográficos
│   ├── proteins/        # Datos relacionados con proteínas
│   └── sources/         # Datos técnicos y de sistemas
```

---

## Requisitos

- **Compilador**: `g++` compatible con C++11 o superior.
- **Biblioteca SDSL**: Debe estar instalada y configurada en el sistema.

---

## Instrucciones de Compilación

### Preprocesamiento
- El script `preprocesar.py` se utiliza para concatenar los documentos y generar el archivo `posiciones.txt`, que mapea índices del texto concatenado a sus documentos originales.

### FM-Index
1. Compilar el archivo `FM-index.cpp`:
   ```bash
   g++ -std=c++11 -DNDEBUG -O3 -I<ruta_includes_sdsl> -L<ruta_lib_sdsl> FM-index.cpp -o fmi -lsdsl -ldivsufsort -ldivsufsort64
   ```
   Reemplaza `<ruta_includes_sdsl>` y `<ruta_lib_sdsl>` con las rutas correspondientes a las bibliotecas SDSL instaladas. Si SDSL está en el `PATH`, no es necesario especificar estas rutas.

   Si SDSL está en el `PATH`, no es necesario especificar estas rutas.

2. Ejecutar el programa:
   ```bash
   ./fmi <archivo_entrada>
   ```

### Suffix Array
1. Compilar el archivo `sa.cpp`:
   ```bash
   g++ -std=c++11 -DNDEBUG -O3 -I<ruta_includes_sdsl> -L<ruta_lib_sdsl> sa.cpp -o sa -lsdsl -ldivsufsort -ldivsufsort64
   ```
   Si SDSL está en el `PATH`, no es necesario especificar estas rutas.

2. Ejecutar el programa:
   ```bash
   ./sa <archivo_entrada>
   ```

---

## Datasets

- **`dblp`**: Contiene información bibliográfica sobre publicaciones académicas.
- **`proteins`**: Datos relacionados con proteínas y sus características.
- **`sources`**: Información técnica y de sistemas.

---

## Notas
- Los scripts en Python (`analisis.py`, `preprocesar.py`) y el notebook (`plots.ipynb`) son complementarios para análisis y visualización de datos.