import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    visible: true
    width: 500
    height: 500
    title: "Graficacion - Tarea 4"

    Item {
        anchors.fill: parent
        focus: true

        Keys.onPressed: (event) => {

            switch (event.key) {

            case Qt.Key_Space:
                canvas.renderCube()
                break

            case Qt.Key_P:
                canvas.toggleProjection()
                break

            case Qt.Key_1:
                canvas.setCameraPreset(0)
                break

            case Qt.Key_2:
                canvas.setCameraPreset(1)
                break

            case Qt.Key_V:
                canvas.toggleDualView()
                break

            case Qt.Key_A:
                canvas.toggleAnimation()
                break

            case Qt.Key_Q:
                canvas.toggleWhiteLight()
                break

            case Qt.Key_W:
                canvas.toggleRedLight()
                break

            case Qt.Key_S:
                canvas.toggleShadingMode()
                break

            case Qt.Key_Up:
                canvas.increaseSpecularIntensity()
                break

            case Qt.Key_Down:
                canvas.decreaseSpecularIntensity()
                break
            
            case Qt.Key_F:
                canvas.toggleFilter()
                break
            
            

            event.accepted = true
        }
    }
}}