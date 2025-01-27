#include "funciones.h"
#include <iostream>
#include <limits>

int main() {
    int opcion = 0;

    do {
        std::cout << "=== MENU SICUE (Consola) ===\n";
        std::cout << "1. Inscribir Usuario\n";
        std::cout << "2. Agregar Plan\n";
        std::cout << "3. Consultar Inscripcion\n";
        std::cout << "4. Mostrar Inscritos\n";
        std::cout << "5. Mostrar Planes\n";
        std::cout << "6. Ejecutar pruebas\n";
        std::cout << "0. Salir\n";
        std::cout << "Seleccione una opción: ";
        std::cin >> opcion;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // limpiar buffer

        switch(opcion) {
        case 1:
            inscribirUsuario();
            break;
        case 2:
            agregarPlan();
            break;
        case 3:
            consultarInscripcion();
            break;
        case 4:
            mostrarInscritos();
            break;
        case 5:
            mostrarPlanes();
            break;
        case 6:
            pruebasErrores();
            break;
        case 0:
            std::cout << "Saliendo...\n";
            break;
        default:
            std::cout << "Opción inválida.\n";
            break;
        }
    } while(opcion != 0);

    return 0;
}
