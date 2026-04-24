
# Base-RenderEngine

Este repositorio contiene la implementación hasta la **Tarea 2** del curso de **Graficación Computacional**.

La rama principal (`main`) contiene siempre la versión más reciente de la tarea actual.  
Para tareas posteriores se crearán nuevas ramas específicas por cada entrega.

---

## 📁 Estructura del proyecto

```

Base-RenderEngine/
├── LICENSE.txt                # Licencia del proyecto
├── Graficacion1.slnx          # Solución de Visual Studio
├── Graficacion1.vcxproj       # Proyecto de Visual Studio (Qt + QML)
├── main.cpp                   # Punto de entrada de la aplicación
├── main.qml                   # Interfaz gráfica (Qt Quick)
├── pixelbuffer.h              # Estructura del buffer de píxeles
├── pixelcanvas.h              # Declaración de la clase PixelCanvas
├── pixelcanvas.cpp            # Implementación del canvas
├── qml.qrc                    # Archivo de recursos QML
└── README.md                  # Documentación del repositorio

````

---

## clonar y compilar el proyecto

### Clonar el repositorio

Abrir una terminal (Git Bash o CMD) y ejecutar:
```bash
git clone https://github.com/Poncius1/Base-RenderEngine.git
cd Base-RenderEngine
````

---

### Abrir en Visual Studio

1. Abrir **Visual Studio**.
2. Seleccionar **Open a project or solution**.
3. Abrir el archivo:

```
Graficacion1.slnx
```

---

### Compilar el proyecto

1. Seleccionar configuración: **Debug**
2. Seleccionar plataforma: **x64**
3. Ir a **Build → Build Solution**

---

El ejecutable se generará en la carpeta:

```
x64/Debug/
```
