import matplotlib.pyplot as plt

def visualizar_datos(nombre_archivo):
    try:
        # Abrir el archivo de texto y leer los datos
        with open(nombre_archivo, 'r') as f:
            lines = f.readlines()

        # Procesar los datos en columnas
        x = []
        y1 = []
        y2 = []
        y3 = []
        for line in lines:
            data = line.strip().split()  # Dividir la línea por espacios en blanco
            if len(data) >= 4:  # Verificar que haya al menos 4 elementos en la lista
                x.append(float(data[0]))
                y1.append(float(data[1]))
                y2.append(float(data[2]))
                y3.append(float(data[3]))

        # Trazar las curvas
        plt.plot(x, y1, label='$Potencia$')
        plt.plot(x, y2, label='$Amplitud Media$')
        plt.plot(x, y3, label='$Tasa Cruces por Zero$')

        # Configurar el gráfico
        plt.xlabel('x')
        plt.ylabel('Valores')
        plt.title('Datos del Archivo')
        plt.grid(True)
        plt.legend()
        plt.show()
    except Exception as e:
        print("Error al visualizar los datos:", e)

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description='Visualizar datos desde un archivo de texto.')
    parser.add_argument('archivo', type=str, help='Nombre del archivo de texto')
    args = parser.parse_args()

    visualizar_datos(args.archivo)

