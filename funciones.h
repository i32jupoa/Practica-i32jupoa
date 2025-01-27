#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <cassert>

// Estructura de Usuario
struct Usuario {
    std::string nombre;
    std::string apellidos;
    std::string dni;
    std::string correo;
    std::string estadoInscripcion;
    std::string fechaInscripcion;
    std::string tipoUsuario;
};

// Estructura de Plan (opcional)
struct Plan {
    std::string nombre;
    std::string universidad;
    std::vector<std::string> asignaturas;
};

// Cabeceras de funciones
std::string trim(const std::string &s);
std::string toLower(const std::string &s);

bool existeEnFichero(const std::string &dato, const std::string &fichero);
bool perteneceAUniversidad(const std::string &universidad);
bool existeCarrera(const std::string &carrera);
bool existePlan(const std::string &nombrePlan);

std::string obtenerFechaActual();

void inscribirUsuario();
void agregarPlan();
void consultarInscripcion();
void mostrarInscritos();
void mostrarPlanes();

// Pruebas
void pruebasErrores();
void pruebaPerteneceAUniversidad();
void pruebaExisteCarrera();
void pruebaObtenerFechaActual();
void pruebaGenerarFichero();

#endif
