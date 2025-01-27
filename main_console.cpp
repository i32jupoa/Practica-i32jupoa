#include <iostream>
#include <limits>

#include "funciones.h"

int main() {
    int opcion = 0;

    do {
        std::cout << "==========================================\n";
        std::cout << "   MENU SICUE (Consola) \n";
        std::cout << "==========================================\n";
        std::cout << "1. Inscribir Usuario\n";
        std::cout << "2. Agregar Plan de Convalidacion\n";
        std::cout << "3. Consultar Inscripcion\n";
        std::cout << "4. Mostrar Inscritos\n";
        std::cout << "5. Mostrar Planes\n";
        std::cout << "6. Matricular Estudiantes\n";
        std::cout << "7. Salir\n";
        std::cout << "(Opcion oculta) 8. Ejecutar pruebas\n";
        std::cout << "Seleccione una opcion: ";
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
                matricularEstudiantes();
                break;
            case 7:
                std::cout << "Saliendo...\n";
                break;
            case 8:
                std::cout << "Ejecutando pruebas...\n";
                pruebasErrores();
                break;
            default:
                std::cout << "Opcion invalida.\n";
                break;
        }
    } while(opcion != 7);

    return 0;
}