# =============================
# Makefile
# =============================
CXX = g++
CXXFLAGS = -std=c++17 -Wall -fPIC
INCLUDES = -I.
QTFLAGS = `pkg-config --cflags --libs Qt5Widgets` 
# Asegúrate de tener pkg-config y qtbase5-dev instalados
# sudo apt-get install pkg-config qtbase5-dev

# Archivos compartidos
SHARED_HEADERS = funciones.h
SHARED_SOURCES = funciones.cpp
SHARED_OBJECTS = $(SHARED_SOURCES:.cpp=.o)

# ----------------------
# 1) Consola
# ----------------------
CONSOLE_TARGET = sistema_sicue_console
CONSOLE_SOURCES = main_console.cpp
CONSOLE_OBJECTS = $(CONSOLE_SOURCES:.cpp=.o)

$(CONSOLE_TARGET): $(CONSOLE_OBJECTS) $(SHARED_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# ----------------------
# 2) GUI (Qt)
# ----------------------
GUI_TARGET = sistema_sicue_gui
GUI_SOURCES = main_gui.cpp
GUI_OBJECTS = $(GUI_SOURCES:.cpp=.o)

$(GUI_TARGET): $(GUI_OBJECTS) $(SHARED_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(GUI_TARGET) $(GUI_OBJECTS) $(SHARED_OBJECTS) $(QTFLAGS)

# ----------------------
# Reglas de compilación
# ----------------------
%.o: %.cpp $(SHARED_HEADERS)
	$(CXX) $(CXXFLAGS) $(QTFLAGS) $(INCLUDES) -c $< -o $@

all: $(CONSOLE_TARGET) $(GUI_TARGET)

clean:
	rm -f *.o $(CONSOLE_TARGET) $(GUI_TARGET)