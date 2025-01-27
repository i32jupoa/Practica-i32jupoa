#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <string>
#include <vector>

// ------------------------------------------------------------------------
// Estructuras
// ------------------------------------------------------------------------
struct Usuario {
    std::string nombre;
    std::string apellidos;
    std::string dni;
    std::string correo;
    std::string estadoInscripcion;
    std::string fechaInscripcion;
    std::string tipoUsuario;  // 'Alumno' o 'Profesor' (heredado del código anterior)
};

struct Plan {
    std::string nombre;
    std::string universidadOrigen;
    std::string universidadDestino;
    std::string carrera;
    std::vector<std::string> asignaturas;
};

// ------------------------------------------------------------------------
// Declaración de funciones
// ------------------------------------------------------------------------

// Funciones auxiliares de strings
std::string trim(const std::string &s);
std::string toLower(const std::string &s);

// Comprobaciones en ficheros
bool existeEnFichero(const std::string &dato, const std::string &fichero);
bool perteneceAUniversidad(const std::string &universidad);
bool existeCarrera(const std::string &carrera);
bool existePlan(const std::string &nombrePlan);

// Obtener fecha actual
std::string obtenerFechaActual();

// Funciones de lógica de negocio
void inscribirUsuario();
void agregarPlan();
void consultarInscripcion();
void mostrarInscritos();
void mostrarPlanes();
void matricularEstudiantes();

// Pruebas
void pruebasErrores();

#endif // FUNCIONES_H