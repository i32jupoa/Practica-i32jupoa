#include "funciones.h"

#include <algorithm>    // std::transform
#include <cctype>       // std::isspace
#include <ctime>        // std::time, std::localtime, std::strftime
#include <cassert>      // assert
#include <fstream>      // std::ifstream, std::ofstream
#include <iostream>     // std::cin, std::cout, std::cerr
#include <limits>       // std::numeric_limits
#include <sstream>      // std::stringstream
#include <vector>

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
// Verificaciones en ficheros
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
        // Se busca "<nombrePlan>:" en cada línea
        size_t pos = linea.find(":");
        if (pos != std::string::npos) {
            std::string plan = trim(toLower(linea.substr(0, pos)));
            if (plan == toLower(trim(nombrePlan))) {
                return true;
            }
        }
    }
    return false;
}

// ------------------------------------------------------------------------
// Obtener fecha actual
// ------------------------------------------------------------------------
std::string obtenerFechaActual() {
    std::time_t t = std::time(nullptr);
    char fecha[20];
    std::strftime(fecha, sizeof(fecha), "%Y-%m-%d", std::localtime(&t));
    return std::string(fecha);
}

// ------------------------------------------------------------------------
// Inscribir Usuario
// ------------------------------------------------------------------------
void inscribirUsuario() {
    Usuario nuevoUsuario; 
    std::string dni, nombrePlan;
    char tipoUsuario; // 'A' (Alumno) o 'P' (Profesor), heredado de la versión previa

    // 1. Ingresar DNI
    std::cout << "1) Ingrese el DNI del Usuario: ";
    std::getline(std::cin, dni);

    // Mantener el error anterior si el DNI no está en Usuarios.txt
    if (!existeEnFichero(dni, "Usuarios.txt")) {
        std::cout << "El Usuario no cumple los requisitos (DNI no encontrado en Usuarios.txt).\n\n";
        return;
    }

    // 2. Tipo de usuario (heredado de la versión anterior)
    std::cout << "Seleccione el tipo de usuario (A para Alumno, P para Profesor): ";
    std::cin >> tipoUsuario;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (tipoUsuario != 'A' && tipoUsuario != 'P') {
        std::cout << "Opción no válida. Debe ser 'A' o 'P'.\n\n";
        return;
    }

    // 3. Nombre
    std::cout << "2) Ingrese el nombre: ";
    std::getline(std::cin, nuevoUsuario.nombre);

    // 4. Apellidos
    std::cout << "3) Ingrese los apellidos: ";
    std::getline(std::cin, nuevoUsuario.apellidos);

    // 5. Correo
    std::cout << "4) Ingrese el correo electrónico: ";
    std::getline(std::cin, nuevoUsuario.correo);

    // 6. Plan de convalidación
    std::cout << "5) Ingrese el nombre del plan de convalidacion: ";
    std::getline(std::cin, nombrePlan);
    if (!existePlan(nombrePlan)) {
        std::cout << "El plan no existe en planes.txt.\n\n";
        return;
    }

    // 7. 1 o 2 cuatrimestres
    int cuatrimestre;
    std::cout << "6) Ingrese la duración (1 para 1 cuatrimestre, 2 para 2 cuatrimestres): ";
    std::cin >> cuatrimestre;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (cuatrimestre != 1 && cuatrimestre != 2) {
        std::cout << "Opción no válida. Debe ser 1 o 2.\n\n";
        return;
    }

    // 8. Confirmación
    char confirmacion;
    std::cout << "7) ¿Desea confirmar la inscripción? (S/N): ";
    std::cin >> confirmacion;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (confirmacion == 'S' || confirmacion == 's') {
        // Se guarda en preinscritos.txt con estado "en proceso"
        nuevoUsuario.dni = dni;
        nuevoUsuario.estadoInscripcion = "en proceso";
        nuevoUsuario.fechaInscripcion = obtenerFechaActual();
        nuevoUsuario.tipoUsuario = (tipoUsuario == 'A') ? "Alumno" : "Profesor";

        std::ofstream archivoPre("preinscritos.txt", std::ios::app);
        if (!archivoPre) {
            std::cerr << "Error al abrir preinscritos.txt.\n";
            return;
        }

        archivoPre << nuevoUsuario.dni << ", "
                   << nuevoUsuario.nombre << " " << nuevoUsuario.apellidos << ", "
                   << nuevoUsuario.correo << ", "
                   << nombrePlan << ", "
                   << cuatrimestre << " cuatrimestre(s), "
                   << nuevoUsuario.estadoInscripcion << ", "
                   << nuevoUsuario.fechaInscripcion << ", "
                   << nuevoUsuario.tipoUsuario << "\n";

        std::cout << "Se ha inscrito al usuario en preinscritos.txt (en proceso).\n\n";
    } else {
        std::cout << "Inscripción cancelada.\n\n";
    }
}

