#include "funciones.h"
#include <algorithm>    // std::transform
#include <cctype>       // std::isspace
#include <sstream>      // std::stringstream
#include <limits>       // std::numeric_limits
#include <iostream>     // std::cin, std::cout

// ------------------------------------------------------------------------
// Funciones auxiliares de strings
// ------------------------------------------------------------------------
std::string trim(const std::string &s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(*start)) {
        ++start;
    }

    auto end = s.end();
    do {
        --end;
    } while (std::distance(start, end) > 0 && std::isspace(*end));

    return std::string(start, end + 1);
}

std::string toLower(const std::string &s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// ------------------------------------------------------------------------
// Lógica de negocio
// ------------------------------------------------------------------------
bool existeEnFichero(const std::string &dato, const std::string &fichero) {
    std::ifstream archivo(fichero);
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el fichero: " << fichero << "\n";
        return false;
    }

    std::string linea;
    while (std::getline(archivo, linea)) {
        if (toLower(trim(linea)) == toLower(trim(dato))) {
            return true;
        }
    }
    return false;
}

bool perteneceAUniversidad(const std::string &universidad) {
    return existeEnFichero(universidad, "universidad.txt");
}

bool existeCarrera(const std::string &carrera) {
    return existeEnFichero(carrera, "carrera.txt");
}

bool existePlan(const std::string &nombrePlan) {
    std::ifstream archivo("planes.txt");
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el fichero de planes.\n";
        return false;
    }

    std::string linea;
    while (std::getline(archivo, linea)) {
        // Buscar el nombre del plan antes de los dos puntos ":"
        size_t pos = linea.find(":");
        if (pos != std::string::npos) {
            std::string plan = linea.substr(0, pos);  // El nombre del plan está antes de ":"
            plan = trim(toLower(plan));  // Limpiar y convertir a minúsculas

            // Comparar con el que buscamos
            if (plan == toLower(trim(nombrePlan))) {
                return true;
            }
        }
    }
    return false;
}

std::string obtenerFechaActual() {
    std::time_t t = std::time(nullptr);
    char fecha[20];
    std::strftime(fecha, sizeof(fecha), "%Y-%m-%d", std::localtime(&t));
    return std::string(fecha);
}

// ------------------------------------------------------------------------
// Funciones de inscripción / planes
// ------------------------------------------------------------------------
void inscribirUsuario() {
    Usuario nuevoUsuario; 
    std::string dni, nombrePlan;
    char tipoUsuario; // 'A' para Alumno, 'P' para Profesor

    // 1. Ingresar el DNI
    std::cout << "Ingrese el DNI del Usuario: ";
    std::getline(std::cin, dni);

    if (!existeEnFichero(dni, "Usuarios.txt")) {
        std::cout << "El Usuario no cumple los requisitos para cursar el programa SICUE.\n\n";
        return;
    }

    // 2. Tipo de usuario
    std::cout << "Seleccione el tipo de usuario (A para Alumno, P para Profesor): ";
    std::cin >> tipoUsuario;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // limpiar buffer

    if (tipoUsuario != 'A' && tipoUsuario != 'P') {
        std::cout << "Opción no válida. Debe ser 'A' o 'P'.\n\n";
        return;
    }

    // 3. Nombre
    std::cout << "Ingrese el nombre: ";
    std::getline(std::cin, nuevoUsuario.nombre);

    // 4. Apellidos
    std::cout << "Ingrese los apellidos: ";
    std::getline(std::cin, nuevoUsuario.apellidos);

    // 5. Correo
    std::cout << "Ingrese el correo electrónico: ";
    std::getline(std::cin, nuevoUsuario.correo);

    // 6. Plan
    std::cout << "Ingrese el nombre del plan: ";
    std::getline(std::cin, nombrePlan);
    if (!existePlan(nombrePlan)) {
        std::cout << "El plan no existe.\n\n";
        return;
    }

    // 7. Duración
    int cuatrimestre;
    std::cout << "Ingrese la duración del plan (1 para 1 cuatrimestre, 2 para 2 cuatrimestres): ";
    std::cin >> cuatrimestre;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (cuatrimestre != 1 && cuatrimestre != 2) {
        std::cout << "Opción no válida. Debe ser 1 o 2.\n\n";
        return;
    }

    // 8. Confirmación
    char confirmacion;
    std::cout << "¿Desea confirmar la inscripción? (S/N): ";
    std::cin >> confirmacion;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (confirmacion == 'S' || confirmacion == 's') {
        nuevoUsuario.dni = dni;
        nuevoUsuario.estadoInscripcion = "Confirmado";
        nuevoUsuario.fechaInscripcion = obtenerFechaActual();
        nuevoUsuario.tipoUsuario = (tipoUsuario == 'A') ? "Alumno" : "Profesor";

        // Guardar en inscritos.txt
        std::ofstream archivoInscritos("inscritos.txt", std::ios::app);
        if (!archivoInscritos) {
            std::cerr << "Error al abrir inscritos.txt.\n";
            return;
        }

        archivoInscritos << nuevoUsuario.dni << ", "
                         << nuevoUsuario.nombre << " " << nuevoUsuario.apellidos << ", "
                         << nuevoUsuario.correo << ", "
                         << nombrePlan << ", "
                         << cuatrimestre << " cuatrimestre(s), "
                         << nuevoUsuario.tipoUsuario << "\n";

        std::cout << "Inscripción confirmada correctamente.\n\n";
    } else {
        std::cout << "Inscripción cancelada.\n\n";
    }
}

