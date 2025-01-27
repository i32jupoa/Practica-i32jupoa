#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <cassert>
#include <sstream>
#include <cctype>
#include <algorithm>

struct Usuario {
    std::string nombre;
    std::string apellidos;
    std::string dni;
    std::string correo;
    std::string estadoInscripcion;
    std::string fechaInscripcion;
    std::string tipoUsuario;
};

struct Plan {
    std::string nombre;
    std::string universidad;
    std::vector<std::string> asignaturas;
};

bool existeEnFichero(const std::string &dato, const std::string &fichero);
bool perteneceAUniversidad(const std::string &universidad);
bool existeCarrera(const std::string &carrera);
bool existePlan(const std::string &nombrePlan);
void inscribirUsuario();
void agregarPlan();
void consultarInscripcion();
void mostrarInscritos();
void pruebasErrores();
void mostrarPlanes(); 
std::string obtenerFechaActual();
void pruebaPerteneceAUniversidad();
void pruebaExisteCarrera();
void pruebaObtenerFechaActual();
void pruebaGenerarFichero();

#endif
