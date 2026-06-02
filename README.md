# Práctica 4 - Procesamiento e Interpolación de Imágenes Táctiles

## Descripción

Este proyecto implementa un sistema distribuido en **C++ y Python** para el procesamiento de datos táctiles obtenidos de un gripper robótico.

A partir de un conjunto de 50 capturas almacenadas en formato JSON, el sistema:

1. Lee las matrices táctiles originales de 16×16.
2. Valida que todas las capturas tengan el formato correcto.
3. Aplica interpolación bilineal manual para aumentar la resolución a 128×128.
4. Envía las matrices interpoladas mediante HTTP desde un cliente en C++.
5. Recibe los datos en un servidor Python utilizando Flask.
6. Genera imágenes táctiles utilizando Matplotlib.
7. Guarda automáticamente las imágenes generadas.

---

## Objetivos

* Procesar información táctil almacenada en JSON.
* Manipular matrices bidimensionales.
* Implementar interpolación bilineal manual.
* Utilizar comunicación cliente-servidor mediante HTTP.
* Integrar aplicaciones desarrolladas en C++ y Python.
* Generar representaciones visuales de presión táctil.

---

## Tecnologías utilizadas

### C++

* nlohmann/json
* libcurl

### Python

* Flask
* NumPy
* Matplotlib

---

## Estructura del proyecto

```text
.
├── cliente
├── json.hpp
├── main.cpp
├── output_images/
│   ├── capture_0.png
│   ├── ...
│   └── capture_49.png
├── servidor.py
├── tactile.cpp
├── tactile.h
├── tactile_captures_50.json
└──.gitignore
```

### Descripción de archivos

| Archivo                  | Función                                                      |
| ------------------------ | ------------------------------------------------------------ |
| main.cpp                 | Programa principal del cliente                               |
| tactile.h                | Declaración de funciones                                     |
| tactile.cpp              | Implementación de lectura, validación e interpolación        |
| json.hpp                 | Biblioteca JSON para C++                                     |
| servidor.py              | Servidor Flask encargado de recibir datos y generar imágenes |
| tactile_captures_50.json | Archivo con las 50 capturas táctiles                         |
| output_images            | Directorio donde se almacenan las imágenes generadas         |
| cliente                  | Ejecutable compilado del cliente                             |

---

## Funcionamiento del sistema

### Paso 1: Lectura del JSON

El programa carga las 50 capturas táctiles almacenadas en el archivo:

```text
tactile_captures_50.json
```

Cada captura contiene una matriz de 16×16 valores de presión.

### Paso 2: Validación

Se comprueba que todas las matrices tengan exactamente:

* 16 filas
* 16 columnas

### Paso 3: Interpolación bilineal

Cada matriz es ampliada desde:

```text
16 × 16 → 128 × 128
```

La interpolación bilineal se implementa manualmente sin utilizar funciones automáticas de OpenCV o SciPy.

### Paso 4: Comunicación HTTP

El cliente desarrollado en C++ envía las matrices interpoladas al servidor Python mediante solicitudes HTTP POST.

### Paso 5: Generación de imágenes

El servidor recibe la matriz y genera un mapa táctil utilizando Matplotlib con el mapa de color **inferno**.

### Paso 6: Almacenamiento

Las imágenes generadas se almacenan automáticamente dentro del directorio:

```text
output_images/
```

---

## Compilación
### Requisitos

Antes de compilar, instala las dependencias necesarias:

```bash
sudo apt update
sudo apt install g++ libcurl4-openssl-dev python3 python3-pip
```
### Para compilar el cliente C++:

```bash
g++ main.cpp tactile.cpp -o cliente -lcurl
```
### Ejecución

Primero inicia el servidor Python:

```python
python3 server.py
```
En otra terminal ejecuta el cliente C++:
```python
./cliente
```
## Flujo del sistema

```text
Archivo JSON
      ↓
Matrices 16×16
      ↓
Validación
      ↓
Interpolación Bilineal
      ↓
Matrices 128×128
      ↓
HTTP POST
      ↓
Servidor Flask
      ↓
Matplotlib
      ↓
Imágenes PNG
```

---

## Resultados

El sistema procesa correctamente las 50 capturas táctiles incluidas en el conjunto de datos.

Para cada captura:

* Se valida la estructura de la matriz.
* Se realiza la interpolación a 128×128.
* Se envían los datos al servidor.
* Se genera una imagen táctil.
* Se almacena la imagen correspondiente.

Como resultado se obtienen 50 mapas táctiles que permiten visualizar la distribución de presión con mayor nivel de detalle.

---

## Conclusiones

Esta práctica permitió aplicar conceptos fundamentales de Sistemas Operativos y programación distribuida mediante la integración de aplicaciones desarrolladas en C++ y Python.

Además, se trabajó con procesamiento de matrices, interpolación numérica, comunicación HTTP y visualización científica de datos.

La interpolación bilineal permitió mejorar significativamente la representación visual de las capturas táctiles originales, generando imágenes más detalladas y fáciles de analizar.

---

## Autor
