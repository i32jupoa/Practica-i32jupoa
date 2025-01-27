#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QAction>
#include <QInputDialog>
#include <QMessageBox>
#include <fstream>
#include <sstream>

#include "funciones.h"

class MainWindow : public QMainWindow {
public:
    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        // --- Configurar la ventana ---
        setWindowTitle("Sistema Gestión SICUE (Qt)");
        resize(900, 600);

        // --- Menú superior ---
        QMenu* fileMenu = menuBar()->addMenu("Archivo");
        QAction* exitAction = new QAction("Salir", this);
        fileMenu->addAction(exitAction);
        connect(exitAction, &QAction::triggered, this, &MainWindow::close);

        // --- Barra de estado ---
        statusBar()->showMessage("Listo");

        // --- Widget central ---
        QWidget* central = new QWidget(this);
        setCentralWidget(central);

        // Layout principal (vertical)
        QVBoxLayout* mainLayout = new QVBoxLayout(central);

        // 1) TÍTULO PRINCIPAL
        QLabel* title = new QLabel("Sistema Gestión SICUE (Qt)");
        title->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        title->setStyleSheet(R"(
            font-size: 28px;
            font-weight: bold;
            color: #2c3e50;
        )");
        mainLayout->addWidget(title);

        // 2) PEQUEÑA INTRO
        QLabel* introLabel = new QLabel(
            "Bienvenido/a al Sistema de Gestión SICUE.\n\n"
            "Este programa permite:\n"
            " - Inscribir usuarios en el sistema.\n"
            " - Agregar planes de convalidación.\n"
            " - Consultar inscripciones, mostrar planes y más.\n\n"
            "Utiliza los botones inferiores para navegar por las distintas funcionalidades."
        );
        introLabel->setWordWrap(true);
        introLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
        introLabel->setStyleSheet(R"(
            font-size: 16px;
            color: #2c3e50;
            margin: 20px;
        )");
        mainLayout->addWidget(introLabel, /*stretch=*/1);

        // 3) BARRA DE BOTONES
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        mainLayout->addLayout(buttonLayout);

        QPushButton* inscribirBtn        = new QPushButton("Inscribir Usuario");
        QPushButton* agregarPlanBtn      = new QPushButton("Agregar Plan");
        QPushButton* consultarBtn        = new QPushButton("Consultar Inscripción");
        QPushButton* mostrarInscritosBtn = new QPushButton("Mostrar Inscritos");
        QPushButton* mostrarPlanesBtn    = new QPushButton("Mostrar Planes");
        QPushButton* salirBtn            = new QPushButton("Salir");

        buttonLayout->addWidget(inscribirBtn);
        buttonLayout->addWidget(agregarPlanBtn);
        buttonLayout->addWidget(consultarBtn);
        buttonLayout->addWidget(mostrarInscritosBtn);
        buttonLayout->addWidget(mostrarPlanesBtn);
        buttonLayout->addWidget(salirBtn);

        // Conexiones (lambdas)
        connect(inscribirBtn,        &QPushButton::clicked, this, [this]() { inscribirUsuarioGUI(); });
        connect(agregarPlanBtn,      &QPushButton::clicked, this, [this]() { agregarPlanGUI(); });
        connect(consultarBtn,        &QPushButton::clicked, this, [this]() { consultarInscripcionGUI(); });
        connect(mostrarInscritosBtn, &QPushButton::clicked, this, [this]() { mostrarInscritosGUI(); });
        connect(mostrarPlanesBtn,    &QPushButton::clicked, this, [this]() { mostrarPlanesGUI(); });
        connect(salirBtn,            &QPushButton::clicked, this, &MainWindow::close);

        // Estilos de los botones
        setStyleSheet(R"(
            QMainWindow {
                background-color: #f5f5f5;
            }
            QPushButton {
                background-color: #2980b9;
                color: white;
                border-radius: 8px;
                font-size: 14px;
                font-weight: bold;
                min-width: 160px;
                min-height: 50px;
            }
            QPushButton:hover {
                background-color: #3498db;
            }
            QPushButton:pressed {
                background-color: #1c5fa8;
            }
        )");
    }

