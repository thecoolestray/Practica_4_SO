#ifndef TACTILE_H
#define TACTILE_H

#include <vector>
#include <string>

// Estructura para almacenar una captura tactil
struct Capture {
    int id;
    std::vector<std::vector<double>> matrix;
};

// Lee todas las capturas desde el archivo JSON
std::vector<Capture> leerJSON(const std::string& filename);

// Valida matriz 16x16
bool validarMatriz(const std::vector<std::vector<double>>& matriz);

// Interpolacion bilineal 16x16 → 128x128
std::vector<std::vector<double>> Interpolacionbilineal(
    const std::vector<std::vector<double>>& input,
    int nuevoAncho,
    int nuevaAltura
);

// Convierte matriz a JSON
std::string matrizaJSON(
    int captureID,
    const std::vector<std::vector<double>>& matriz
);

// Envia datos al servidor Python
bool enviarPOST(
    int captureID,
    const std::vector<std::vector<double>>& matriz
);

#endif
