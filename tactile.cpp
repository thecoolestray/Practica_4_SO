#include "tactile.h"
#include "json.hpp"

#include <iostream>
#include <fstream>
#include <cmath>
#include <curl/curl.h>

using namespace std;
using json = nlohmann::json;


// Lee el archivo JSON y devuelve todas las capturas
vector<Capture> leerJSON(const string& filename) {

    ifstream archivo(filename);

    if (!archivo.is_open()) {
        cout << "Error: no se pudo abrir el archivo: " << filename << endl;
        exit(1);
    }

    json j;
    archivo >> j;

    vector<Capture> capturas;

    for (const auto& item : j["captures"]) {

        Capture captura;
        captura.id = item["id"];

        for (const auto& fila : item["matrix"]) {

            vector<double> filaMatriz;

            for (const auto& valor : fila) {
                filaMatriz.push_back(valor.get<double>());
            }

            captura.matrix.push_back(filaMatriz);
        }

        capturas.push_back(captura);
    }

    cout << "Archivo JSON leido correctamente." << endl;

    return capturas;
}


// Valida que la matriz sea exactamente 16x16
bool validarMatriz(const vector<vector<double>>& matriz) {

    if (matriz.size() != 16) return false;

    for (int i = 0; i < (int)matriz.size(); i++) {
        if (matriz[i].size() != 16) return false;
    }

    return true;
}


// Interpolacion bilineal manual: 16x16 → 128x128
vector<vector<double>> Interpolacionbilineal(
    const vector<vector<double>>& entrada,
    int nuevoAncho,
    int nuevaAltura) {

    int altoAntiguo = (int)entrada.size();
    int anchoAntiguo = (int)entrada[0].size();

    vector<vector<double>> salida(nuevaAltura, vector<double>(nuevoAncho, 0.0));

    double ratioX = (double)(anchoAntiguo - 1) / (nuevoAncho - 1);
    double ratioY = (double)(altoAntiguo - 1) / (nuevaAltura - 1);

    for (int y = 0; y < nuevaAltura; y++) {
        for (int x = 0; x < nuevoAncho; x++) {

            double gx = x * ratioX;
            double gy = y * ratioY;

            int x1 = (int)floor(gx);
            int y1 = (int)floor(gy);
            int x2 = min(x1 + 1, anchoAntiguo - 1);
            int y2 = min(y1 + 1, altoAntiguo - 1);

            double dx = gx - x1;
            double dy = gy - y1;

            double q11 = entrada[y1][x1];
            double q21 = entrada[y1][x2];
            double q12 = entrada[y2][x1];
            double q22 = entrada[y2][x2];

            double r1 = q11 * (1 - dx) + q21 * dx;
            double r2 = q12 * (1 - dx) + q22 * dx;

            salida[y][x] = r1 * (1 - dy) + r2 * dy;
        }
    }

    return salida;
}


// Convierte la matriz a JSON
string matrizaJSON(int captureID, const vector<vector<double>>& matriz) {

    json j;
    j["capture_id"] = captureID;
    j["width"]  = (int)matriz[0].size();
    j["height"] = (int)matriz.size();
    j["data"]   = matriz;

    return j.dump();
}


// Callback CURL
static size_t callbackEscribir(void* contents, size_t size, size_t nmemb, void* userp) {
    return size * nmemb;
}


// Envia la matriz al servidor Python
bool enviarPOST(int captureID, const vector<vector<double>>& matriz) {

    CURL* curl = curl_easy_init();

    if (!curl) {
        cout << "Error: no se pudo inicializar CURL." << endl;
        return false;
    }

    string cuerpo = matrizaJSON(captureID, matriz);

    struct curl_slist* encabezados = nullptr;
    encabezados = curl_slist_append(encabezados, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:5000/capture");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, encabezados);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, cuerpo.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)cuerpo.size());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callbackEscribir);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

    CURLcode resultado = curl_easy_perform(curl);

    long codigoHTTP = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &codigoHTTP);

    curl_slist_free_all(encabezados);
    curl_easy_cleanup(curl);

    if (resultado != CURLE_OK) {
        cout << "Error de conexion: " << curl_easy_strerror(resultado) << endl;
        return false;
    }

    if (codigoHTTP == 200) {
        cout << "Captura " << captureID
             << " enviada correctamente (HTTP " << codigoHTTP << ")" << endl;
        return true;
    }

    cout << "Error: HTTP " << codigoHTTP << endl;
    return false;
}