// ------------------------------------------------------------------------
// Agregar Plan de Convalidación
// ------------------------------------------------------------------------
void agregarPlan() {
    std::string nombrePlan;
    std::string universidadOrigen;
    std::string universidadDestino;
    std::string carrera;

    // 1. Nombre del plan
    std::cout << "1) Ingrese el nombre del plan: ";
    std::getline(std::cin, nombrePlan);

    // (Opcional) Se puede chequear si ya existe un plan con ese nombre
    if (existePlan(nombrePlan)) {
        std::cout << "Ya existe un plan con ese nombre. Se recomienda usar otro.\n";
    }

    // 2. Universidad de Origen (debe ser la de Córdoba)
    std::cout << "2) Ingrese la universidad de origen (debe ser 'Universidad de Cordoba'): ";
    std::getline(std::cin, universidadOrigen);

    if (!perteneceAUniversidad(universidadOrigen)) {
        std::cout << "La universidad de origen no se encuentra en el programa SICUE.\n\n";
        return;
    }
    if (toLower(trim(universidadOrigen)) != "universidad de cordoba"
        && toLower(trim(universidadOrigen)) != "universidad de córdoba")
    {
        std::cout << "La universidad de origen debe ser la de Córdoba.\n\n";
        return;
    }

    // 3. Universidad de Destino (todas menos Córdoba)
    std::cout << "3) Ingrese la universidad de destino (no puede ser la de Córdoba): ";
    std::getline(std::cin, universidadDestino);

    if (!perteneceAUniversidad(universidadDestino)) {
        std::cout << "La universidad de destino no se encuentra en el programa SICUE.\n\n";
        return;
    }
    if (toLower(trim(universidadDestino)) == "universidad de cordoba"
        || toLower(trim(universidadDestino)) == "universidad de córdoba")
    {
        std::cout << "La universidad de destino no puede ser la de Córdoba.\n\n";
        return;
    }

    // 4. Carrera asociada
    std::cout << "4) Ingrese la carrera asociada al plan: ";
    std::getline(std::cin, carrera);
    if (!existeCarrera(carrera)) {
        std::cout << "La carrera no existe en el sistema (carrera.txt).\n\n";
        return;
    }

    // 5. Asignaturas
    std::vector<std::string> asignaturas;
    bool seAgregoAlMenosUna = false;
    std::string asignatura;
    char agregarOtra;
    std::cout << "5) Ingrese las asignaturas que desea convalidar.\n";
    do {
        std::cout << "   Asignatura (o pulse Enter sin texto para terminar): ";
        std::getline(std::cin, asignatura);

        if (!asignatura.empty()) {
            asignaturas.push_back(asignatura);
            seAgregoAlMenosUna = true;
        }

        std::cout << "¿Desea agregar otra asignatura? (S/N): ";
        std::cin >> agregarOtra;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } while (agregarOtra == 'S' || agregarOtra == 's');

    if (!seAgregoAlMenosUna) {
        std::cout << "Error: No se ha introducido ninguna asignatura.\n\n";
        return;
    }

    // 6. Confirmar el plan
    char confirmarPlan;
    std::cout << "6) ¿Desea confirmar el plan? (S/N): ";
    std::cin >> confirmarPlan;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (confirmarPlan != 'S' && confirmarPlan != 's') {
        std::cout << "Se ha cancelado la creación del plan.\n\n";
        return;
    }

    // Guardar en planes.txt
    std::ofstream archivo("planes.txt", std::ios::app);
    if (!archivo) {
        std::cerr << "Error al abrir planes.txt.\n";
        return;
    }

    archivo << nombrePlan << ": "
            << universidadOrigen << " --> " << universidadDestino
            << ", Carrera: " << carrera << "\n";
    archivo << "Asignaturas convalidadas:\n";
    for (auto &asig : asignaturas) {
        archivo << "- " << asig << "\n";
    }
    archivo << "\n\n";

    std::cout << "Plan agregado correctamente en planes.txt.\n\n";
}

