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
        resize(900, 600);  // Un poco más grande

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

        // 2) PEQUEÑA INTRODUCCIÓN
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

        // 3) BARRA DE BOTONES (abajo)
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        mainLayout->addLayout(buttonLayout);

        QPushButton* inscribirBtn        = new QPushButton("Inscribir Usuario");
        QPushButton* agregarPlanBtn      = new QPushButton("Agregar Plan");
        QPushButton* consultarBtn        = new QPushButton("Consultar Inscripción");
        QPushButton* mostrarInscritosBtn = new QPushButton("Mostrar Inscritos");
        QPushButton* mostrarPlanesBtn    = new QPushButton("Mostrar Planes");
        QPushButton* salirBtn            = new QPushButton("Salir");

        // Añadir botones a la fila
        buttonLayout->addWidget(inscribirBtn);
        buttonLayout->addWidget(agregarPlanBtn);
        buttonLayout->addWidget(consultarBtn);
        buttonLayout->addWidget(mostrarInscritosBtn);
        buttonLayout->addWidget(mostrarPlanesBtn);
        buttonLayout->addWidget(salirBtn);

        // 4) Conexiones (lambdas)
        connect(inscribirBtn, &QPushButton::clicked, this, [this]() {
            inscribirUsuarioGUI();
        });
        connect(agregarPlanBtn, &QPushButton::clicked, this, [this]() {
            agregarPlanGUI();
        });
        connect(consultarBtn, &QPushButton::clicked, this, [this]() {
            consultarInscripcionGUI();
        });
        connect(mostrarInscritosBtn, &QPushButton::clicked, this, [this]() {
            mostrarInscritosGUI();
        });
        connect(mostrarPlanesBtn, &QPushButton::clicked, this, [this]() {
            mostrarPlanesGUI();
        });
        connect(salirBtn, &QPushButton::clicked, this, &MainWindow::close);

        // 5) Estilos
        // Podemos aplicar un stylesheet global a la ventana principal
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
    // -----------
    // Funciones 
    // -----------

    void inscribirUsuarioGUI() {
        // Simula la lógica de inscribirUsuario() de consola, pidiendo datos con diálogos
        bool ok;

        // 1) Pedir DNI
        QString dni = QInputDialog::getText(this, "Inscribir Usuario",
                                            "Ingrese DNI:", QLineEdit::Normal,
                                            QString(), &ok);
        if (!ok || dni.isEmpty()) return;

        if (!existeEnFichero(dni.toStdString(), "Usuarios.txt")) {
            QMessageBox::warning(this, "Error", "El Usuario no cumple los requisitos para SICUE.");
            return;
        }

        // 2) Tipo de usuario
        QStringList tipos; 
        tipos << "Alumno" << "Profesor";
        QString tipoUsuario = QInputDialog::getItem(this, "Tipo de Usuario",
                                                   "Seleccione:", tipos, 0, false, &ok);
        if (!ok) return;

        // 3) Nombre
        QString nombre = QInputDialog::getText(this, "Nombre",
                                              "Ingrese el nombre:", QLineEdit::Normal,
                                              QString(), &ok);
        if (!ok) return;

        // 4) Apellidos
        QString apellidos = QInputDialog::getText(this, "Apellidos",
                                                 "Ingrese los apellidos:", QLineEdit::Normal,
                                                 QString(), &ok);
        if (!ok) return;

        // 5) Correo
        QString correo = QInputDialog::getText(this, "Correo",
                                              "Ingrese el correo:", QLineEdit::Normal,
                                              QString(), &ok);
        if (!ok) return;

        // 6) Plan
        QString plan = QInputDialog::getText(this, "Plan",
                                            "Ingrese el nombre del plan:", QLineEdit::Normal,
                                            QString(), &ok);
        if (!ok || plan.isEmpty()) return;

        if (!existePlan(plan.toStdString())) {
            QMessageBox::warning(this, "Error", "El plan no existe.");
            return;
        }

        // 7) Duración (1 o 2)
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

        // Guardar en "inscritos.txt"
        std::ofstream out("inscritos.txt", std::ios::app);
        if (!out) {
            QMessageBox::critical(this, "Error", "No se pudo abrir inscritos.txt");
            return;
        }

        std::string tipo = (tipoUsuario == "Alumno") ? "Alumno" : "Profesor";
        out << dni.toStdString() << ", "
            << nombre.toStdString() << " " << apellidos.toStdString() << ", "
            << correo.toStdString() << ", "
            << plan.toStdString() << ", "
            << cuatrimestre << " cuatrimestre(s), "
            << tipo << "\n";
        QMessageBox::information(this, "Éxito", "Inscripción confirmada.");
    }

    void agregarPlanGUI() {
        bool ok;
        // 1) Nombre del plan
        QString nombrePlan = QInputDialog::getText(this, "Agregar Plan",
                                                  "Nombre del plan:", QLineEdit::Normal,
                                                  QString(), &ok);
        if (!ok || nombrePlan.isEmpty()) return;

        // 2) Universidad de origen
        QString origen = QInputDialog::getText(this, "Universidad de Origen",
                                              "Ingrese la U. de origen:", QLineEdit::Normal,
                                              QString(), &ok);
        if (!ok) return;

        if (!perteneceAUniversidad(origen.toStdString())) {
            QMessageBox::warning(this, "Error", "Universidad de origen no está en SICUE.");
            return;
        }

        // 3) Universidad de destino
        QString destino = QInputDialog::getText(this, "Universidad de Destino",
                                               "Ingrese la U. de destino:", QLineEdit::Normal,
                                               QString(), &ok);
        if (!ok) return;

        if (!perteneceAUniversidad(destino.toStdString())) {
            QMessageBox::warning(this, "Error", "Universidad de destino no está en SICUE.");
            return;
        }

        // 4) Carrera
        QString carrera = QInputDialog::getText(this, "Carrera",
                                               "Ingrese la carrera:", QLineEdit::Normal,
                                               QString(), &ok);
        if (!ok) return;

        if (!existeCarrera(carrera.toStdString())) {
            QMessageBox::warning(this, "Error", "La carrera no existe.");
            return;
        }

        // 5) Asignaturas (bucle con getText)
        std::vector<std::string> asignaturas;
        while (true) {
            QString asig = QInputDialog::getText(this, "Asignaturas",
                                                "Ingrese una asignatura (deje vacío p/terminar):",
                                                 QLineEdit::Normal, "", &ok);
            if (!ok || asig.isEmpty()) {
                break; 
            }
            asignaturas.push_back(asig.toStdString());
        }

        // Guardamos en planes.txt
        std::ofstream out("planes.txt", std::ios::app);
        if (!out) {
            QMessageBox::critical(this, "Error", "No se pudo abrir planes.txt");
            return;
        }

        out << nombrePlan.toStdString() << ": "
            << origen.toStdString() << " --> " << destino.toStdString()
            << ", Carrera: " << carrera.toStdString() << "\n"
            << "Asignaturas convalidadas:\n";
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

        std::ifstream in("inscritos.txt");
        if (!in) {
            QMessageBox::critical(this, "Error", "No se pudo abrir inscritos.txt");
            return;
        }
        std::string linea;
        bool encontrado = false;
        while (std::getline(in, linea)) {
            if (linea.empty()) continue;
            size_t pos = linea.find(", ");
            if (pos != std::string::npos) {
                std::string dniUser = linea.substr(0, pos);
                if (dniUser == dni.toStdString()) {
                    encontrado = true;
                    QMessageBox::information(
                        this, "Datos de Inscripción",
                        QString::fromStdString(linea)
                    );
                    break;
                }
            }
        }
        if (!encontrado) {
            QMessageBox::information(this, "No encontrado",
                                     "DNI no encontrado en inscritos.txt");
        }
    }

    void mostrarInscritosGUI() {
        std::ifstream in("inscritos.txt");
        if (!in) {
            QMessageBox::critical(this, "Error", "No se pudo abrir inscritos.txt");
            return;
        }
        std::string linea;
        QString acumulado;
        bool hayDatos = false;
        while (std::getline(in, linea)) {
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
        std::ifstream in("planes.txt");
        if (!in) {
            QMessageBox::critical(this, "Error", "No se pudo abrir planes.txt");
            return;
        }
        std::string linea;
        QString acumulado;
        bool hayDatos = false;
        while (std::getline(in, linea)) {
            acumulado += QString::fromStdString(linea) + "\n";
            hayDatos = true;
        }
        if (!hayDatos) {
            acumulado = "No hay planes registrados.\n";
        }
        QMessageBox::information(this, "Listado de Planes", acumulado);
    }
};

// Para no requerir main_gui.moc, no usamos Q_OBJECT en un .h aparte. 
// Este ejemplo funciona tal cual.
#include <QObject> 

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    MainWindow w;
    w.show();

    return app.exec();
}
