#include <math.h>
#include <fcntl.h>
#include <time.h>
#include <C_General.hpp>
#include <C_Trace.hpp>
#include <C_File.hpp>
#include <C_Arguments.hpp>
#include <C_Matrix.hpp>
#include <C_Image.hpp>
#include <iostream>
#include <vector>
#include <map>


// Declaración del prototipo de la función
void convolucion(C_Image& matriz, const std::vector<std::vector<int>>& mascara, double& divisor);


std::string obtenerNombreImagen() {
	char nombreImagen[100];
	printf("Introduce el nombre de la imagen que quiere procesar: ");
	scanf_s("%s", nombreImagen, 100);
	return std::string(nombreImagen) + ".bmp";
}

std::string obtenerNombreImagenResultante() {
	char nombreImagen[100];
	printf("Introduzca el nombre de la imagen resultante: ");
	scanf_s("%s", nombreImagen, 100);
	return std::string(nombreImagen) + "_Mod.bmp";
}

std::string nombreImagen = obtenerNombreImagen();

int main(int argc, char** argv)
{
	C_Image matriz;
	matriz.Read(nombreImagen.c_str());

	bool continuar = true;
	bool aplicarConv = true;
	double divisor = 1;
	std::vector<std::vector<int>> mascara;

	while (continuar) {

		// Mostrar opciones para elegir el tipo de máscara o kernel
		std::cout << "\nSeleccione el tipo de mascara:\n";
		std::cout << "1. Laplaciano\n";
		std::cout << "2. Desenfoque\n";
		std::cout << "3. Desenfoque Gaussiano\n";
		std::cout << "4. Enfocar\n";
		std::cout << "5. Realzar\n";
		std::cout << "6. Sobel horizontal\n";
		std::cout << "7. Sobel vertical\n";
		std::cout << "8. Personalizar Kernel\n";
		std::cout << "9. Otra imagen\n";
		std::cout << "10. Salir\n";
		std::cout << "\nElige Opcion: ";

		int opcion;
		std::cin >> opcion;


		// Construir la máscara según la elección del usuario (válido para cualquier tamaño de máscara)
		switch (opcion) {
		case 1: //convolución basica laplaciano
			mascara = { {-1, -1, -1},
						{-1, 8, -1},
						{-1, -1, -1} };
			divisor = 1;
			aplicarConv = true;
			break;
		case 2: //Desenfoque (filtro mediana)
			mascara = { {1, 1, 1},
						{1, 1, 1},
						{1, 1, 1} };
			divisor = 9;
			aplicarConv = true;
			break;
		case 3: //Gaussiano
			mascara = { {1, 4, 6, 4, 1},
						{4, 16, 24, 16, 4},
						{6, 24, 36, 24, 6},
						{4, 16, 24, 16, 4},
						{1, 4, 6, 4, 1} };
			divisor = 256;
			aplicarConv = true;
			break;
		case 4: //Enfocar
			mascara = { {0, -1, 0},
						{-1, 5, -1},
						{0, -1, 0} };
			divisor = 1;
			aplicarConv = true;
			break;
		case 5:  //Realzar
			mascara = { {-2, -1, 0},
						{-1, 1, 1},
						{0, 1, 2} };
			divisor = 1;
			aplicarConv = true;
			break;
		case 6: //Sobel horizontal
			mascara = { {-1, -2, -1},
						{0, 0, 0},
						{1, 2, 1} };
			divisor = 1;
			aplicarConv = true;
			break;
		case 7: //sobel vertical
			mascara = { {-1, 0, 1},
						{-2, 0, 2},
						{-1, 0, 1} };
			divisor = 1;
			aplicarConv = true;
			break;

		case 8: // Matriz personalizada
			int filas, columnas;
			std::cout << "Ingrese el numero de filas de la matriz: ";
			std::cin >> filas;
			std::cout << "Ingrese el numero de columnas de la matriz: ";
			std::cin >> columnas;

			mascara.resize(filas);
			for (int i = 0; i < filas; ++i) {
				mascara[i].resize(columnas);
				for (int j = 0; j < columnas; ++j) {
					std::cout << "Ingrese el valor para la posicion [" << i << "][" << j << "]: ";
					std::cin >> mascara[i][j];
				}
			}

			// Mostrar la matriz personalizada
			std::cout << "El kernel personalizado es:\n";
			for (int i = 0; i < filas; ++i) {
				for (int j = 0; j < columnas; ++j) {
					std::cout << mascara[i][j] << " ";
				}
				std::cout << std::endl;
			}

			// Verificar que la matriz personalizada sea de tamaño impar
			if (filas % 2 == 0 || columnas % 2 == 0) {
				std::cerr << "La matriz personalizada debe tener un numero impar de filas y columnas." << std::endl;
				break;
			}

			std::cout << "Ingrese el divisor: ";
			std::cin >> divisor;

			if (divisor > 0)
			{
				aplicarConv = true;

			}
			else {
				std::cerr << "El kernel debe tener un divisor mayor que 0." << std::endl;
				break;
			}

			break;

		case 9:
			nombreImagen = obtenerNombreImagen();
			matriz.Read(nombreImagen.c_str());
			aplicarConv = false;
			continue;
			//break;

		case 10:
			continuar = false;
			aplicarConv = false;
			break;

		default:
			std::cerr << "Opción no válida." << std::endl;
			return 1; // Salir del programa con error
		}

		if (opcion != 10 && opcion != 9 && aplicarConv) {
			// Llamar a la función de convolución con la máscara seleccionada
			convolucion(matriz, mascara, divisor);
		}
	}

	return 0;
}

