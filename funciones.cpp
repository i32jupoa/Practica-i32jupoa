#include "funciones.h"

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

// Función para convertir texto a minúsculas
std::string toLower(const std::string &s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Función genérica para buscar si un dato existe en un fichero
bool existeEnFichero(const std::string &dato, const std::string &fichero) {
    std::ifstream archivo(fichero);
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el fichero: " << fichero << "\n";
        return false;
    }

    std::string linea;
    while (getline(archivo, linea)) {
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
    while (getline(archivo, linea)) {
        // Buscar el nombre del plan antes de los dos puntos ":"
        size_t pos = linea.find(":");
        if (pos != std::string::npos) {
            std::string plan = linea.substr(0, pos);  // El nombre del plan está antes de ":"
            plan = trim(toLower(plan));  // Limpiar y convertir a minúsculas
            
            // Comparar el nombre del plan (en minúsculas y sin espacios extra) con el nombrePlan ingresado
            if (plan == toLower(trim(nombrePlan))) {
                return true;  // Si el nombre del plan coincide, existe
            }
        }
    }
    return false;  // Si no se encontró el plan
}

std::string obtenerFechaActual() {
    std::time_t t = std::time(nullptr);
    char fecha[20];
    std::strftime(fecha, sizeof(fecha), "%Y-%m-%d", std::localtime(&t));
    return std::string(fecha);
}

void inscribirUsuario() {
   Usuario nuevoUsuario;  // Asegúrate de tener la estructura de Usuario
    std::string dni, nombrePlan, universidad, carrera;
    char tipoUsuario; // Para almacenar 'A' para Alumno o 'P' para Profesor

    // 1. Ingresar el DNI y verificar si está en Usuarios.txt
    std::cout << "Ingrese el DNI del Usuario: ";
    std::getline(std::cin, dni);
    if (!existeEnFichero(dni, "Usuarios.txt")) {
        std::cout << "El Usuario no cumple los requisitos para cursar el programa SICUE.\n\n";
        return;
    }

    // 2. Elegir si es Usuario o Profesor
    std::cout << "Seleccione el tipo de usuario (A para Alumno, P para Profesor): ";
    std::cin >> tipoUsuario;
    std::cin.ignore();  // Limpiar el buffer del cin

    // Validar la opción seleccionada
    if (tipoUsuario != 'A' && tipoUsuario != 'P') {
        std::cout << "Opción no válida. Debe seleccionar 'A' para Alumno o 'P' para Profesor.\n\n";
        return;
    }

    // 3. Ingresar el nombre
    std::cout << "Ingrese el nombre: ";
    std::getline(std::cin, nuevoUsuario.nombre);

    // 4. Ingresar los apellidos
    std::cout << "Ingrese los apellidos: ";
    std::getline(std::cin, nuevoUsuario.apellidos);

    // 5. Ingresar el correo
    std::cout << "Ingrese el correo electrónico: ";
    std::getline(std::cin, nuevoUsuario.correo);

    // 6. Ingresar el nombre del plan y verificar que exista
    std::cout << "Ingrese el nombre del plan: ";
    std::getline(std::cin, nombrePlan);
    if (!existePlan(nombrePlan)) {
        std::cout << "El plan no existe.\n\n";
        return;
    }

    // 7. Ingresar cuatrimestre (1 o 2) y validar
    int cuatrimestre;
    std::cout << "Ingrese la duración del plan (1 para 1 cuatrimestre, 2 para 2 cuatrimestres): ";
    std::cin >> cuatrimestre;
    std::cin.ignore();  // Limpiar el buffer del cin
    if (cuatrimestre != 1 && cuatrimestre != 2) {
        std::cout << "Opción no válida. Debe seleccionar 1 o 2.\n\n";
        return;
    }

    // 8. Confirmar inscripción
    char confirmacion;
    std::cout << "¿Desea confirmar la inscripción? (S/N): ";
    std::cin >> confirmacion;
    std::cin.ignore();  // Limpiar el buffer del cin
    if (confirmacion == 'S' || confirmacion == 's') {
        // Guardar en inscritos.txt
        nuevoUsuario.dni = dni;  // Añadir el DNI a la estructura
        nuevoUsuario.estadoInscripcion = "Confirmado";
        nuevoUsuario.fechaInscripcion = obtenerFechaActual();

        // Guardar el tipo de usuario en la estructura (Alumno o Profesor)
        nuevoUsuario.tipoUsuario = (tipoUsuario == 'A') ? "Alumno" : "Profesor";

        // Escribir los datos en el archivo
        std::ofstream archivoInscritos("inscritos.txt", std::ios::app);
        if (!archivoInscritos) {
            std::cerr << "Error al abrir el fichero de inscritos.\n";
            return;
        }

        // Escribir la información en el archivo
        archivoInscritos << nuevoUsuario.dni << ", "  // DNI
                         << nuevoUsuario.nombre << " " << nuevoUsuario.apellidos << ", "  // Nombre y Apellidos
                         << nuevoUsuario.correo << ", "  // Correo electrónico
                         << nombrePlan << ", "  // Nombre del plan
                         << cuatrimestre << " cuatrimestre(s), "  // Duración del plan
                         << nuevoUsuario.tipoUsuario << "\n";  // Tipo de usuario (Alumno o Profesor)

        std::cout << "Inscripción confirmada correctamente.\n\n";
    } else {
        std::cout << "Inscripción cancelada.\n\n";
    }
}

void agregarPlan() {
    std::string nombrePlan, universidadOrigen, universidadDestino, carrera;

    // 1. Ingresar el nombre del plan
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

    // 4. Carrera asociada al plan
    std::cout << "Ingrese la carrera asociada al plan: ";
    std::getline(std::cin, carrera);
    if (!existeCarrera(carrera)) {
        std::cout << "La carrera no existe en el sistema.\n\n";
        return;
    }

    // 5. Guardar las asignaturas que se desean convalidar
    std::vector<std::string> asignaturas;
    std::string asignatura;
    char agregarOtra;

    std::cout << "Ingrese las asignaturas que desea convalidar (al introducir una, pulsar intro):\n";
    do {
        std::cout << "Asignatura: ";
        std::getline(std::cin, asignatura);
        asignaturas.push_back(asignatura);

        std::cout << "¿Desea agregar otra asignatura? (S/N): ";
        std::cin >> agregarOtra;
        std::cin.ignore(); // Limpiar el buffer
    } while (agregarOtra == 'S' || agregarOtra == 's');

    // 6. Guardar el plan y sus asignaturas en el fichero planes.txt
    std::ofstream archivo("planes.txt", std::ios::app);
    if (!archivo) {
        std::cerr << "Error al abrir el fichero de planes.\n";
        return;
    }
    archivo << nombrePlan << ": " << universidadOrigen << " --> " << universidadDestino
            << ", Carrera: " << carrera << "\n";
    archivo << "Asignaturas convalidadas:\n";
    for (const auto &asig : asignaturas) {
        archivo << "- " << asig << "\n";
    }
    archivo << "\n\n"; // Añadir una línea extra para separar los planes

    std::cout << "Plan de convalidación agregado correctamente.\n\n";
}

void consultarInscripcion() {
    std::string dni;
    std::cout << "Ingrese el DNI del Usuario: ";
    std::getline(std::cin, dni);

    std::ifstream archivo("inscritos.txt");
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo 'inscritos.txt'.\n";
        return;
    }

    std::string linea;
    bool encontrado = false;

    // Leemos cada línea del archivo
    while (getline(archivo, linea)) {
        size_t pos = linea.find(", ");  // Buscar la posición del primer delimitador (', ')
        if (pos != std::string::npos) {
            std::string dniUsuario = linea.substr(0, pos);  // Extraer el DNI al inicio de la línea
            
            if (dniUsuario == dni) {  // Comparación estricta
                encontrado = true;

                // Extraer los datos restantes
                std::string nombre, correo, plan, cuatrimestres, fechaInscripcion, estado;

                // Obtener Nombre completo
                linea = linea.substr(pos + 2);
                pos = linea.find(", ");
                nombre = linea.substr(0, pos);
                linea = linea.substr(pos + 2);

                // Obtener Correo
                pos = linea.find(", ");
                correo = linea.substr(0, pos);
                linea = linea.substr(pos + 2);

                // Obtener Plan
                pos = linea.find(", ");
                plan = linea.substr(0, pos);
                linea = linea.substr(pos + 2);

                // Obtener Cuatrimestres
                pos = linea.find(", ");
                cuatrimestres = linea.substr(0, pos);
                linea = linea.substr(pos + 2);

                // Obtener Fecha de inscripción
                fechaInscripcion = obtenerFechaActual(); // Se podría leer del archivo si estuviese presente.

                // Mostrar la información extraída
                std::cout << "Inscripción de: " << nombre << ", con DNI: " << dniUsuario << "\n";
                std::cout << "Correo: " << correo << "\n";
                std::cout << "Plan seleccionado: " << plan << "\n";
                std::cout << "Duración: " << cuatrimestres << "\n";
                std::cout << "Fecha de inscripción: " << fechaInscripcion << "\n";
                std::cout << "Estado de la inscripción: Confirmado\n\n";  // Estado fijo como ejemplo

                break;  // Salimos del bucle al encontrar el DNI
            }
        }
    }

    if (!encontrado) {
        std::cout << "Usuario con DNI " << dni << " no encontrado. Aún no está inscrito.\n\n";
    }
    archivo.close();
}

