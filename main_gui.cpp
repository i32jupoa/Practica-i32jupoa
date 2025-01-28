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
#include <QDialog>
#include <QScrollArea>
#include <QGroupBox>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

#include "funciones.h"

// Función auxiliar para obtener la universidad de destino de un plan
// (reutilizamos la misma lógica que en el .cpp anterior)
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
            " - Inscribir usuarios.\n"
            " - Agregar planes de convalidación.\n"
            " - Consultar inscripciones, mostrar planes...\n"
            " - Matricular estudiantes.\n\n"
            "Utiliza los botones inferiores para navegar por las distintas funcionalidades."
        );
        introLabel->setWordWrap(true);
        introLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
        introLabel->setStyleSheet(R"(
            font-size: 16px;
            color: #2c3e50;
            margin: 20px;
        )");
        mainLayout->addWidget(introLabel, 1);

        // 3) BARRA DE BOTONES
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        mainLayout->addLayout(buttonLayout);

        QPushButton* inscribirBtn        = new QPushButton("Inscribir Usuario");
        QPushButton* agregarPlanBtn      = new QPushButton("Agregar Plan");
        QPushButton* consultarBtn        = new QPushButton("Consultar Inscripción");
        QPushButton* mostrarInscritosBtn = new QPushButton("Mostrar Inscritos");
        QPushButton* mostrarPlanesBtn    = new QPushButton("Mostrar Planes");
        QPushButton* matricularBtn       = new QPushButton("Matricular Estudiantes");
        QPushButton* salirBtn            = new QPushButton("Salir");

        buttonLayout->addWidget(inscribirBtn);
        buttonLayout->addWidget(agregarPlanBtn);
        buttonLayout->addWidget(consultarBtn);
        buttonLayout->addWidget(mostrarInscritosBtn);
        buttonLayout->addWidget(mostrarPlanesBtn);
        buttonLayout->addWidget(matricularBtn);
        buttonLayout->addWidget(salirBtn);

        // Conexiones (lambdas)
        connect(inscribirBtn,        &QPushButton::clicked, this, [this]() { inscribirUsuarioGUI(); });
        connect(agregarPlanBtn,      &QPushButton::clicked, this, [this]() { agregarPlanGUI(); });
        connect(consultarBtn,        &QPushButton::clicked, this, [this]() { consultarInscripcionGUI(); });
        connect(mostrarInscritosBtn, &QPushButton::clicked, this, [this]() { mostrarInscritosGUI(); });
        connect(mostrarPlanesBtn,    &QPushButton::clicked, this, [this]() { mostrarPlanesGUI(); });
        connect(matricularBtn,       &QPushButton::clicked, this, [this]() { matricularEstudiantesGUI(); });
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
                min-width: 140px;
                min-height: 45px;
            }
            QPushButton:hover {
                background-color: #3498db;
            }
            QPushButton:pressed {
                background-color: #1c5fa8;
            }
        )");
    }