// ------------------------------------------------------------------------
// Consultar Inscripción
// ------------------------------------------------------------------------
void consultarInscripcion() {
    std::string dni;
    std::cout << "Ingrese el DNI del Usuario para consultar: ";
    std::getline(std::cin, dni);

    // 1) Buscar en preinscritos.txt => "en proceso"
    {
        std::ifstream archivoPre("preinscritos.txt");
        if (!archivoPre.is_open()) {
            std::cerr << "Error al abrir preinscritos.txt.\n";
        } else {
            std::string linea;
            while (std::getline(archivoPre, linea)) {
                if (linea.empty()) continue;
                size_t pos = linea.find(", ");
                if (pos != std::string::npos) {
                    std::string dniUsuario = trim(linea.substr(0, pos));
                    if (dniUsuario == trim(dni)) {
                        std::cout << "\nEl usuario con DNI " << dni << " se encuentra en preinscritos.txt\n";
                        std::cout << "Estado de la inscripcion: EN PROCESO\n";

                        // Extraer la fecha (7mo campo en la línea)
                        int comaCount = 0;
                        size_t idx = 0;
                        while (comaCount < 6 && idx < linea.size()) {
                            if (linea[idx] == ',') comaCount++;
                            idx++;
                        }
                        while (idx < linea.size() && std::isspace(linea[idx])) idx++;
                        std::string resto = linea.substr(idx);
                        size_t pos2 = resto.find(",");
                        std::string fecha;
                        if (pos2 != std::string::npos) {
                            fecha = trim(resto.substr(0, pos2));
                        } else {
                            fecha = trim(resto);
                        }
                        std::cout << "Fecha de inscripcion: " << fecha << "\n\n";
                        return;
                    }
                }
            }
        }
    }

    // 2) Buscar en inscritos.txt => "confirmada"
    {
        std::ifstream archivoIns("inscritos.txt");
        if (!archivoIns.is_open()) {
            std::cerr << "Error al abrir inscritos.txt.\n";
        } else {
            std::string linea;
            while (std::getline(archivoIns, linea)) {
                if (linea.empty()) continue;
                size_t pos = linea.find(", ");
                if (pos != std::string::npos) {
                    std::string dniUsuario = trim(linea.substr(0, pos));
                    if (dniUsuario == trim(dni)) {
                        std::cout << "\nEl usuario con DNI " << dni << " se encuentra en inscritos.txt\n";
                        std::cout << "Estado de la inscripcion: CONFIRMADA\n";

                        // Extraer fecha (7mo campo)
                        int comaCount = 0;
                        size_t idx = 0;
                        while (comaCount < 6 && idx < linea.size()) {
                            if (linea[idx] == ',') comaCount++;
                            idx++;
                        }
                        while (idx < linea.size() && std::isspace(linea[idx])) idx++;
                        std::string resto = linea.substr(idx);
                        size_t pos2 = resto.find(",");
                        std::string fecha;
                        if (pos2 != std::string::npos) {
                            fecha = trim(resto.substr(0, pos2));
                        } else {
                            fecha = trim(resto);
                        }
                        std::cout << "Fecha de inscripcion: " << fecha << "\n\n";
                        return;
                    }
                }
            }
        }
    }

    // 3) No encontrado
    std::cout << "Usuario con DNI " << dni << " no encontrado ni en preinscritos.txt ni en inscritos.txt.\n\n";
}

// ------------------------------------------------------------------------
// Mostrar Inscritos (lee de inscritos.txt)
// ------------------------------------------------------------------------
void mostrarInscritos() {
    std::ifstream archivo("inscritos.txt");
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir inscritos.txt.\n";
        return;
    }

    std::string linea;
    bool hayDatos = false;

    std::cout << "\n--- Lista de INSCRITOS (confirmados) ---\n";
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

