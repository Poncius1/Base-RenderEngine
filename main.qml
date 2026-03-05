import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    id: win
    visible: true
    width: 400
    height: 400
    title: "Graficacion - Tarea 3"

    Item {
        anchors.fill: parent
        focus: true

        Keys.onPressed: (event) => {

            if (event.key === Qt.Key_Space) {
                canvas.renderCube()
                event.accepted = true

            } else if (event.key === Qt.Key_P) {
                canvas.toggleProjection()
                event.accepted = true

            } else if (event.key === Qt.Key_1) {
                canvas.setCameraPreset(0)
                event.accepted = true

            } else if (event.key === Qt.Key_2) {
                canvas.setCameraPreset(1)
                event.accepted = true

            } else if (event.key === Qt.Key_V) {
                canvas.toggleDualView()
                event.accepted = true

            } else if (event.key === Qt.Key_A) {
                canvas.toggleAnimation()
                event.accepted = true
            }
        }
       
    }
}