void mostrarInscritos() {
    std::ifstream archivo("inscritos.txt");
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el fichero de inscritos.\n";
        return;
    }

    std::string linea;
    bool encontrado = false;

    std::cout << "--- Lista de inscritos ---\n";
    while (getline(archivo, linea)) {
        if (!linea.empty()) {
            std::cout << linea << "\n";
            encontrado = true;
        }
    }

    if (!encontrado) {
        std::cout << "Aún no hay inscritos registrados.\n";
    }
}


void mostrarPlanes() {
    std::ifstream archivo("planes.txt");
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el fichero de planes.\n";
        return;
    }

    std::string linea;
    bool encontrado = false;

    std::cout << "--- Consultando planes ---\n";
    while (getline(archivo, linea)) {
        if (!linea.empty()) {
            std::cout << linea << "\n";
            encontrado = true;
        } else {
            // Si hay una línea vacía en el archivo, imprime una línea adicional
            std::cout << "\n";
        }
    }

    if (!encontrado) {
        std::cout << "Aún no hay planes registrados.\n";
    }

    std::cout << "\n";
    archivo.close();
}


void pruebaPerteneceAUniversidad() {
    std::cout << "Prueba: perteneceAUniversidad\n";
    assert(perteneceAUniversidad("Universidad de Cordoba") == true);
    std::cout << "La Universidad de Cordoba se encuentra en el fichero universidades.txt\n";
    assert(perteneceAUniversidad("Universidad de Zaragoza") == true);
    std::cout << "La Universidad de Zaragoza se encuentra en el fichero universidades.txt\n";
    assert(perteneceAUniversidad("Universidad de Saturno") == false);
    std::cout << "La Universidad de Saturno NO se encuentra en el fichero universidades.txt\n";
    std::cout << "Prueba perteneceAUniversidad completada con exito.\n\n";
}