void agregarPlan() {
    std::string nombrePlan, universidadOrigen, universidadDestino, carrera;

    // 1. Nombre del plan
    std::cout << "Ingrese el nombre del plan: ";
    std::getline(std::cin, nombrePlan);

    // 2. Universidad de origen
    std::cout << "Ingrese la universidad de origen: ";
    std::getline(std::cin, universidadOrigen);
    if (!perteneceAUniversidad(universidadOrigen)) {
        std::cout << "La universidad de origen no se encuentra en el programa SICUE.\n\n";
        return;
    }

    // 3. Universidad de destino
    std::cout << "Ingrese la universidad de destino: ";
    std::getline(std::cin, universidadDestino);
    if (!perteneceAUniversidad(universidadDestino)) {
        std::cout << "La universidad de destino no se encuentra en el programa SICUE.\n\n";
        return;
    }

    // 4. Carrera
    std::cout << "Ingrese la carrera asociada al plan: ";
    std::getline(std::cin, carrera);
    if (!existeCarrera(carrera)) {
        std::cout << "La carrera no existe en el sistema.\n\n";
        return;
    }

    // 5. Asignaturas
    std::vector<std::string> asignaturas;
    std::string asignatura;
    char agregarOtra;

    std::cout << "Ingrese las asignaturas que desea convalidar:\n";
    do {
        std::cout << "Asignatura: ";
        std::getline(std::cin, asignatura);
        asignaturas.push_back(asignatura);

        std::cout << "¿Desea agregar otra asignatura? (S/N): ";
        std::cin >> agregarOtra;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } while (agregarOtra == 'S' || agregarOtra == 's');

    // 6. Guardar en planes.txt
    std::ofstream archivo("planes.txt", std::ios::app);
    if (!archivo) {
        std::cerr << "Error al abrir planes.txt.\n";
        return;
    }

    archivo << nombrePlan << ": "
            << universidadOrigen << " --> " << universidadDestino
            << ", Carrera: " << carrera << "\n";
    archivo << "Asignaturas convalidadas:\n";
    for (const auto &asig : asignaturas) {
        archivo << "- " << asig << "\n";
    }
    archivo << "\n\n";

    std::cout << "Plan agregado correctamente.\n\n";
}

void consultarInscripcion() {
    std::string dni;
    std::cout << "Ingrese el DNI del Usuario: ";
    std::getline(std::cin, dni);

    std::ifstream archivo("inscritos.txt");
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir inscritos.txt.\n";
        return;
    }

    std::string linea;
    bool encontrado = false;

    while (std::getline(archivo, linea)) {
        if (linea.empty()) continue;

        // El DNI se asume hasta la primera coma
        size_t pos = linea.find(", ");
        if (pos != std::string::npos) {
            std::string dniUsuario = linea.substr(0, pos);
            if (dniUsuario == dni) {
                encontrado = true;
                std::cout << "Datos de inscripción:\n" << linea << "\n\n";
                break;
            }
        }
    }
    if (!encontrado) {
        std::cout << "Usuario con DNI " << dni << " no encontrado.\n\n";
    }
}

