#include "funciones.h"

int main() {
    int opcion;
    do {
        std::cout << "\n--- Sistema Gestion SICUE ---\n";
        std::cout << "1. Inscribir Usuario\n";
        std::cout << "2. Agregar plan de convalidacion\n";
        std::cout << "3. Consultar inscripcion\n";
        std::cout << "4. Mostrar lista de inscritos\n";
        std::cout << "5. Ejecutar pruebas de error\n";
        std::cout << "6. Mostrar planes\n";  
        std::cout << "7. Salir\n\n";
        std::cout << "Seleccione una opcion: ";
        std::cin >> opcion;
        std::cin.ignore();
        std::cout << "\n";

        switch (opcion) {
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
                pruebasErrores();
                break;
            case 6:
                mostrarPlanes();  
                break;
            case 7:
                std::cout << "Saliendo del sistema...\n";
                break;
            default:
                std::cout << "Opción no válida. Intente de nuevo.\n";
        }
    } while (opcion != 7);

    return 0;
}
