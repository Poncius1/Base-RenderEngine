import QtQuick 2.9
import QtQuick.Window 2.2

Window {
    id: win
    visible: true
    width: 457
    height: 600
    title: "Graficacion - Tarea 2"
    color: "black"

    // Cursor lógico
    property int cx: Math.floor(width / 2)
    property int cy: Math.floor(height / 2)
    property color currentColor: "red"


    // Cursor visible (marcador)
    Rectangle {
        id: cursor
        width: 6
        height: 6
        x: win.cx - width / 2
        y: win.cy - height / 2
        color: "transparent"
        border.color: "white"
        border.width: 1
        z: 2
    }

    // Coordenadas (arriba izquierda)
    Text {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 10
        color: "white"
        font.pointSize: 12
        text: "Pixel: (" + win.cx + ", " + win.cy + ")"
        z: 3
    }

    // Controles (abajo izquierda)
    Text {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 10
        color: "white"
        font.pointSize: 12
        text: "Flechas: Mover | SPACE: Pintar | R/G/B: Color | C: Limpiar | P: Random"
        z: 3
    }

    // Input (teclado)
    Item {
        anchors.fill: parent
        focus: true

        Keys.onPressed: function(event) {
            switch (event.key) {
            case Qt.Key_Left:
                win.cx = Math.max(0, win.cx - 1)
                break
            case Qt.Key_Right:
                win.cx = Math.min(win.width - 1, win.cx + 1)
                break
            case Qt.Key_Up:
                win.cy = Math.max(0, win.cy - 1)
                break
            case Qt.Key_Down:
                win.cy = Math.min(win.height - 1, win.cy + 1)
                break
            case Qt.Key_Space:
                canvas.setPixel(win.cx, win.cy, win.currentColor)
                break
            case Qt.Key_C:
                canvas.clear()
                break
            case Qt.Key_P:
                canvas.randomPixels(2000)
                break
            case Qt.Key_R:
                win.currentColor = "red"
                break
            case Qt.Key_G:
                win.currentColor = "lime"
                break
            case Qt.Key_B:
                win.currentColor = "deepskyblue"
                break
            default:
                return
            }

            event.accepted = true
        }
    }
}
