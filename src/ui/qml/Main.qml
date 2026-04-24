import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import GfxEngine 1.0

Window {
    id: root
    width: 1200
    height: 780
    visible: true
    color: "#0B0B0F"
    title: "Angel Ponce - Render Engine"

    function loadDefaultModel() {
        canvas.loadObj("D:/Visual Projects/Graficacion1/assets/models/dragon.obj")
    }

    Component.onCompleted: {
        loadDefaultModel()
    }

    Item {
        anchors.fill: parent
        focus: true

        Keys.onPressed: function(event) {
            switch (event.key) {
            case Qt.Key_C:
                canvas.nextCamera()
                event.accepted = true
                break
            case Qt.Key_M:
                canvas.toggleMaterial()
                event.accepted = true
                break
            case Qt.Key_P:
                canvas.toggleProjection()
                event.accepted = true
                break
            case Qt.Key_L:
                canvas.toggleSecondaryLight()
                event.accepted = true
                break
            case Qt.Key_R:
                canvas.toggleAnimation()
                event.accepted = true
                break
            }
        }

        PixelCanvas {
            id: canvas
            anchors.fill: parent
        }

        Rectangle {
    anchors.left: parent.left
    anchors.top: parent.top
    anchors.margins: 18
    width: 320
    radius: 16
    color: "#AA0F172A"
    border.color: "#334155"
    border.width: 1

    Column {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 10

       

       

        Rectangle {
            width: parent.width
            height: 1
            color: "#334155"
        }

        Text {
            text: "Estado actual"
            color: "#93C5FD"
            font.pixelSize: 16
            font.bold: true
        }

        Text { text: "Camara: " + canvas.activeCameraName; color: "white"; font.pixelSize: 15 }
        Text { text: "Proyeccion: " + canvas.projectionName; color: "white"; font.pixelSize: 15 }
        Text { text: "Material: " + canvas.activeMaterialName; color: "white"; font.pixelSize: 15 }
        Text { text: "Textura: " + canvas.activeTextureName; color: "white"; font.pixelSize: 15 }
        Text { text: canvas.blueLightState; color: "white"; font.pixelSize: 15 }
        Text { text: canvas.animationState; color: "white"; font.pixelSize: 15 }

        Rectangle {
            width: parent.width
            height: 1
            color: "#334155"
        }

        Text {
            text: "Controles"
            color: "#93C5FD"
            font.pixelSize: 16
            font.bold: true
        }

        Text { text: "[C] Cambiar camara"; color: "#E5E7EB"; font.pixelSize: 14 }
        Text { text: "[M] Cambiar material"; color: "#E5E7EB"; font.pixelSize: 14 }
        Text { text: "[P] Cambiar proyeccion"; color: "#E5E7EB"; font.pixelSize: 14 }
        Text { text: "[L] Encender/apagar luz azul"; color: "#E5E7EB"; font.pixelSize: 14 }
        Text { text: "[R] Iniciar/parar rotacion"; color: "#E5E7EB"; font.pixelSize: 14 }
    }
}
    }
}