void convolucion(C_Image & matriz, const std::vector<std::vector<int>>&mascara, double& divisor) {
	printf("\n\nAplicando filtro . . .\n\n");
	C_Image matrizResultante;
	matrizResultante.Read(nombreImagen.c_str());

	double valor = 0;
	int medioMascaraX = mascara.size() / 2;
	int medioMascaraY = mascara[0].size() / 2;

	// Recorrer la matriz utilizando bucles for
	for (int i = matriz.FirstRow(); i <= matriz.LastRow(); ++i) {
		for (int j = matriz.FirstCol(); j <= matriz.LastCol(); ++j) {

			valor = 0; //Reestablecer valor a 0
			// Aplicar la convolución en la posición (i, j)
			for (int mi = 0; mi < mascara.size(); ++mi) {
				for (int mj = 0; mj < mascara[0].size(); ++mj) {

					// Calcula las coordenadas de la imagen correspondientes a la posición actual de la máscara (centrar)
					int x = i + mi - medioMascaraX;
					int y = j + mj - medioMascaraY;

					// Ajustar las coordenadas (x, y) para que se refieran al píxel más cercano dentro de los límites (control de bordes)
					x = (x < matriz.FirstRow()) ? matriz.FirstRow() : (x > matriz.LastRow()) ? matriz.LastRow() : x;
					y = (y < matriz.FirstCol()) ? matriz.FirstCol() : (y > matriz.LastCol()) ? matriz.LastCol() : y;

					// Aplicar la operación para ese punto
					valor += matriz(x, y) * (mascara[mi][mj] / divisor);
				}
			}

			// Truncar el valor a los límites [0, 255]
			valor = (valor > 255) ? 255 : (valor < 0) ? 0 : valor;

			// Asignar el valor resultante a la matrizResultante en la posición (i, j)
			matrizResultante(i, j) = valor;
		}
	}

	printf("\n\nFiltro aplicado\n\n");
	C_Image	resultado(matrizResultante);
	std::string nombreImagenResult = obtenerNombreImagenResultante();
	matrizResultante.WriteBMP(nombreImagenResult.c_str());

}