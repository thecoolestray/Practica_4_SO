#include <iostream>
#include <string>
#include <vector>
#include "tactile.h"

using namespace std;

int main(int argc, char* argv[]) {

    string archivo = "tactile_captures_50.json";

    if (argc > 1) {
        archivo = argv[1];
    }

    cout << "==================================" << endl;
    cout << "Sistema de procesamiento tactil" << endl;
    cout << "==================================" << endl;
    cout << "Archivo: " << archivo << endl;

    vector<Capture> capturas = leerJSON(archivo);

    if (capturas.empty()) {
        cout << "Error: no se pudieron cargar capturas." << endl;
        return 1;
    }

    cout << "Capturas cargadas: " << capturas.size() << endl;

    int enviados = 0;
    int omitidos = 0;

    for (int i = 0; i < (int)capturas.size(); i++) {

        cout << "----------------------------------" << endl;
        cout << "Procesando captura " << capturas[i].id << endl;

        if (!validarMatriz(capturas[i].matrix)) {
            cout << "Matriz invalida (no 16x16). Saltando." << endl;
            omitidos++;
            continue;
        }

        cout << "Validacion OK" << endl;

        vector<vector<double>> interpolada =
            Interpolacionbilineal(capturas[i].matrix, 128, 128);

        cout << "Interpolacion lista: "
             << interpolada.size() << "x"
             << interpolada[0].size() << endl;

        bool ok = enviarPOST(capturas[i].id, interpolada);

        if (ok) {
            enviados++;
        } else {
            cout << "Error enviando captura " << capturas[i].id << endl;
        }
    }

    cout << "==================================" << endl;
    cout << "RESUMEN" << endl;
    cout << "Enviadas: " << enviados << endl;
    cout << "Omitidas: " << omitidos << endl;
    cout << "==================================" << endl;

    return 0;
}
