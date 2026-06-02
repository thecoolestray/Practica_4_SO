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

// Valida que la matriz sea exactamente 16x16
bool validarMatriz(const std::vector<std::vector<double>>& matrix);

// Interpolacion bilineal manual: escala 16x16 a 128x128
std::vector<std::vector<double>> Interpolacionbilineal(
    const std::vector<std::vector<double>>& input,
    int newWidth,
    int newHeight
);

// Convierte la matriz interpolada a formato JSON string
std::string matrizaJSON(
    int captureID,
    const std::vector<std::vector<double>>& matrix
);

// Envia la matriz interpolada al servidor Python via HTTP POST
bool enviarPOST(
    int captureID,
    const std::vector<std::vector<double>>& matrix
);

#endif
