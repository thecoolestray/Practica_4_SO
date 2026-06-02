#include <iostream>
#include <string>
#include <vector>
#include "tactile.h"

using namespace std;

int main(int argc, char* argv[]) {

    //1. Leer el archivo JSON
    string filename = "tactile_captures_50.json";

    if (argc > 1) {
        filename = argv[1];
    }

    cout << "==================================" << endl;
    cout << "Sistema de procesamiento tactil" << endl;
    cout << "==================================" << endl;
    cout << "Archivo: " << filename << endl;

    vector<Capture> captures = readJSON(filename);

    if (captures.empty()) {
        cout << "Error: no se han podido cargar capturas." << endl;
        return 1;
    }

    cout << "Capturas cargadas: " << captures.size() << endl;
    cout << endl;

    //contadores para el resumen final
    int sent    = 0;
    int skipped = 0;

    //2. Procesar cada captura 
    for (int i = 0; i < (int)captures.size(); i++) {

        cout << "----------------------------------" << endl;
        cout << "Procesando captura " << captures[i].id << endl;

        //3. Validar que la matriz sea 16x16
        if (!validateMatrix(captures[i].matrix)) {
            cout << "Matriz invalida: no tiene 16x16. Saltando." << endl;
            skipped++;
            continue;
        }

        cout << "Validacion OK: matriz 16x16" << endl;

        //4. Aplicar interpolacion bilineal 16x16 → 128x128
        vector<vector<double>> interpolated =
            bilinearInterpolation(captures[i].matrix, 128, 128);

        cout << "Interpolacion completada: "
             << interpolated.size() << "x"
             << interpolated[0].size() << endl;

        //5.enviar al servidor Python via HTTP POST
        bool ok = sendPOST(captures[i].id, interpolated);

        if (ok) {
            sent++;
        } else {
            cout << "Fallo al enviar captura " << captures[i].id << endl;
        }
    }

    //6. Resumen final
    cout << endl;
    cout << "==================================" << endl;
    cout << "RESUMEN" << endl;
    cout << "==================================" << endl;
    cout << "Total capturas : " << captures.size() << endl;
    cout << "Enviadas OK    : " << sent             << endl;
    cout << "Saltadas       : " << skipped          << endl;
    cout << "==================================" << endl;
    cout << "Proceso finalizado." << endl;

    return 0;
}
