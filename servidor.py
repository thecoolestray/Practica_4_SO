import os
from flask import Flask, request, jsonify
import numpy as np
import matplotlib
matplotlib.use("Agg")   # Sin ventana grafica, solo guardar archivo
import matplotlib.pyplot as plt

app = Flask(__name__)

# carpeta donde se guardan las imagenes generadas
OUTPUT_DIR = "output_images"
os.makedirs(OUTPUT_DIR, exist_ok=True)


# Genera y guarda la imagen tactil como archivo PNG
def generateImage(capture_id, matrix_np):

    fig, ax = plt.subplots(figsize=(6, 6))

    # Mostrar la matriz como mapa de calor
    im = ax.imshow(matrix_np, cmap="inferno")

    # Barra de color lateral
    plt.colorbar(im, ax=ax, label="Presion")

    # Titulos y etiquetas
    ax.set_title("Mapa tactil - Captura " + str(capture_id))
    ax.set_xlabel("X")
    ax.set_ylabel("Y")

    # Nombre del archivo de salida
    filename = os.path.join(OUTPUT_DIR, "capture_" + str(capture_id) + ".png")

    # Guardar la imagen
    plt.savefig(filename, bbox_inches="tight", dpi=300)
    plt.close(fig)

    return filename


# Endpoint principal: recibe matrices desde el cliente C++
@app.route('/capture', methods=['POST'])
def receive_capture():

    try:
        # ler el JSON recibido
        data = request.get_json()

        if data is None:
            return jsonify({"status": "error", "message": "No se recibio JSON"}), 400

        # Extraer los campos del JSON
        capture_id = data["capture_id"]
        width      = data["width"]
        height     = data["height"]
        matrix     = data["data"]

        print("")
        print("==================================")
        print("Captura recibida: " + str(capture_id))
        print("Dimensiones: " + str(width) + "x" + str(height))

        # Validar que las dimensiones son correctas
        if len(matrix) != height:
            return jsonify({"status": "error", "message": "Altura incorrecta"}), 400

        for row in matrix:
            if len(row) != width:
                return jsonify({"status": "error", "message": "Anchura incorrecta"}), 400

        # Avisar si la captura ya existia
        expected_file = os.path.join(OUTPUT_DIR, "capture_" + str(capture_id) + ".png")
        if os.path.exists(expected_file):
            print("Aviso: captura " + str(capture_id) + " ya existia, sobreescribiendo.")

        # Convertir la lista a array de NumPy y generar la imagen
        matrix_np = np.array(matrix)
        filename  = generateImage(capture_id, matrix_np)

        print("Imagen guardada: " + filename)
        print("==================================")

        return jsonify({"status": "ok", "capture_id": capture_id, "file": filename})

    except KeyError as e:
        print("Campo faltante en JSON: " + str(e))
        return jsonify({"status": "error", "message": "Campo faltante: " + str(e)}), 400

    except Exception as e:
        print("Error interno: " + str(e))
        return jsonify({"status": "error", "message": str(e)}), 500


# Arrancar el servidor Flask
if __name__ == '__main__':
    print("==================================")
    print("Servidor Flask iniciado")
    print("URL: http://127.0.0.1:5000")
    print("Esperando capturas de C++...")
    print("==================================")

    app.run(host='127.0.0.1', port=5000, debug=False)