void mostrarInscritos() {
    std::ifstream archivo("inscritos.txt");
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir inscritos.txt.\n";
        return;
    }

    std::string linea;
    bool hayDatos = false;

    std::cout << "--- Lista de inscritos ---\n";
    while (std::getline(archivo, linea)) {
        if (!linea.empty()) {
            std::cout << linea << "\n";
            hayDatos = true;
        }
    }
    if (!hayDatos) {
        std::cout << "No hay inscritos aún.\n";
    }
    std::cout << "\n";
}

void mostrarPlanes() {
    std::ifstream archivo("planes.txt");
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir planes.txt.\n";
        return;
    }

    std::string linea;
    bool hayDatos = false;

    std::cout << "--- Listado de planes ---\n";
    while (std::getline(archivo, linea)) {
        std::cout << linea << "\n";
        hayDatos = true;
    }
    if (!hayDatos) {
        std::cout << "No hay planes registrados.\n";
    }
    std::cout << "\n";
}

// ------------------------------------------------------------------------
// Pruebas y funciones de testeo
// ------------------------------------------------------------------------
void pruebaPerteneceAUniversidad() {
    std::cout << "Prueba: perteneceAUniversidad\n";
    assert(perteneceAUniversidad("Universidad de Cordoba") == true);
    std::cout << "La Universidad de Cordoba se encuentra.\n";
    assert(perteneceAUniversidad("Universidad de Zaragoza") == true);
    std::cout << "La Universidad de Zaragoza se encuentra.\n";
    assert(perteneceAUniversidad("Universidad de Saturno") == false);
    std::cout << "La Universidad de Saturno NO se encuentra.\n";
    std::cout << "OK: pruebaPerteneceAUniversidad\n\n";
}

void pruebaExisteCarrera() {
    std::cout << "Prueba: existeCarrera\n";
    assert(existeCarrera("Ingenieria Informatica") == true);
    std::cout << "Ingenieria Informatica existe.\n";
    assert(existeCarrera("Psicologia") == true);
    std::cout << "Psicologia existe.\n";
    assert(existeCarrera("Astronomia") == false);
    std::cout << "Astronomia NO existe.\n";
    std::cout << "OK: pruebaExisteCarrera\n\n";
}

void pruebaObtenerFechaActual() {
    std::cout << "Prueba: obtenerFechaActual\n";
    std::string fecha = obtenerFechaActual();
    // formato: AAAA-MM-DD (10 chars)
    assert(fecha.size() == 10);
    std::cout << "Fecha obtenida: " << fecha << "\n";
    std::cout << "OK: pruebaObtenerFechaActual\n\n";
}

void pruebaGenerarFichero() {
    std::cout << "Prueba: Generar un fichero\n";
    std::string nombreFichero = "fichero_prueba.txt";
    {
        std::ofstream archivo(nombreFichero);
        archivo << "DNI: 12345678A\n";
        archivo << "Nombre: Fernando\n";
        archivo << "Correo: fernandoalonso@uco.es\n";
    }
    // Verificamos
    std::ifstream archivoVerificacion(nombreFichero);
    assert(archivoVerificacion.is_open());
    bool encontrado = false;
    std::string linea;
    while (std::getline(archivoVerificacion, linea)) {
        if (linea == "DNI: 12345678A") {
            encontrado = true;
            break;
        }
    }
    assert(encontrado && "No se encontró el contenido esperado.");
    std::cout << "Fichero de prueba generado y verificado.\n\n";
}

void pruebasErrores(){
    std::cout << "\n--- Ejecutando pruebas...\n";

    // Prueba 1
    pruebaPerteneceAUniversidad();

    // Prueba 2
    pruebaExisteCarrera();

    // Prueba 3
    std::cout << "Intentar agregar un plan con universidad de destino inexistente.\n";
    agregarPlan();

    // Prueba 4
    std::cout << "Intentar hacer un plan con carrera inválida.\n";
    agregarPlan();

    // Prueba 5
    std::cout << "Consultar un Usuario con DNI inexistente.\n";
    consultarInscripcion();

    // Prueba 6
    std::cout << "Intentar inscribir con más de 2 cuatrimestres.\n";
    inscribirUsuario();

    // Prueba 7
    std::cout << "Cancelar la inscripción.\n";
    inscribirUsuario();

    // Prueba 8
    pruebaObtenerFechaActual();

    // Prueba 9
    pruebaGenerarFichero();

    std::cout << "--- Pruebas completadas ---\n";
}
