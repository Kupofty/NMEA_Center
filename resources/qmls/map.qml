
////////////////////////
/// Import libraries ///
////////////////////////
import QtQuick
import QtQuick.Controls
import QtLocation
import QtPositioning



Item {
    id: window

    ////////////////////////
    /// Global variables ///
    ////////////////////////
    //Position
    property double latitude : 35
    property double longitude : 0

    //Cursor
    property double cursorLatitude : NaN
    property double cursorLongitude: NaN

    //Boat data
    property string noData: "No Data"
    property string boatDate: ""
    property string boatTime: ""
    property double boatLatitude: 0
    property double boatLongitude: 0
    property double boatHeading: 0
    property double boatDepth: 0
    property double boatSpeed: 0
    property double boatCourse: 0
    property double boatWaterTemperature: 0

    //Boat data received check
    property bool boatDateReceived: false
    property bool boatTimeReceived: false
    property bool boatPositionReceived: false
    property bool boatHeadingReceived: false
    property bool boatCourseReceived: false
    property bool boatDepthReceived: false
    property bool boatSpeedReceived: false
    property bool boatWaterTempratureReceived: false

    //Labels
    property int labelRightSideWidth : 140
    property int labelLeftSideWidth : 130
    property int labelPadding : 8
    property int labelLateralMargin : 8
    property int labelVerticalMargin : 8
    property int labelFontSize : 14
    property int labelBackgroundRadius : 4
    property string labelColor : "grey"

    //Zoom
    property double mapZoomLevel : 3
    property double zoomSpeed : 0.2
    property double lastWheelRotation : 0

    //Markers
    property Component locationmarker: locmarker
    property Component boatMapMarker: boatmarker

    //Right-click Menu
    property int rightClickMenuWidth: 150

    //Map
    property bool zoomedIn : false
    property bool headingUpView: false
    property real mapRotation: headingUpView ? boatHeading : 0

    //Timer Position Update
    property int timeBeforePositionLost: 30
    property double timeLastPosition: 0
    property double elapsedSec: 0
    property string textTimerPositionUpdate: "No Position Data"



    //////////////
    /// Plugin ///
    //////////////
    Plugin {
        id: osmView
        name: "osm"
        locales: "fr_FR"
        PluginParameter {
            name:"osm.mapping.custom.host"
            value: "https://tile.openstreetmap.org/"
        }
    }



    ///////////
    /// Map ///
    ///////////
    Map {
        id: map
        anchors.fill: parent
        plugin: osmView
        center: QtPositioning.coordinate(latitude, longitude)
        zoomLevel: mapZoomLevel
        activeMapType: map.supportedMapTypes[map.supportedMapTypes.length - 1]
        bearing: mapRotation
    }



    ////////////
    /// Zoom ///
    ////////////
    WheelHandler {
        id: wheelZoom
        target: map

        onWheel: {
            var wheelRotation = rotation - lastWheelRotation
            if (wheelRotation > 0)
                map.zoomLevel = Math.min(map.maximumZoomLevel, map.zoomLevel + zoomSpeed);
            else if (wheelRotation < 0)
                map.zoomLevel = Math.max(map.minimumZoomLevel, map.zoomLevel - zoomSpeed);
            lastWheelRotation = rotation
            mapZoomLevel = map.zoomLevel
        }
    }



    //////////////////
    /// Mouse Area ///
    //////////////////
    MouseArea
    {
        id: mouseArea
        anchors.fill: map
        hoverEnabled: true
        acceptedButtons: Qt.RightButton | Qt.LeftButton

        property var lastCoord
        property bool dragging: false
        property var coordinate: map.toCoordinate(Qt.point(mouseX, mouseY))

        //Dragging map
        onPressed: {
            if (mouse.button === Qt.LeftButton) {
                lastCoord = map.toCoordinate(Qt.point(mouseX, mouseY))
                dragging = true
            }
        }

        onReleased: dragging = false

        onPositionChanged: {
            if (dragging && mouse.buttons === Qt.LeftButton)
            {
                var currentCoord = map.toCoordinate(Qt.point(mouseX, mouseY))
                var dx = lastCoord.longitude - currentCoord.longitude
                var dy = lastCoord.latitude - currentCoord.latitude
                map.center = QtPositioning.coordinate(map.center.latitude + dy, map.center.longitude + dx)
                lastCoord = map.toCoordinate(Qt.point(mouseX, mouseY))
            }
            else
                lastCoord = map.toCoordinate(Qt.point(mouseX, mouseY))

            cursorLatitude = coordinate.latitude
            cursorLongitude = coordinate.longitude
        }

        // Right-click menu
        onClicked: {
             if (mouse.button === Qt.RightButton) {
                 contextMenu.popup()
             }
         }
    }



    ////////////////////////
    /// Right-click Menu ///
    ////////////////////////
    Menu {
        id: contextMenu
        width: rightClickMenuWidth

        MenuItem {
            contentItem: Label {
                text: "Center Map On Boat"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                width: parent.width
            }
            onTriggered: setCenterPositionOnBoat()
        }

        MenuItem {
            contentItem: Label {
                text: zoomedIn ? "Large View" : "Close View"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                width: parent.width
            }
            onTriggered:{
                goToZoomLevelMap(zoomedIn ? 12 : 17)
                zoomedIn = !zoomedIn
            }
        }

        MenuItem {
            contentItem: Label {
                text: headingUpView ? "North Up" : "Heading Up"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                width: parent.width
            }
            onTriggered: {
                headingUpView = !headingUpView
            }
        }

        MenuItem {
            contentItem: Label {
                text: "Clear Map"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                width: parent.width
            }
            onTriggered: clearMarkers()
        }
    }



    ////////////////////////
    /// Marker Component ///
    ////////////////////////
    Component {
        id: locmarker
        MapQuickItem {
            id: markerImg
            anchorPoint.x: image.width / 2
            anchorPoint.y: image.height / 2
            coordinate: position

            sourceItem: Image {
                id: image
                width: 20
                height: 20
                source: "qrc:/markers/blueMarker"
            }
        }
    }

    Component {
        id: boatmarker
        MapQuickItem {
            id: boatMarkerImg
            anchorPoint.x: image.width / 2
            anchorPoint.y: image.height / 2
            coordinate: position

            sourceItem: Image {
                id: image
                width: 20
                height: 60
                source: "qrc:/boats/boat1"
                opacity: elapsedSec > timeBeforePositionLost ? 0.5 : 1.0
            }
        }
    }



    //////////////
    /// Timers ///
    //////////////
    Timer {
        id: updateTimer
        interval: 1000
        running: true
        repeat: true
        onTriggered: {
            if (timeLastPosition === 0) {
                return
            }

            elapsedSec = (Date.now() - timeLastPosition) / 1000

            if(elapsedSec <= timeBeforePositionLost)
                if(elapsedSec < 1)
                    textTimerPositionUpdate = "Position Updated\n < 1s ago"
                else
                    textTimerPositionUpdate = "Position Updated\n"+ Math.ceil(elapsedSec) + "s ago"
            else
                textTimerPositionUpdate = "Position Lost"
        }
    }



    ///////////////////////////////
    /// Data Labels / Left Side ///
    ///////////////////////////////
    Column {
        id: leftSideInfoColumn
        anchors.top: parent.top
        anchors.topMargin: labelVerticalMargin * 2
        anchors.left: parent.left
        anchors.leftMargin: labelLateralMargin
        spacing: labelVerticalMargin

        // Label map type
        Label {
            id: mapLabel
            width: labelLeftSideWidth
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            padding: labelPadding
            background: Rectangle {
                color: labelColor
                radius :  labelBackgroundRadius
            }
            font.pixelSize: 14
            text: "Chart: OSM "
        }

        // Label showing zoom level
        Label {
            id: zoomLabel
            width: labelLeftSideWidth
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            padding: labelPadding
            background: Rectangle {
                color: labelColor
                radius :  labelBackgroundRadius
            }
            font.pixelSize: 14
            text: "Zoom Level: " + mapZoomLevel.toFixed(1)
        }

        // Label showing cursor position
        Label {
            id: cursorPosition
            width: labelLeftSideWidth
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            padding: labelPadding
            background: Rectangle {
                color: labelColor
                radius :  labelBackgroundRadius
            }
            font.pixelSize: 14
            text: "Cursor Position\nLat: " + cursorLatitude.toFixed(6) + "\nLon: " + cursorLongitude.toFixed(6)
        }

        // Label Map View Mode
        Label {
            id: mapViewMode
            width: labelLeftSideWidth
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            padding: labelPadding
            background: Rectangle {
                color: labelColor
                radius :  labelBackgroundRadius
            }
            font.pixelSize: 14
            text: headingUpView ? "Heading Up" : "North Up"
        }

        // Label Timer Last Position Update
        Label {
            id: elapsedLabel
            width: labelLeftSideWidth
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            padding: labelPadding
            background: Rectangle {
                color: elapsedSec > timeBeforePositionLost ? "indianred" : labelColor
                radius :  labelBackgroundRadius
            }
            font.pixelSize: 14
            text: textTimerPositionUpdate
        }

    }



    ////////////////////////////////
    /// Data Labels / Right Side ///
    ////////////////////////////////
    Column {
        id: rightSideInfoColumn
        anchors.top: parent.top
        anchors.topMargin: labelVerticalMargin * 2
        anchors.right: parent.right
        anchors.rightMargin: labelLateralMargin
        spacing: labelVerticalMargin

        // Boat Date Label
        Label {
            id: dateLabel
            visible: boatDateReceived
            width: labelRightSideWidth
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            padding: labelPadding
            background: Rectangle {
                color: labelColor
                radius :  labelBackgroundRadius
            }
            font.pixelSize: labelFontSize
            text: boatDateReceived ? "Date: " + boatDate
                                   : "Date: "+ noData
        }

        // Boat Time Label
        Label {
            id: timeLabel
            visible: boatTimeReceived
            width: labelRightSideWidth
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            padding: labelPadding
            background: Rectangle {
                color: labelColor
                radius :  labelBackgroundRadius
            }
            font.pixelSize: labelFontSize
            text: boatTimeReceived ? "Time: " + boatTime
                                   : "Time: "+ noData
        }

        // Boat Position Label
        Label {
            id: positionLabel
            visible: boatPositionReceived
            width: labelRightSideWidth
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            padding: labelPadding
            background: Rectangle {
                color: labelColor
                radius :  labelBackgroundRadius
            }
            font.pixelSize: labelFontSize
            text: boatPositionReceived ? "Boat Position\nLat: " + boatLatitude.toFixed(6) + "\nLon: " + boatLongitude.toFixed(6)
                                       : "Boat Position\n" + noData
        }

        // Heading Label
        Label {
            id: headingLabel
            visible: boatHeadingReceived
            width: labelRightSideWidth
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            padding: labelPadding
            background: Rectangle {
                color: labelColor
                radius :  labelBackgroundRadius
            }
            font.pixelSize: labelFontSize
            text: boatHeadingReceived ? "Heading: " + boatHeading.toFixed(1) + "°"
                                      : "Heading: " + noData
        }

        // Course Label
        Label {
            id: courseLabel
            visible: boatCourseReceived
            width: labelRightSideWidth
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            padding: labelPadding
            background: Rectangle {
                color: labelColor
                radius :  labelBackgroundRadius
            }
            font.pixelSize: labelFontSize
            text: boatCourseReceived ? "Course: " + boatCourse.toFixed(1) + "°"
                                     : "Course: " + noData
        }

        // Speed Label
        Label {
            id: speedLabel
            visible: boatSpeedReceived
            width: labelRightSideWidth
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            padding: labelPadding
            background: Rectangle {
                color: labelColor
                radius :  labelBackgroundRadius
            }
            font.pixelSize: labelFontSize
            text: boatSpeedReceived ? "Speed: " + boatSpeed.toFixed(1) + "kts"
                                    : "Speed: " + noData
        }

        // Depth Label
        Label {
            id: depthLabel
            visible: boatDepthReceived
            width: labelRightSideWidth
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            padding: labelPadding
            background: Rectangle {
                color: labelColor
                radius :  labelBackgroundRadius
            }
            font.pixelSize: labelFontSize
            text: boatDepthReceived ? "Depth: " + boatDepth.toFixed(1) + "m"
                                    : "Depth: " + noData
        }

        // WaterTemperature Label
        Label {
            id: waterTemperatureLabel
            visible: boatWaterTempratureReceived
            width: labelRightSideWidth
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            padding: labelPadding
            background: Rectangle {
                color: labelColor
                radius :  labelBackgroundRadius
            }
            font.pixelSize: labelFontSize
            text: boatWaterTempratureReceived ? "Water Temp: " + boatWaterTemperature.toFixed(1) + "°C"
                                              : "Water Temp: " + noData
        }
    }


    ///////////////////
    /// Data Canvas ///
    ///////////////////
    Canvas {
        id: compassCanvas
        width: 150
        height: 150
        visible: boatHeadingReceived
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.bottomMargin: labelVerticalMargin
        anchors.rightMargin: labelLateralMargin

        property real heading: boatHeading
        property real course: boatCourse

        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()
            var centerX = width / 2
            var centerY = height / 2
            var radius = Math.min(width, height) / 2 - 10

            // Draw compass circle
            ctx.beginPath()
            ctx.strokeStyle = "black"
            ctx.lineWidth = 2
            ctx.arc(centerX, centerY, radius, 0, 2 * Math.PI)
            ctx.stroke()

            // Draw cardinal directions
            ctx.fillStyle = "black"
            ctx.font = "bold 14px sans-serif"
            ctx.textAlign = "center"
            ctx.textBaseline = "middle"
            ctx.fillText("N",  centerX, centerY - radius + 10);
            ctx.fillText("E",  centerX + radius - 10, centerY);
            ctx.fillText("S",  centerX, centerY + radius - 10);
            ctx.fillText("W",  centerX - radius + 10, centerY);

            // Intercardinal directions
            ctx.font = "10px sans-serif"
            ctx.fillText("NE", centerX + radius * 0.55, centerY - radius * 0.55);
            ctx.fillText("SE", centerX + radius * 0.55, centerY + radius * 0.55);
            ctx.fillText("SW", centerX - radius * 0.55, centerY + radius * 0.55);
            ctx.fillText("NW", centerX - radius * 0.55, centerY - radius * 0.55);

            // Draw heading arrow
            ctx.save()
            ctx.translate(centerX, centerY)
            ctx.rotate((heading - 0) * Math.PI / 180)

            ctx.beginPath()
            ctx.moveTo(0, -radius + 10)
            ctx.lineTo(5, 0)
            ctx.lineTo(-5, 0)
            ctx.closePath()

            ctx.fillStyle = "red"
            ctx.fill()
            ctx.restore()

            // Draw course arrow
            ctx.save()
            ctx.translate(centerX, centerY)
            ctx.rotate((course - 0) * Math.PI / 180)

            ctx.beginPath()
            ctx.moveTo(0, -radius + 15)  // same length
            ctx.lineTo(3, 0)             // narrower base
            ctx.lineTo(-3, 0)
            ctx.closePath()

            ctx.fillStyle = "blue"
            ctx.fill()
            ctx.restore()
        }

        // Redraw when heading changes
        Connections {
            target: compassCanvas
            onHeadingChanged: compassCanvas.requestPaint()
            onCourseChanged: compassCanvas.requestPaint()
        }
    }


    //////////////////
    /// Update Map ///
    //////////////////
    //Go To New Position
    function setCenterPosition(targetLat, targetLon) {
        map.center = QtPositioning.coordinate(targetLat, targetLon)
    }

    //Go To New Position
    function setCenterPositionOnBoat() {
        map.center = QtPositioning.coordinate(boatLatitude, boatLongitude)
    }

    //Add marker
    function setLocationMarking(lat, lon) {
        var item = locationmarker.createObject(window, {
            coordinate: QtPositioning.coordinate(lat, lon),
        });
        map.addMapItem(item)
    }

    //Add boat icon
    function updateBoatMap() {
        var item = boatMapMarker.createObject(window, {
            coordinate: QtPositioning.coordinate(boatLatitude, boatLongitude),
            rotation: boatHeading - mapRotation
        });
        map.addMapItem(item)
    }

    //Remove all markers from map
    function clearMarkers() {
        map.clearMapItems()
    }

    // Increment Map Zoom
    function incrementZoomMap(dz) {
        if (dz > 0)
            map.zoomLevel = Math.min(map.maximumZoomLevel, map.zoomLevel + dz);
        else if (dz < 0)
            map.zoomLevel = Math.max(map.minimumZoomLevel, map.zoomLevel + dz);
        mapZoomLevel = map.zoomLevel
    }

    // Go To Zoom Level Map
    function goToZoomLevelMap(zoomLevel) {
        map.zoomLevel = zoomLevel;
        mapZoomLevel = map.zoomLevel
    }



    ///////////////////
    /// Update Data ///
    ///////////////////
    //Update boat UTC time
    function updateBoatTime(time) {
        boatTime = time
        boatTimeReceived = true
    }

    //Update boat UTC Date
    function updateBoatDate(date) {
        boatDate = date
        boatDateReceived = true
    }

    //Update boat position
    function updateBoatPositiong(lat, lon) {
        boatLatitude = lat
        boatLongitude = lon

        timeLastPosition = Date.now()
        boatPositionReceived = true
    }

    // Update boat heading
    function updateBoatHeading(head) {
        boatHeading = head
        boatHeadingReceived = true
    }

    // Update boat depth
    function updateBoatDepth(depth) {
        boatDepth = depth
        boatDepthReceived = true
    }

    // Update boat speed
    function updateBoatSpeed(speed) {
        boatSpeed = speed
        boatSpeedReceived = true
    }

    // Update boat course
    function updateBoatCourse(course) {
        boatCourse = course
        boatCourseReceived = true
    }

    // Update boat water temperature
    function updateBoatWaterTemperature(temp) {
        boatWaterTemperature = temp
        boatWaterTempratureReceived = true
    }

}