private:
    // ----------------------------------------------------------------------
    // Funciones GUI auxiliares para replicar la lógica de funciones.cpp
    // ----------------------------------------------------------------------

    void inscribirUsuarioGUI() {
        bool ok;

        // 1) DNI
        QString dni = QInputDialog::getText(this, "Inscribir Usuario",
                                            "Ingrese DNI:", QLineEdit::Normal,
                                            "", &ok);
        if (!ok || dni.isEmpty()) return;

        // Verificar en Usuarios.txt
        if (!existeEnFichero(dni.toStdString(), "Usuarios.txt")) {
            QMessageBox::warning(this, "Error",
                                 "El Usuario no cumple requisitos (DNI no encontrado en Usuarios.txt).");
            return;
        }

        // 2) Tipo de usuario
        QStringList tipos; 
        tipos << "Alumno" << "Profesor";
        QString tipoUsuario = QInputDialog::getItem(this, "Tipo de Usuario",
                                                   "Seleccione:", tipos, 0, false, &ok);
        if (!ok || tipoUsuario.isEmpty()) return;

        // 3) Nombre
        QString nombre = QInputDialog::getText(this, "Nombre",
                                              "Ingrese el nombre:", QLineEdit::Normal,
                                              "", &ok);
        if (!ok) return;

        // 4) Apellidos
        QString apellidos = QInputDialog::getText(this, "Apellidos",
                                                 "Ingrese los apellidos:", QLineEdit::Normal,
                                                 "", &ok);
        if (!ok) return;

        // 5) Correo
        QString correo = QInputDialog::getText(this, "Correo",
                                              "Ingrese el correo electrónico:", QLineEdit::Normal,
                                              "", &ok);
        if (!ok) return;

        // 6) Plan
        QString plan = QInputDialog::getText(this, "Plan",
                                            "Ingrese el nombre del plan de convalidación:", QLineEdit::Normal,
                                            "", &ok);
        if (!ok || plan.isEmpty()) return;

        if (!existePlan(plan.toStdString())) {
            QMessageBox::warning(this, "Error", "El plan no existe en planes.txt.");
            return;
        }

        // 7) Cuatrimestre (1 o 2)
        int cuatrimestre = QInputDialog::getInt(this, "Duración",
                                               "Ingrese cuatrimestres (1 ó 2):", 1, 1, 2, 1, &ok);
        if (!ok) return;

        // 8) Confirmar
        auto resp = QMessageBox::question(
            this, "Confirmar inscripción",
            QString("¿Confirmar inscripción?\n\nDNI: %1\nTipo: %2\nPlan: %3\nCuatrimestre(s): %4")
                .arg(dni).arg(tipoUsuario).arg(plan).arg(cuatrimestre)
        );
        if (resp != QMessageBox::Yes) {
            QMessageBox::information(this, "Info", "Inscripción cancelada.");
            return;
        }

        // Guardar en preinscritos.txt (estado "en proceso")
        std::ofstream pre("preinscritos.txt", std::ios::app);
        if (!pre.is_open()) {
            QMessageBox::critical(this, "Error", "No se pudo abrir preinscritos.txt");
            return;
        }
        // Obtenemos la fecha
        std::string fecha = obtenerFechaActual();
        // Convertimos a std::string
        std::string tipo = (tipoUsuario == "Alumno") ? "Alumno" : "Profesor";
        pre << dni.toStdString() << ", "
            << nombre.toStdString() << " " << apellidos.toStdString() << ", "
            << correo.toStdString() << ", "
            << plan.toStdString() << ", "
            << cuatrimestre << " cuatrimestre(s), "
            << "en proceso, "
            << fecha << ", "
            << tipo
            << "\n";
        pre.close();

        QMessageBox::information(this, "Éxito",
                                 "Se ha inscrito al usuario en preinscritos.txt (en proceso).");
    }

    void agregarPlanGUI() {
        bool ok;
        // 1) Nombre del plan
        QString nombrePlan = QInputDialog::getText(this, "Agregar Plan",
                                                  "Nombre del plan:", QLineEdit::Normal,
                                                  "", &ok);
        if (!ok || nombrePlan.isEmpty()) return;

        // 2) Univ. origen (solo Córdoba)
        QString origen = QInputDialog::getText(this, "Universidad de Origen",
                                              "Ingrese la U. de origen (Universidad de Cordoba):",
                                              QLineEdit::Normal, "", &ok);
        if (!ok) return;
        if (!perteneceAUniversidad(origen.toStdString())) {
            QMessageBox::warning(this, "Error", "La U. de origen no está en SICUE.");
            return;
        }
        if (toLower(origen.toStdString()) != "universidad de cordoba"
            && toLower(origen.toStdString()) != "universidad de córdoba") {
            QMessageBox::warning(this, "Error", "La U. de origen debe ser la de Córdoba.");
            return;
        }

        // 3) Univ. destino (≠ Córdoba)
        QString destino = QInputDialog::getText(this, "Universidad de Destino",
                                               "Ingrese la U. de destino (no puede ser Córdoba):",
                                               QLineEdit::Normal, "", &ok);
        if (!ok) return;
        if (!perteneceAUniversidad(destino.toStdString())) {
            QMessageBox::warning(this, "Error", "La U. de destino no está en SICUE.");
            return;
        }
        if (toLower(destino.toStdString()) == "universidad de cordoba"
            || toLower(destino.toStdString()) == "universidad de córdoba") {
            QMessageBox::warning(this, "Error", "La U. de destino no puede ser la de Córdoba.");
            return;
        }

        // 4) Carrera
        QString carrera = QInputDialog::getText(this, "Carrera",
                                               "Ingrese la carrera asociada:", QLineEdit::Normal,
                                               "", &ok);
        if (!ok) return;
        if (!existeCarrera(carrera.toStdString())) {
            QMessageBox::warning(this, "Error", "La carrera no existe en carrera.txt.");
            return;
        }

        // 5) Asignaturas
        std::vector<std::string> asignaturas;
        while (true) {
            QString asig = QInputDialog::getText(this, "Asignaturas",
                                                "Ingrese una asignatura (o vacío para terminar):",
                                                 QLineEdit::Normal, "", &ok);
            if (!ok || asig.isEmpty()) {
                break;
            }
            asignaturas.push_back(asig.toStdString());
        }
        if (asignaturas.empty()) {
            QMessageBox::warning(this, "Error", "No se introdujo ninguna asignatura.");
            return;
        }

        // 6) Confirmar
        auto resp = QMessageBox::question(
            this, "Confirmar Plan",
            QString("¿Crear el plan '%1'?\nOrigen: %2\nDestino: %3\nCarrera: %4")
                .arg(nombrePlan).arg(origen).arg(destino).arg(carrera)
        );
        if (resp != QMessageBox::Yes) {
            QMessageBox::information(this, "Cancelado", "Se canceló la creación del plan.");
            return;
        }

        // Guardar en planes.txt
        std::ofstream out("planes.txt", std::ios::app);
        if (!out) {
            QMessageBox::critical(this, "Error", "No se pudo abrir planes.txt");
            return;
        }
        out << nombrePlan.toStdString() << ": "
            << origen.toStdString() << " --> " << destino.toStdString()
            << ", Carrera: " << carrera.toStdString() << "\n";
        out << "Asignaturas convalidadas:\n";
        for (auto &a : asignaturas) {
            out << "- " << a << "\n";
        }
        out << "\n\n";

        QMessageBox::information(this, "Éxito", "Plan agregado correctamente.");
    }

    void consultarInscripcionGUI() {
        bool ok;
        QString dni = QInputDialog::getText(this, "Consultar Inscripción",
                                            "Ingrese DNI:", QLineEdit::Normal,
                                            "", &ok);
        if (!ok || dni.isEmpty()) return;

        // Simulamos la lógica de consultarInscripcion() (versión resumida)
        // 1) Buscar en preinscritos
        {
            std::ifstream pre("preinscritos.txt");
            if (pre.is_open()) {
                std::string linea;
                while (std::getline(pre, linea)) {
                    if (linea.empty()) continue;
                    size_t pos = linea.find(", ");
                    if (pos != std::string::npos) {
                        std::string dniUsuario = trim(linea.substr(0, pos));
                        if (dniUsuario == dni.toStdString()) {
                            QMessageBox::information(this, "Resultado",
                                QString("El usuario %1 está en preinscritos (en proceso).").arg(dni));
                            return;
                        }
                    }
                }
            }
        }

        // 2) Buscar en inscritos
        {
            std::ifstream ins("inscritos.txt");
            if (ins.is_open()) {
                std::string linea;
                while (std::getline(ins, linea)) {
                    if (linea.empty()) continue;
                    size_t pos = linea.find(", ");
                    if (pos != std::string::npos) {
                        std::string dniUsuario = trim(linea.substr(0, pos));
                        if (dniUsuario == dni.toStdString()) {
                            QMessageBox::information(this, "Resultado",
                                QString("El usuario %1 está en inscritos (confirmada).").arg(dni));
                            return;
                        }
                    }
                }
            }
        }

        // 3) No encontrado
        QMessageBox::information(this, "No encontrado",
                                 QString("El usuario %1 no está en preinscritos ni en inscritos.").arg(dni));
    }

    void mostrarInscritosGUI() {
        std::ifstream ins("inscritos.txt");
        if (!ins.is_open()) {
            QMessageBox::critical(this, "Error", "No se pudo abrir inscritos.txt");
            return;
        }
        std::string linea;
        QString acumulado;
        bool hayDatos = false;
        while (std::getline(ins, linea)) {
            if (!linea.empty()) {
                acumulado += QString::fromStdString(linea) + "\n";
                hayDatos = true;
            }
        }
        if (!hayDatos) {
            acumulado = "No hay inscritos aún.\n";
        }
        QMessageBox::information(this, "Lista de Inscritos", acumulado);
    }

    void mostrarPlanesGUI() {
        std::ifstream planf("planes.txt");
        if (!planf.is_open()) {
            QMessageBox::critical(this, "Error", "No se pudo abrir planes.txt");
            return;
        }
        std::string linea;
        QString acumulado;
        bool hayDatos = false;
        while (std::getline(planf, linea)) {
            acumulado += QString::fromStdString(linea) + "\n";
            hayDatos = true;
        }
        if (!hayDatos) {
            acumulado = "No hay planes registrados.\n";
        }
        QMessageBox::information(this, "Listado de Planes", acumulado);
    }
};

// ------------------------------------------------------------------------
// MAIN (Qt)
// ------------------------------------------------------------------------
#include <QObject>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    MainWindow w;
    w.show();

    return app.exec();
}