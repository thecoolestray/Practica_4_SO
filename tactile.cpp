#include "tactile.h"
#include "json.hpp"

#include <iostream>
#include <fstream>
#include <cmath>
#include <curl/curl.h>

using namespace std;
using json = nlohmann::json;


// Lee el archivo JSON y devuelve todas las capturas
vector<Capture> readJSON(const string& filename) {

    // Abrir el archivo
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Error: no se pudo abrir el archivo: " << filename << endl;
        exit(1);
    }

    // Parsear el JSON
    json j;
    file >> j;

    vector<Capture> captures;

    for (const auto& item : j["captures"]) {

        Capture capture;
        capture.id = item["id"];

        // leer cada fila de la matriz
        for (const auto& row : item["matrix"]) {

            vector<double> matrixRow;

            for (const auto& value : row) {
                matrixRow.push_back(value.get<double>());
            }

            capture.matrix.push_back(matrixRow);
        }

        captures.push_back(capture);
    }

    cout << "Archivo JSON leido correctamente." << endl;

    return captures;
}


// Valida que la matriz sea exactamente 16x16
bool validateMatrix(const vector<vector<double>>& matrix) {

    // Comprobar que tiene exactamente 16 filas
    if (matrix.size() != 16) {
        return false;
    }

    // Comprobar que cada fila tiene exactamente 16 columnas
    for (int i = 0; i < (int)matrix.size(); i++) {
        if (matrix[i].size() != 16) {
            return false;
        }
    }

    return true;
}


// Interpolacion bilineal manual: 16x16 → 128x128
//
// Para cada pixel de la imagen de salida (128x128),
// calculamos su posicion equivalente en la imagen
// original (16x16) y combinamos los 4 pixeles
// vecinos mas cercanos con pesos proporcionales.

vector<vector<double>> bilinearInterpolation(
    const vector<vector<double>>& input,
    int newWidth,
    int newHeight) {

    int oldHeight = (int)input.size();
    int oldWidth  = (int)input[0].size();

    // Crear la matriz de salida rellena de ceros
    vector<vector<double>> output(newHeight, vector<double>(newWidth, 0.0));

    // Razon de escala entre imagen original y nueva
    double x_ratio = (double)(oldWidth  - 1) / (newWidth  - 1);
    double y_ratio = (double)(oldHeight - 1) / (newHeight - 1);

    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {

            // Posicion equivalente en la imagen original
            double gx = x * x_ratio;
            double gy = y * y_ratio;

            // Indices de los 4 pixeles vecinos
            int x1 = (int)floor(gx);
            int y1 = (int)floor(gy);
            int x2 = min(x1 + 1, oldWidth  - 1);
            int y2 = min(y1 + 1, oldHeight - 1);

            // Distancia fraccional dentro del cuadrado
            double dx = gx - x1;
            double dy = gy - y1;

            // Valores de los 4 vecinos
            double q11 = input[y1][x1];
            double q21 = input[y1][x2];
            double q12 = input[y2][x1];
            double q22 = input[y2][x2];

            // interpolacion horizontal (dos veces)
            double r1 = q11 * (1 - dx) + q21 * dx;
            double r2 = q12 * (1 - dx) + q22 * dx;

            // Interpolacion vertical final
            output[y][x] = r1 * (1 - dy) + r2 * dy;
        }
    }

    return output;
}


// Convierte la matriz a un string JSON listo para enviar
string matrixToJSON(int captureID, const vector<vector<double>>& matrix) {

    json j;
    j["capture_id"] = captureID;
    j["width"]      = (int)matrix[0].size();
    j["height"]     = (int)matrix.size();
    j["data"]       = matrix;

    return j.dump();
}


// Callback de CURL: descarta la respuesta del servidor
static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    return size * nmemb;
}


// Envia la matriz interpolada al servidor Python via HTTP POST
bool sendPOST(int captureID, const vector<vector<double>>& matrix) {

    CURL* curl = curl_easy_init();

    if (!curl) {
        cout << "Error: no se pudo inicializar CURL." << endl;
        return false;
    }

    // Construir el cuerpo JSON usando matrixToJSON
    string body = matrixToJSON(captureID, matrix);

    // Configurar cabeceras HTTP
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    // Configurar opciones de CURL
    curl_easy_setopt(curl, CURLOPT_URL,           "http://127.0.0.1:5000/capture");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER,    headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS,    body.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)body.size());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT,       30L);

    // Ejecutar la peticion
    CURLcode result = curl_easy_perform(curl);

    // Obtener el codigo de respuesta HTTP
    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    // Liberar recursos
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    // Comprobar errores
    if (result != CURLE_OK) {
        cout << "Error de conexion: " << curl_easy_strerror(result) << endl;
        return false;
    }

    if (httpCode == 200) {
        cout << "Captura " << captureID
             << " enviada correctamente (HTTP " << httpCode << ")" << endl;
        return true;
    }

    cout << "Error: el servidor respondio con HTTP " << httpCode << endl;
    return false;
}