void pruebaExisteCarrera() {
    std::cout << "Prueba: existeCarrera\n";
    assert(existeCarrera("Ingenieria Informatica") == true);
    std::cout << "Ingenieria Informatica existe en carrera.txt\n";
    assert(existeCarrera("Psicologia") == true);
    std::cout << "Psicologia existe en carrera.txt\n";
    assert(existeCarrera("Astronomia") == false);
    std::cout << "Astronomia NO existe en carrera.txt\n";
    std::cout << "Prueba existeCarrera completada con exito.\n\n";
}


void pruebaObtenerFechaActual() {
    std::cout << "Prueba: obtenerFechaActual\n";
    std::string fecha = obtenerFechaActual();
    assert(fecha.size() == 10); // Verifica el formato AAAA-MM-DD
    std::cout << "Fecha obtenida: " << fecha << "\n";
    std::cout << "Prueba obtenerFechaActual completada con exito.\n\n";
}

void pruebaGenerarFichero() {
    std::cout << "Prueba: Generar un fichero\n";

    // Crear el fichero de prueba
    std::string nombreFichero = "fichero_prueba.txt";
    std::ofstream archivo(nombreFichero);

    // Escribir datos de prueba en el archivo
    archivo << "DNI: 12345678A\n";
    archivo << "Nombre: Fernando\n";
    archivo << "Correo: fernandoalonso@uco.es\n";

    // Cerrar el archivo
    archivo.close();

    // Verificar si el fichero fue creado y contiene los datos esperados
    std::ifstream archivoVerificacion(nombreFichero);
    assert(archivoVerificacion.is_open() && "El fichero no se pudo abrir.");

    std::string linea;
    bool encontrado = false;
    while (getline(archivoVerificacion, linea)) {
        if (linea == "DNI: 12345678A") {
            encontrado = true;
            break;
        }
    }

     assert(encontrado && "El contenido esperado no se encontró en el archivo.");

    std::cout << "Fichero de prueba generado correctamente y verificado.\n\n";
}

void pruebasErrores(){
    std::cout << "\n--- Ejecutando pruebas..\n";

    //Prueba 1
    pruebaPerteneceAUniversidad();

    //Prueba 2
    pruebaExisteCarrera();

    //Prueba 3:
    std::cout << "Intentar agregar un plan con universidad de Destino inexistente.\n";
    agregarPlan();

    //Prueba 4:
    std::cout << "Intentar hacer un plan con carrera invalida.\n";
    agregarPlan();

    //Prueba 5:
    std::cout << "Consultar un Usuario con DNI inexistente.\n";
    consultarInscripcion();

    //Prueba 6:
    std::cout <<"Intentar hacer una inscripcion de mas de 2 cuatrimestres\n";
    inscribirUsuario();

    //Prueba 7:
    std::cout << "Cancelar la inscripcion antes de confirmarla\n";
    inscribirUsuario();

    //Prueba 8:
    pruebaObtenerFechaActual();

    //Prueba 9:
    pruebaGenerarFichero();
    std::cout << "--- Todas las pruebas completadas con exito ---\n";
}
