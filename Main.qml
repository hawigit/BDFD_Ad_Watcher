import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: parentWindow
    width: 640
    height: 480
    visible: true
    minimumWidth: 200
    minimumHeight: 100
    title: "BDFD Ad Watcher v.5.0"
    TabBar {
        id: bar
        width: parent.width
        anchors.bottom: parent.bottom
        TabButton {
            text: qsTr("Main Tab")
        }
        TabButton {
            text: qsTr("Configurations")
        }
        TabButton {
            text: qsTr("Set Click Location")
        }
        TabButton {
            text: qsTr("Set Template")
        }
    }
    StackLayout {
        id: stackLayout
        currentIndex: bar.currentIndex
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: bar.top
        anchors.leftMargin: parent.width*0.01
        anchors.rightMargin: parent.width*0.01
        Rectangle {
            id: tab1
            width: parent.width
            height: parent.height
            Text {
                id: title
                text: "BDFD Ad watcher 5.0"
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: Math.min(Math.max(12, parent.width * 0.1), 40)
            }
            Text {
                id: title2
                text: "Bluestacks Portrait layout, 900x1600."
                anchors.left: parent.left
                anchors.top: title.bottom
                leftPadding: parent.width*0.01
            }
            Rectangle {
                id: outerBorder
                border.width: 2
                anchors.top: title2.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                Rectangle {
                    id: mainBorder
                    anchors.top: parent.top
                    anchors.left: parent.left
                    width: Math.min(Math.max(19, parentWindow.width * 0.25), 1000)
                    anchors.bottom: parent.bottom
                    color: "transparent"
                    border.color: "black"
                    ColumnLayout {
                        id: maincol
                        anchors.fill: parent
                        anchors.margins: 10
                        Button {
                            id: connectButton
                            contentItem: Text {
                                text: "Connect / Disconnect to bluestacks"
                                wrapMode: Text.WordWrap
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.verticalCenter: parent.verticalCenter
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            Layout.preferredHeight: 1
                            onClicked: {
                                backend.connectBlueStacks()
                            }
                        }
                        Button {
                            id: startButton
                            text: "Start"
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            Layout.preferredHeight: 1
                            onClicked: {
                                backend.start()
                            }
                        }
                        Button {
                            id: stopButton
                            text: "Stop"
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            Layout.preferredHeight: 1
                            onClicked: {
                                backend.stop()
                            }
                        }
                    }
                }
                Rectangle {
                    id: outputFrame
                    anchors.left: mainBorder.right
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    color: "transparent"
                    border.color: "black"
                    clip: true
                    Text {
                        id: outputTitle
                        text: "Output"
                        anchors.top: parent.top
                        anchors.horizontalCenter: outputFrame.horizontalCenter
                        font.bold: true
                        height: outputFrame.height * 0.1
                        width: outputFrame.width * 0.1
                        fontSizeMode: Text.Fit
                        minimumPixelSize: 5
                        font.pixelSize: 75
                    }
                    ScrollView {
                        id: outputScroll
                        anchors.top: outputTitle.bottom
                        anchors.bottom: outputFrame.bottom // Fills the remaining space!
                        anchors.left: outputFrame.left
                        anchors.right: outputFrame.right
                        anchors.leftMargin: 10
                        anchors.rightMargin: 10
                        clip: true // Ensures text doesn't spill out

                        TextArea {
                            id: outputText
                            readOnly: true
                            wrapMode: Text.Wrap
                            text: ">>"

                            Connections {
                                target: backend
                                function onLogUpdated(msg) {
                                    outputText.append(">> " + msg)
                                    outputText.cursorPosition = outputText.length - 1
                                }
                            }
                        }
                    }

                }
            }
        }
        // Tab 1: Configurations Tab
        Rectangle {
            id: configurationsTab
            anchors.fill: parent
            anchors.margins: parentWindow.width * 0.02

            ColumnLayout {
                anchors.fill: parent
                spacing: 15

                Text {
                    text: "General Configurations"
                    font.pixelSize: 18
                    font.bold: true
                }

                Text {
                    text: "Connect to BlueStacks before configuring or capturing anything."
                    color: "#555555"
                    font.pixelSize: 13
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 60
                    border.color: "#cccccc"
                    border.width: 1
                    radius: 4

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 10

                        Text {
                            text: "BlueStacks ADB Port:"
                            font.bold: true
                            font.pixelSize: 14
                        }

                        TextField {
                            id: adbPortInput
                            Layout.fillWidth: true
                            text: backend.adbPort
                            placeholderText: "127.0.0.1:5555"
                            onTextEdited: backend.updatePort(adbPortInput.text)
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 70
                    border.color: "#cccccc"
                    border.width: 1
                    radius: 4

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 10

                        Button {
                            id: btnGetPos
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            contentItem: Text {
                                text: "Get Position + Color"
                                wrapMode: Text.WordWrap
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 13
                            }
                            onClicked: () => {
                                backend.captureScreen()
                                getPosScreen.mode = 0
                                getPosScreen.visible = true
                            }
                        }

                        Button {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            contentItem: Text {
                                text: "Capture Screen"
                                wrapMode: Text.WordWrap
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 13
                            }
                            onClicked: backend.captureScreen()
                        }
                    }
                }

                Item {
                    Layout.fillHeight: true // Spacing filler
                }
            }
        }

        // Tab 2: Set Click Location Tab
        Rectangle {
            id: clickLocationTab
            anchors.fill: parent
            anchors.margins: parentWindow.width * 0.02

            ColumnLayout {
                anchors.fill: parent
                spacing: 10

                Text {
                    text: "Set Click Locations (1 Click)"
                    font.pixelSize: 18
                    font.bold: true
                }

                Text {
                    text: "Click a button below, then make a single click on the emulator preview to set that coordinate."
                    color: "#555555"
                    font.pixelSize: 13
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    GridLayout {
                        width: parent.width
                        columns: 2
                        columnSpacing: 10
                        rowSpacing: 10

                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 50
                            contentItem: Text {
                                text: "Set Ad Watch Button location"
                                wrapMode: Text.WordWrap
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 13
                            }
                            onClicked: () => {
                                backend.captureScreen()
                                getPosScreen.mode = 3
                                getPosScreen.visible = true
                            }
                        }

                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 50
                            contentItem: Text {
                                text: "Set Main X button location"
                                wrapMode: Text.WordWrap
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 13
                            }
                            onClicked: () => {
                                backend.captureScreen()
                                getPosScreen.mode = 1
                                getPosScreen.visible = true
                            }
                        }

                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 50
                            contentItem: Text {
                                text: "Set Left X button location"
                                wrapMode: Text.WordWrap
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 13
                            }
                            onClicked: () => {
                                backend.captureScreen()
                                getPosScreen.mode = 8
                                getPosScreen.visible = true
                            }
                        }

                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 50
                            contentItem: Text {
                                text: "Set X button 2 location"
                                wrapMode: Text.WordWrap
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 13
                            }
                            onClicked: () => {
                                backend.captureScreen()
                                getPosScreen.mode = 2
                                getPosScreen.visible = true
                            }
                        }

                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 50
                            contentItem: Text {
                                text: "Set Ad Continue Button location"
                                wrapMode: Text.WordWrap
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 13
                            }
                            onClicked: () => {
                                backend.captureScreen()
                                getPosScreen.mode = 7
                                getPosScreen.visible = true
                            }
                        }

                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 50
                            contentItem: Text {
                                text: "Set Captcha Textbox location"
                                wrapMode: Text.WordWrap
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 13
                            }
                            onClicked: () => {
                                backend.captureScreen()
                                getPosScreen.mode = 4
                                getPosScreen.visible = true
                            }
                        }

                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 50
                            contentItem: Text {
                                text: "Set Captcha Confirm Button location"
                                wrapMode: Text.WordWrap
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 13
                            }
                            onClicked: () => {
                                backend.captureScreen()
                                getPosScreen.mode = 5
                                getPosScreen.visible = true
                            }
                        }

                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 50
                            contentItem: Text {
                                text: "Capture Screen (Refresh Preview)"
                                wrapMode: Text.WordWrap
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 13
                            }
                            onClicked: backend.captureScreen()
                        }
                    }
                }
            }
        }

        // Tab 3: Set Template Tab
        Rectangle {
            id: templateTab
            anchors.fill: parent
            anchors.margins: parentWindow.width * 0.02

            ColumnLayout {
                anchors.fill: parent
                spacing: 10

                Text {
                    text: "Set Templates (2 Clicks)"
                    font.pixelSize: 18
                    font.bold: true
                }

                Text {
                    text: "Click a button below, then make 2 clicks (Top-Left, then Bottom-Right) on the preview to crop."
                    color: "#555555"
                    font.pixelSize: 13
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    GridLayout {
                        width: parent.width
                        columns: 2
                        columnSpacing: 10
                        rowSpacing: 10

                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 45
                            contentItem: Text {
                                text: "Capture Ad Watch Button"
                                wrapMode: Text.WordWrap
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 13
                            }
                            onClicked: () => {
                                backend.captureScreen()
                                getPosScreen.mode = 6
                                getPosScreen.widgetName = "adWatchButton"
                                getPosScreen.visible = true
                            }
                        }

                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 45
                            contentItem: Text {
                                text: "Capture white-themed X button"
                                wrapMode: Text.WordWrap
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 13
                            }
                            onClicked: () => {
                                backend.captureScreen()
                                getPosScreen.mode = 6
                                getPosScreen.widgetName = "XButton1"
                                getPosScreen.visible = true
                            }
                        }

                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 45
                            contentItem: Text {
                                text: "Capture dark-themed X button"
                                wrapMode: Text.WordWrap
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 13
                            }
                            onClicked: () => {
                                backend.captureScreen()
                                getPosScreen.mode = 6
                                getPosScreen.widgetName = "XButton1_black"
                                getPosScreen.visible = true
                            }
                        }

                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 45
                            contentItem: Text {
                                text: "Capture skip ad button"
                                wrapMode: Text.WordWrap
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 13
                            }
                            onClicked: () => {
                                backend.captureScreen()
                                getPosScreen.mode = 6
                                getPosScreen.widgetName = "XButton1_skip"
                                getPosScreen.visible = true
                            }
                        }

                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 45
                            contentItem: Text {
                                text: "Capture popup X button"
                                wrapMode: Text.WordWrap
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 13
                            }
                            onClicked: () => {
                                backend.captureScreen()
                                getPosScreen.mode = 6
                                getPosScreen.widgetName = "XButton2"
                                getPosScreen.visible = true
                            }
                        }

                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 45
                            contentItem: Text {
                                text: "Capture captcha textbox"
                                wrapMode: Text.WordWrap
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 13
                            }
                            onClicked: () => {
                                backend.captureScreen()
                                getPosScreen.mode = 6
                                getPosScreen.widgetName = "captchaTextbox"
                                getPosScreen.visible = true
                            }
                        }

                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 45
                            contentItem: Text {
                                text: "Capture success message"
                                wrapMode: Text.WordWrap
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 13
                            }
                            onClicked: () => {
                                backend.captureScreen()
                                getPosScreen.mode = 6
                                getPosScreen.widgetName = "successScreen"
                                getPosScreen.visible = true
                            }
                        }

                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 45
                            contentItem: Text {
                                text: "Capture success message (v2)"
                                wrapMode: Text.WordWrap
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 13
                            }
                            onClicked: () => {
                                backend.captureScreen()
                                getPosScreen.mode = 6
                                getPosScreen.widgetName = "successScreen2"
                                getPosScreen.visible = true
                            }
                        }

                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 45
                            contentItem: Text {
                                text: "Capture ad continue screen"
                                wrapMode: Text.WordWrap
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 13
                            }
                            onClicked: () => {
                                backend.captureScreen()
                                getPosScreen.mode = 6
                                getPosScreen.widgetName = "adContinueScreen"
                                getPosScreen.visible = true
                            }
                        }

                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 45
                            contentItem: Text {
                                text: "Capture ad waiting screen"
                                wrapMode: Text.WordWrap
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 13
                            }
                            onClicked: () => {
                                backend.captureScreen()
                                getPosScreen.mode = 6
                                getPosScreen.widgetName = "adWaitingScreen"
                                getPosScreen.visible = true
                            }
                        }
                    }
                }
            }
        }
    }

    // Shared Screen Picker Overlay
    Rectangle {
        id: getPosScreen
        anchors.fill: parent
        visible: false
        z: 50
        property string widgetName: "placeholder"
        property int clickCount: 1
        property int mode: 0
        property int savedWidth: 640
        property int savedHeight: 480

        function adjustWindowRatio() {
            if (getPosImg.sourceSize.width > 0 && getPosImg.sourceSize.height > 0) {
                let ratio = getPosImg.sourceSize.width / getPosImg.sourceSize.height;
                let targetWidth = Math.round(parentWindow.height * ratio);
                if (targetWidth >= parentWindow.minimumWidth) {
                    parentWindow.width = targetWidth;
                } else {
                    parentWindow.width = parentWindow.minimumWidth;
                    parentWindow.height = Math.round(parentWindow.minimumWidth / ratio);
                }
            }
        }

        onVisibleChanged: {
            if (visible) {
                savedWidth = parentWindow.width;
                savedHeight = parentWindow.height;
                adjustWindowRatio();
            } else {
                parentWindow.width = savedWidth;
                parentWindow.height = savedHeight;
            }
        }

        Image {
            id: getPosImg
            source: backend.imgPath
            anchors.fill: parent

            onStatusChanged: {
                if (status === Image.Ready && getPosScreen.visible) {
                    getPosScreen.adjustWindowRatio();
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: (mouse) => {
                    if (getPosScreen.mode == 0) {
                        backend.getPos(mouse.x, mouse.y, parent.width, parent.height, 0)
                        getPosScreen.visible = false
                    } else if (getPosScreen.mode == 1) {
                        backend.getPos(mouse.x, mouse.y, parent.width, parent.height, 1)
                        getPosScreen.visible = false
                    } else if (getPosScreen.mode == 2) {
                        backend.getPos(mouse.x, mouse.y, parent.width, parent.height, 2)
                        getPosScreen.visible = false
                    } else if (getPosScreen.mode == 3) {
                        backend.getPos(mouse.x, mouse.y, parent.width, parent.height, 3)
                        getPosScreen.visible = false
                    } else if (getPosScreen.mode == 4) {
                        backend.getPos(mouse.x, mouse.y, parent.width, parent.height, 4)
                        getPosScreen.visible = false
                    } else if (getPosScreen.mode == 5) {
                        backend.getPos(mouse.x, mouse.y, parent.width, parent.height, 5)
                        getPosScreen.visible = false
                    } else if (getPosScreen.mode == 6) {
                        if (getPosScreen.clickCount < 2) {
                            backend.getPos(mouse.x, mouse.y, parent.width, parent.height, 6, getPosScreen.clickCount)
                            getPosScreen.clickCount += 1
                        } else {
                            backend.getPos(mouse.x, mouse.y, parent.width, parent.height, 6, getPosScreen.clickCount, getPosScreen.widgetName)
                            getPosScreen.clickCount = 1
                            getPosScreen.widgetName = "placeholder"
                            getPosScreen.visible = false
                        }
                    } else if (getPosScreen.mode == 7) {
                        backend.getPos(mouse.x, mouse.y, parent.width, parent.height, 7)
                        getPosScreen.visible = false
                    } else if (getPosScreen.mode == 8) {
                        backend.getPos(mouse.x, mouse.y, parent.width, parent.height, 8)
                        getPosScreen.visible = false
                    }
                }
            }
        }

        Button {
            id: getPosCancel
            text: "Cancel"
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.margins: 15
            height: 40
            width: 100
            onClicked: () => {
                getPosScreen.visible = false
                getPosScreen.clickCount = 1
                getPosScreen.widgetName = "placeholder"
            }
        }
    }

    Rectangle {
        id: loadingOverlay
        anchors.fill: parent
        color: "#f5f5f5"
        visible: !backend.isReady
        z: 999

        ColumnLayout {
            anchors.centerIn: parent
            spacing: 20

            BusyIndicator {
                running: !backend.isReady
                Layout.alignment: Qt.AlignHCenter
            }

            Text {
                text: "Initializing OCR server..."
                font.pixelSize: 16
                font.bold: true
                color: "#333333"
                Layout.alignment: Qt.AlignHCenter
            }
        }
    }
}


