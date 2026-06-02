# Practica 4: Procesamiento de datos táctiles y comunicación Cliente-Servidor usando C++ y Python (HTTP + Flask)

---

## 1. Descripción

Este repositorio contiene una práctica de sistemas desarrollada con **C++ y Python** cuyo objetivo es implementar un sistema de procesamiento de datos táctiles con comunicación cliente-servidor mediante **HTTP (libcurl) y Flask**.

El sistema trabaja con datos de:
- Capturas táctiles almacenadas en formato JSON
- Matrices de 16x16 valores de presión
- Interpolación bilineal para convertir los datos a 128x128
- Generación de imágenes tipo mapa de calor en Python

El proyecto resuelve el problema mediante:
- Lectura de archivos JSON en C++
- Validación de matrices 16x16
- Interpolación bilineal manual (16x16 → 128x128)
- Serialización de datos a formato JSON
- Envío de datos mediante HTTP POST usando libcurl
- Recepción de datos en un servidor Flask
- Generación de imágenes con matplotlib

El programa utiliza conceptos fundamentales como:
- Comunicación cliente-servidor mediante HTTP
- Uso de libcurl para peticiones web
- Manipulación de JSON (nlohmann/json y Python dicts)
- Arrays dinámicos con vector
- Algoritmo de interpolación bilineal
- Procesamiento de datos en Python

---

## 2. Estructura del repositorio

```
Practica_3_SO/
├── main.cpp
├── tactile.cpp
├── tactile.h
├── server.py
├── tactile_captures_50.json
├── output_images/
└── README.md
```

---

## 3. Requisitos del sistema

- **Sistema operativo:** Linux (Ubuntu 22.04 o superior)
- **Compilador:** g++
- **Python 3**
- **pip**
- **libcurl**
- **Terminal Bash**

### Instalación de dependencias

```bash
sudo apt update
sudo apt install g++ libcurl4-openssl-dev python3 python3-pip

pip install flask numpy matplotlib
```

---

## 4. Compilación y ejecución

### 4.1 Compilar el cliente C++

```bash
g++ main.cpp tactile.cpp -o cliente -lcurl
```

### 4.2 Ejecutar el servidor Python

```bash
python3 server.py
```

### 4.3 Ejecutar el cliente C++

```bash
./cliente
```

### 4.4 Ejecutar con archivo JSON personalizado

```bash
./cliente tactile_captures_50.json
```

---

## 5. Funcionamiento del programa en C++

El programa en C++ realiza las siguientes operaciones:

1. Lee el archivo JSON con las capturas táctiles
2. Valida matrices 16x16
3. Aplica interpolación bilineal (16x16 → 128x128)
4. Convierte la matriz a JSON
5. Envía los datos al servidor Flask mediante HTTP POST
6. Maneja respuestas del servidor
7. Utiliza libcurl para comunicación HTTP

---

## 6. Funcionamiento del servidor Python

El servidor Flask realiza las siguientes operaciones:

1. Recibe datos JSON desde el cliente C++
2. Valida dimensiones de la matriz
3. Convierte datos a NumPy array
4. Genera mapa de calor con matplotlib
5. Guarda imágenes PNG en `output_images/`
6. Devuelve respuesta al cliente

---

## 7. Endpoint del servidor

```
POST http://127.0.0.1:5000/capture
```

Estructura del JSON:

```json
{
  "capture_id": 1,
  "width": 128,
  "height": 128,
  "data": [[...], [...]]
}
```

---

## 8. Algoritmo implementado

El proyecto implementa **interpolación bilineal**:

1. Toma matriz 16x16
2. La escala a 128x128
3. Calcula valores intermedios usando 4 vecinos
4. Genera una superficie suave de datos

---

## 9. Ejemplo de salida

**Cliente C++:**

```
Archivo: tactile_captures_50.json
Archivo JSON leido correctamente.
Capturas cargadas: 50

----------------------------------
Procesando captura 2
Validacion OK: matriz 16x16
Interpolacion completada: 128x128
Captura 2 enviada correctamente (HTTP 200)
----------------------------------
```

**Servidor Python:**

```
==================================
Captura recibida: 2
Dimensiones: 128x128
Aviso: captura 2 ya existia, sobreescribiendo.
Imagen guardada: output_images/capture_2.png
==================================
127.0.0.1 - - [02/Jun/2026 20:30:09] "POST /capture HTTP/1.1" 200 -
```

---

## 10. Problemas comunes

**Error de conexión:**
> Verificar que Flask está en ejecución antes del cliente.

**Error libcurl:**
```bash
sudo apt install libcurl4-openssl-dev
```

**Error Python:**
```bash
pip install flask numpy matplotlib
```

---

## 11. Autor

- **Zineb Hamman**
- **Naela Khaldi**
- Sistemas Operativos — Curso Académico 2025/2026

---

## 12. Notas

Este proyecto simula un sistema real de procesamiento de sensores táctiles, donde C++ realiza el procesamiento numérico y Python la visualización.

Representa una arquitectura completa:

- **C++** → procesamiento
- **HTTP** → comunicación
- **Python** → visualización
