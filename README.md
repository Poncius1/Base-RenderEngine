
# Base-RenderEngine

Este repositorio contiene la implementación del **Proyecto Final** del curso de **Graficación Computacional**.

Incluye un **render engine completamente desarrollado en CPU**, con pipeline gráfico propio, rasterización, iluminación y soporte para modelos OBJ.

La rama principal (`master`) contiene la versión final entregable del proyecto.

---

## 📁 Estructura del proyecto

```

Base-RenderEngine/
├── assets/                    # Modelos (.obj)
│   └── models/
├── src/
│   ├── core/                 # Matemáticas (Vec, Mat, utilidades)
│   ├── renderer/             # Pipeline de render
│   │   ├── pipeline/         # RenderEngine
│   │   ├── stages/           # Rasterizer, VertexProcessor
│   │   └── utils/            # Texturas procedurales
│   ├── scene/                # Cámara, luces, materiales
│   ├── loaders/              # Carga de modelos OBJ
│   ├── app/                  # SceneBuilder y presets
│   └── ui/                   # Qt + QML (interfaz)
├── Graficacion1.slnx         # Solución de Visual Studio
├── Graficacion1.vcxproj      # Proyecto (Qt + QML)
├── main.cpp                  # Punto de entrada
├── main.qml                  # Interfaz gráfica
├── README.md                 # Documentación
└── LICENSE.txt
---
```

## Características principales

- Pipeline gráfico completo en CPU:
  - Transformaciones (Model, View, Projection)
  - Rasterización por triángulos
  - Z-buffer
- Iluminación:
  - Modelo de Phong
  - Soporte para múltiples luces
- Shading:
  - Gouraud 
  - Phong 
- Cámaras:
  - Front-Right 3/4
  - Right Profile
  - Rear View
- Proyecciones:
  - Perspectiva
  - Ortográfica
- Materiales:
  - Material A (verde)
  - Material B (perla/azulado)
- Texturas:
  - Procedurales 
- UI en Qt/QML:
  - Control de cámaras
  - Materiales
  - Animación
  - Luces

## 🎮 Controles

| Tecla | Acción |
|------|--------|
| C | Cambiar cámara |
| M | Cambiar material |
| P | Cambiar proyección |
| L | Activar/desactivar luz azul |
| R | Activar/desactivar rotación |

---

## 📦 Clonar y compilar el proyecto

```bash
git clone https://github.com/Poncius1/Base-RenderEngine.git
cd Base-RenderEngine

---

### Abrir en Visual Studio

1. Abrir **Visual Studio**.
2. Seleccionar **Open a project or solution**.
3. Abrir el archivo:
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