// ------------------------------------------------------------------------
// Mostrar Planes (lee de planes.txt)
// ------------------------------------------------------------------------
void mostrarPlanes() {
    std::ifstream archivo("planes.txt");
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir planes.txt.\n";
        return;
    }

    std::string linea;
    bool hayDatos = false;

    std::cout << "\n--- Listado de Planes de Convalidación ---\n";
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
// Función auxiliar para extraer la universidad de destino de un plan
// ------------------------------------------------------------------------
static std::string obtenerUniversidadDestinoDelPlan(const std::string &nombrePlan) {
    std::ifstream archivo("planes.txt");
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir planes.txt.\n";
        return "";
    }

    std::string linea;
    while (std::getline(archivo, linea)) {
        if (linea.empty()) continue;

        // "<plan>:" ...
        size_t pos = linea.find(":");
        if (pos != std::string::npos) {
            std::string planEncontrado = trim(linea.substr(0, pos));
            if (toLower(planEncontrado) == toLower(nombrePlan)) {
                // Formato: "Plan1: UnivCordoba --> UnivZaragoza, Carrera: X"
                size_t flechaPos = linea.find("-->");
                if (flechaPos == std::string::npos) {
                    return "";
                }
                size_t startDestino = flechaPos + 3;
                size_t comaPos = linea.find(",", startDestino);
                std::string univDestino;
                if (comaPos != std::string::npos) {
                    univDestino = trim(linea.substr(startDestino, comaPos - startDestino));
                } else {
                    univDestino = trim(linea.substr(startDestino));
                }
                return univDestino;
            }
        }
    }
    return "";
}

// ------------------------------------------------------------------------
// Matricular Estudiantes:
//   1. Pide la universidad de destino
//   2. De preinscritos.txt, filtra aquellos cuyo plan => univ.destino = la pedida
//   3. Se seleccionan por índice los que se deseen matricular => se pasa a inscritos.txt
//      con estado "confirmada" y se elimina de preinscritos.txt
// ------------------------------------------------------------------------
void matricularEstudiantes() {
    std::string univDestino;
    std::cout << "1) Ingrese la universidad de destino para matricular: ";
    std::getline(std::cin, univDestino);

    // Leer preinscritos
    std::ifstream preFile("preinscritos.txt");
    if (!preFile.is_open()) {
        std::cerr << "Error al abrir preinscritos.txt.\n";
        return;
    }

    // Guardar en memoria todas las líneas
    std::vector<std::string> todasLineas;
    std::vector<std::string> candidatos;
    std::vector<int> indicesCandidatos;

    std::string linea;
    int lineIndex = 0;
    while (std::getline(preFile, linea)) {
        todasLineas.push_back(linea);

        if (!linea.empty()) {
            // Separar la línea para obtener el plan (índice 3)
            std::stringstream ss(linea);
            std::vector<std::string> partes;
            std::string tmp;
            while (std::getline(ss, tmp, ',')) {
                partes.push_back(trim(tmp));
            }
            if (partes.size() >= 4) {
                std::string planUsuario = partes[3]; // plan
                // Extraer la univ destino real de ese plan
                std::string univDelPlan = obtenerUniversidadDestinoDelPlan(planUsuario);
                if (toLower(trim(univDelPlan)) == toLower(trim(univDestino))) {
                    candidatos.push_back(linea);
                    indicesCandidatos.push_back(lineIndex);
                }
            }
        }
        lineIndex++;
    }
    preFile.close();

    if (candidatos.empty()) {
        std::cout << "\nNo hay usuarios preinscritos para la universidad de destino: "
                  << univDestino << "\n\n";
        return;
    }

    std::cout << "\nUsuarios preinscritos que tienen como universidad de destino '"
              << univDestino << "':\n";
    for (size_t i = 0; i < candidatos.size(); i++) {
        std::cout << i << ") " << candidatos[i] << "\n";
    }

    // Pedir índices a matricular
    std::cout << "\nIngrese los índices (separados por espacio) de los usuarios que desea matricular.\n";
    std::cout << "O presione ENTER sin texto para no matricular a nadie: ";

    std::string input;
    std::getline(std::cin, input);
    if (input.empty()) {
        std::cout << "No se ha matriculado a ningún usuario.\n\n";
        return;
    }

    std::stringstream ss2(input);
    std::vector<int> indicesSeleccionados;
    int idxSel;
    while (ss2 >> idxSel) {
        if (idxSel >= 0 && (size_t)idxSel < candidatos.size()) {
            indicesSeleccionados.push_back(idxSel);
        } else {
            std::cout << "Índice " << idxSel << " inválido, se ignora.\n";
        }
    }

    if (indicesSeleccionados.empty()) {
        std::cout << "No se ha matriculado a ningún usuario.\n\n";
        return;
    }

    // Se pasan a inscritos.txt
    std::ofstream insFile("inscritos.txt", std::ios::app);
    if (!insFile.is_open()) {
        std::cerr << "Error al abrir inscritos.txt.\n";
        return;
    }

    // Marcar para borrarlos de preinscritos
    std::vector<bool> borrar(todasLineas.size(), false);

    for (int idx : indicesSeleccionados) {
        int idxReal = indicesCandidatos[idx];
        std::string lin = todasLineas[idxReal];

        // Modificamos "en proceso" => "confirmada" (6to campo)
        std::vector<std::string> partes;
        {
            std::stringstream sst(lin);
            std::string p;
            while (std::getline(sst, p, ',')) {
                partes.push_back(trim(p));
            }
        }
        if (partes.size() > 5) {
            partes[5] = "confirmada"; // Cambiamos ese campo
        }

        // Reconstruimos
        std::stringstream reconst;
        for (size_t k = 0; k < partes.size(); k++) {
            reconst << partes[k];
            if (k < partes.size() - 1) reconst << ", ";
        }
        insFile << reconst.str() << "\n";

        borrar[idxReal] = true;
    }
    insFile.close();

    // Reescribir preinscritos.txt sin los que se matricularon
    {
        std::ofstream outPre("preinscritos.txt");
        if (!outPre.is_open()) {
            std::cerr << "Error al reescribir preinscritos.txt.\n";
            return;
        }
        for (size_t i = 0; i < todasLineas.size(); i++) {
            if (!borrar[i]) {
                outPre << todasLineas[i] << "\n";
            }
        }
    }

    std::cout << "\nSe han matriculado " << indicesSeleccionados.size()
              << " usuario(s) en '" << univDestino
              << "' y movido(s) a inscritos.txt.\n\n";
}

