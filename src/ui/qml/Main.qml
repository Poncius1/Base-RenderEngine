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

    property bool loadingModel: false

    function loadDefaultModel() {
        loadingModel = true
        loadTimer.start()
    }

    Timer {
    id: loadTimer
    interval: 100
    repeat: false
    onTriggered: {
        canvas.loadObj("D:/Visual Projects/Graficacion1/assets/models/teapot.obj")
        loadingModel = false
    }
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
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 18

            width: cameraTitle.width + 42
            height: 44
            radius: 22
            color: "#99000000"
            border.color: "#334155"
            border.width: 1

            Text {
                id: cameraTitle
                anchors.centerIn: parent
                text: canvas.activeCameraName
                color: "white"
                font.bold: true
                font.pixelSize: 22
            }
        }

        // Settings en esquina
        Rectangle {
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: 20

            width: 285
            height: 210
            radius: 16
            color: "#990F172A"
            border.color: "#334155"
            border.width: 1

            Column {
                anchors.fill: parent
                anchors.margins: 18
                spacing: 10

                Text {
                    text: "Scene Settings"
                    color: "#7DBBFF"
                    font.bold: true
                    font.pixelSize: 18
                }

                Rectangle {
                    width: parent.width
                    height: 1
                    color: "#334155"
                }

                Text { text: "Projection: " + canvas.projectionName; color: "#E5E7EB"; font.pixelSize: 14 }
                Text { text: "Material: " + canvas.activeMaterialName; color: "#E5E7EB"; font.pixelSize: 14 }
                Text { text: canvas.blueLightState; color: "#E5E7EB"; font.pixelSize: 14 }
                Text { text: canvas.animationState; color: "#E5E7EB"; font.pixelSize: 14 }
            }
        }

        // Controles abajo en fila
        Rectangle {
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottomMargin: 18

            height: 54
            width: controlsRow.width + 44
            radius: 18
            color: "#99000000"
            border.color: "#334155"
            border.width: 1

            Row {
                id: controlsRow
                anchors.centerIn: parent
                spacing: 26

                Text { text: "[C] Camera"; color: "#E5E7EB"; font.pixelSize: 14 }
                Text { text: "[M] Material"; color: "#E5E7EB"; font.pixelSize: 14 }
                Text { text: "[P] Projection"; color: "#E5E7EB"; font.pixelSize: 14 }
                Text { text: "[L] Blue Light"; color: "#E5E7EB"; font.pixelSize: 14 }
                Text { text: "[R] Rotation"; color: "#E5E7EB"; font.pixelSize: 14 }
            }
        }

        Rectangle {
            anchors.fill: parent
            visible: root.loadingModel
            color: "#0B0B0F"
            z: 100

            Column {
                anchors.centerIn: parent
                spacing: 18

                Text {
                    text: "Cargando OBJ..."
                    color: "white"
                    font.bold: true
                    font.pixelSize: 28
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                BusyIndicator {
                    running: root.loadingModel
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }

    }
}