private slots:
    // ----------------------------------------------------------------------
    // FUNCIONES GUI (similares a las de funciones.cpp)
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

        // 2) Tipo de usuario (Alumno/Profesor)
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
        std::string fecha = obtenerFechaActual();
        std::string tipo = (tipoUsuario == "Alumno") ? "Alumno" : "Profesor";
        pre << dni.toStdString() << ", "
            << nombre.toStdString() << " " << apellidos.toStdString() << ", "
            << correo.toStdString() << ", "
            << plan.toStdString() << ", "
            << cuatrimestre << " cuatrimestre(s), "
            << "en proceso, "
            << fecha << ", "
            << tipo << "\n";
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
                                               "Ingrese la carrera asociada al plan:",
                                               QLineEdit::Normal, "", &ok);
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

        // Buscar en preinscritos
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

        // Buscar en inscritos
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

        // No encontrado
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

    // ----------------------------------------------------------------------
    // NUEVA FUNCIÓN: MATRICULAR ESTUDIANTES CON BOTÓN POR CANDIDATO
    // ----------------------------------------------------------------------
    void matricularEstudiantesGUI() {
        bool ok;
        QString univDestino = QInputDialog::getText(
            this, "Matricular Estudiantes",
            "Ingrese la universidad de destino para matricular:",
            QLineEdit::Normal, "", &ok
        );
        if (!ok || univDestino.isEmpty()) return;

        // Leemos preinscritos y guardamos todas las líneas
        std::ifstream preFile("preinscritos.txt");
        if (!preFile.is_open()) {
            QMessageBox::critical(this, "Error", "No se pudo abrir preinscritos.txt");
            return;
        }

        std::vector<std::string> todasLineas;
        std::vector<int> indicesCandidatos;  // índices en todasLineas que coinciden
        std::string linea;
        int lineIndex = 0;

        while (std::getline(preFile, linea)) {
            todasLineas.push_back(linea);
            if (!linea.empty()) {
                // Separamos la línea para obtener el plan (4º campo)
                // Formato: DNI, Nombre completo, Correo, Plan, Cuatrimestre, Estado, Fecha, Tipo
                std::stringstream ss(linea);
                std::vector<std::string> partes;
                std::string tmp;
                while (std::getline(ss, tmp, ',')) {
                    partes.push_back(trim(tmp));
                }
                if (partes.size() >= 4) {
                    std::string planUsuario = partes[3]; // el 4º campo
                    // Obtenemos la univ. destino real del plan
                    std::string univDelPlan = obtenerUniversidadDestinoDelPlan(planUsuario);
                    if (toLower(trim(univDelPlan)) == toLower(trim(univDestino.toStdString()))) {
                        indicesCandidatos.push_back(lineIndex);
                    }
                }
            }
            lineIndex++;
        }
        preFile.close();

        if (indicesCandidatos.empty()) {
            QMessageBox::information(
                this, "Sin resultados",
                QString("No hay usuarios preinscritos para '%1'.").arg(univDestino)
            );
            return;
        }

        // Creamos un QDialog con scroll donde listamos cada candidato + botón "Matricular"
        QDialog dialog(this);
        dialog.setWindowTitle(QString("Candidatos para %1").arg(univDestino));
        dialog.resize(600, 400);

        QVBoxLayout* dialogLayout = new QVBoxLayout(&dialog);

        // ScrollArea para poder ver muchos candidatos
        QScrollArea* scrollArea = new QScrollArea(&dialog);
        QWidget* container = new QWidget(scrollArea);
        QVBoxLayout* containerLayout = new QVBoxLayout(container);

        // Por cada candidato, creamos un "row"
        for (int idx : indicesCandidatos) {
            const std::string& candLine = todasLineas[idx];

            QHBoxLayout* rowLayout = new QHBoxLayout();
            // Label con la info
            QLabel* infoLabel = new QLabel(QString::fromStdString(candLine));
            QPushButton* matBtn = new QPushButton("Matricular");

            // Conectar el botón "Matricular"
            connect(matBtn, &QPushButton::clicked, this, [this, &dialog, &todasLineas, idx, infoLabel, matBtn]() {
                // 1) Cambiar estado en la línea => "confirmada"
                std::string lin = todasLineas[idx];
                std::vector<std::string> partes;
                {
                    std::stringstream sst(lin);
                    std::string p;
                    while (std::getline(sst, p, ',')) {
                        partes.push_back(trim(p));
                    }
                }
                // partes[5] debería ser "en proceso"
                if (partes.size() > 5) {
                    partes[5] = "confirmada";
                }

                // 2) Guardarlo en inscritos.txt
                std::ofstream insFile("inscritos.txt", std::ios::app);
                if (!insFile.is_open()) {
                    QMessageBox::critical(&dialog, "Error", "No se pudo abrir inscritos.txt");
                    return;
                }
                // Reconstruimos la línea
                std::stringstream reconst;
                for (size_t k = 0; k < partes.size(); k++) {
                    reconst << partes[k];
                    if (k < partes.size() - 1) reconst << ", ";
                }
                insFile << reconst.str() << "\n";
                insFile.close();

                // 3) Eliminar del vector en memoria
                todasLineas[idx].clear(); // marcamos como vacío

                // 4) Reescribir preinscritos.txt sin esa línea
                {
                    std::ofstream outPre("preinscritos.txt");
                    if (!outPre.is_open()) {
                        QMessageBox::critical(&dialog, "Error", "Error reescribiendo preinscritos.txt");
                        return;
                    }
                    for (auto &l : todasLineas) {
                        if (!l.empty()) {
                            outPre << l << "\n";
                        }
                    }
                }

                // 5) Opcional: Quitar la fila de la interfaz
                infoLabel->setText("MATRICULADO");
                matBtn->setEnabled(false);

                QMessageBox::information(&dialog, "Matriculado",
                    "Usuario matriculado y movido a inscritos.txt.");
            });

            rowLayout->addWidget(infoLabel);
            rowLayout->addWidget(matBtn);

            containerLayout->addLayout(rowLayout);
        }

        container->setLayout(containerLayout);
        scrollArea->setWidget(container);
        scrollArea->setWidgetResizable(true);

        dialogLayout->addWidget(scrollArea);

        dialog.setLayout(dialogLayout);
        dialog.exec();
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    MainWindow w;
    w.show();

    return app.exec();
}