// ------------------------------------------------------------------------
// PRUEBAS DE ERRORES (se mantienen)
// ------------------------------------------------------------------------
static void pruebaPerteneceAUniversidad() {
    std::cout << "Prueba: perteneceAUniversidad\n";
    assert(perteneceAUniversidad("Universidad de Cordoba") == true);
    std::cout << "  La Universidad de Cordoba se encuentra.\n";
    assert(perteneceAUniversidad("Universidad de Zaragoza") == true);
    std::cout << "  La Universidad de Zaragoza se encuentra.\n";
    assert(perteneceAUniversidad("Universidad de Saturno") == false);
    std::cout << "  La Universidad de Saturno NO se encuentra.\n";
    std::cout << "OK: pruebaPerteneceAUniversidad\n\n";
}

static void pruebaExisteCarrera() {
    std::cout << "Prueba: existeCarrera\n";
    assert(existeCarrera("Ingenieria Informatica") == true);
    std::cout << "  Ingenieria Informatica existe.\n";
    assert(existeCarrera("Psicologia") == true);
    std::cout << "  Psicologia existe.\n";
    assert(existeCarrera("Astronomia") == false);
    std::cout << "  Astronomia NO existe.\n";
    std::cout << "OK: pruebaExisteCarrera\n\n";
}

static void pruebaObtenerFechaActual() {
    std::cout << "Prueba: obtenerFechaActual\n";
    std::string fecha = obtenerFechaActual();
    // formato AAAA-MM-DD => 10 chars
    assert(fecha.size() == 10);
    std::cout << "  Fecha obtenida: " << fecha << "\n";
    std::cout << "OK: pruebaObtenerFechaActual\n\n";
}

static void pruebaGenerarFichero() {
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
    std::cout << "  Fichero de prueba generado y verificado.\n\n";
}

void pruebasErrores() {
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