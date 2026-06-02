import os
from flask import Flask, request, jsonify
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

app = Flask(__name__)

CARPETA_SALIDA = "output_images"
os.makedirs(CARPETA_SALIDA, exist_ok=True)


def generarImagen(capture_id, matriz_np):

    fig, ax = plt.subplots(figsize=(6, 6))

    im = ax.imshow(matriz_np, cmap="inferno")
    plt.colorbar(im, ax=ax, label="Presion")

    ax.set_title("Mapa tactil - Captura " + str(capture_id))
    ax.set_xlabel("X")
    ax.set_ylabel("Y")

    archivo = os.path.join(CARPETA_SALIDA, f"capture_{capture_id}.png")

    plt.savefig(archivo, bbox_inches="tight", dpi=300)
    plt.close(fig)

    return archivo


@app.route('/capture', methods=['POST'])
def recibir_captura():

    try:
        datos = request.get_json()

        if datos is None:
            return jsonify({"error": "No se recibio JSON"}), 400

        capture_id = datos["capture_id"]
        ancho = datos["width"]
        alto = datos["height"]
        matriz = datos["data"]

        print("\n==================================")
        print("Captura recibida:", capture_id)
        print("Dimensiones:", str(ancho) + "x" + str(alto))

        if len(matriz) != alto:
            return jsonify({"error": "Altura incorrecta"}), 400

        for fila in matriz:
            if len(fila) != ancho:
                return jsonify({"error": "Anchura incorrecta"}), 400

        matriz_np = np.array(matriz)
        archivo = generarImagen(capture_id, matriz_np)

        print("Imagen guardada:", archivo)
        print("==================================")

        return jsonify({"estado": "ok", "archivo": archivo})

    except KeyError as e:
        return jsonify({"error": f"Campo faltante: {e}"}), 400

    except Exception as e:
        return jsonify({"error": str(e)}), 500


if __name__ == '__main__':
    print("Servidor Flask iniciado")
    print("http://127.0.0.1:5000")
    app.run(host='127.0.0.1', port=5000, debug=False)
