
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
    property string boatDate: "00/00/0000"
    property string boatTime: "00:00:00"
    property double boatLatitude : 0
    property double boatLongitude : 0
    property double boatHeading : 0
    property double boatDepth : 0
    property double boatSpeed: 0
    property double boatCourse : 0
    property double boatWaterTemperature : 0

    //Labels
    property int labelRightSideWidth : 130
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
    property int timeBeforePositionLost: 60
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
            lastCoord = map.toCoordinate(Qt.point(mouseX, mouseY))
            dragging = true
        }

        onReleased: dragging = false

        onPositionChanged: {
            if (dragging)
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
                text: "Center Map On Boat"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                width: parent.width
            }
            onTriggered: setCenterPositionOnBoat()
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

            elapsedSec = Math.ceil((Date.now() - timeLastPosition) / 1000)

            if(elapsedSec <= timeBeforePositionLost)
                textTimerPositionUpdate = "Position Update\n"+ elapsedSec + "s ago"
            else
                textTimerPositionUpdate = "Position Lost"
        }
    }



    ///////////////////////////////
    /// Data Labels / Left Side ///
    ///////////////////////////////
    // Label map type
    Label {
        id: mapLabel
        width: labelLeftSideWidth
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.top: parent.top
        anchors.topMargin: labelVerticalMargin * 2
        anchors.left: parent.left
        anchors.leftMargin: labelLateralMargin
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
        anchors.top: mapLabel.bottom
        anchors.topMargin: labelVerticalMargin
        anchors.left: parent.left
        anchors.leftMargin: labelLateralMargin
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
        anchors.top: zoomLabel.bottom
        anchors.topMargin: labelVerticalMargin
        anchors.left: parent.left
        anchors.leftMargin: labelLateralMargin
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
        anchors.top: cursorPosition.bottom
        anchors.topMargin: labelVerticalMargin
        anchors.left: parent.left
        anchors.leftMargin: labelLateralMargin
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
        anchors.top: mapViewMode.bottom
        anchors.topMargin: labelVerticalMargin
        anchors.left: parent.left
        anchors.leftMargin: labelLateralMargin
        padding: labelPadding
        background: Rectangle {
            color: labelColor
            radius :  labelBackgroundRadius
        }
        font.pixelSize: 14
        text: textTimerPositionUpdate
    }



    ////////////////////////////////
    /// Data Labels / Right Side ///
    ////////////////////////////////
    // Boat Date Label
    Label {
        id: dateLabel
        width: labelRightSideWidth
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.top: parent.top
        anchors.topMargin: labelVerticalMargin * 2
        anchors.right: parent.right
        anchors.rightMargin: labelLateralMargin
        padding: labelPadding
        background: Rectangle {
            color: labelColor
            radius :  labelBackgroundRadius
        }
        font.pixelSize: labelFontSize
        text: "Date: " + boatDate
    }

    // Boat Time Label
    Label {
        id: timeLabel
        width: labelRightSideWidth
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.top: dateLabel.bottom
        anchors.topMargin: labelVerticalMargin
        anchors.right: parent.right
        anchors.rightMargin: labelLateralMargin
        padding: labelPadding
        background: Rectangle {
            color: labelColor
            radius :  labelBackgroundRadius
        }
        font.pixelSize: labelFontSize
        text: "Time: " + boatTime
    }

    // Boat Position Label
    Label {
        id: positionLabel
        width: labelRightSideWidth
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.top: timeLabel.bottom
        anchors.topMargin: labelVerticalMargin
        anchors.right: parent.right
        anchors.rightMargin: labelLateralMargin
        padding: labelPadding
        background: Rectangle {
            color: labelColor
            radius :  labelBackgroundRadius
        }
        font.pixelSize: labelFontSize
        text: "Boat Position\nLat: " + boatLatitude.toFixed(6) + "\nLon: " + boatLongitude.toFixed(6)
    }

    // Heading Label
    Label {
        id: headingLabel
        width: labelRightSideWidth
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.top: positionLabel.bottom
        anchors.topMargin: labelVerticalMargin
        anchors.right: parent.right
        anchors.rightMargin: labelLateralMargin
        padding: labelPadding
        background: Rectangle {
            color: labelColor
            radius :  labelBackgroundRadius
        }
        font.pixelSize: labelFontSize
        text: "Heading: " + (boatHeading).toFixed(1) + "°"
    }

    // Course Label
    Label {
        id: courseLabel
        width: labelRightSideWidth
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.top: headingLabel.bottom
        anchors.topMargin: labelVerticalMargin
        anchors.right: parent.right
        anchors.rightMargin: labelLateralMargin
        padding: labelPadding
        background: Rectangle {
            color: labelColor
            radius :  labelBackgroundRadius
        }
        font.pixelSize: labelFontSize
        text: "Course: " + (boatCourse).toFixed(1) + "°"
    }

    // Speed Label
    Label {
        id: speedLabel
        width: labelRightSideWidth
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.top: courseLabel.bottom
        anchors.topMargin: labelVerticalMargin
        anchors.right: parent.right
        anchors.rightMargin: labelLateralMargin
        padding: labelPadding
        background: Rectangle {
            color: labelColor
            radius :  labelBackgroundRadius
        }
        font.pixelSize: labelFontSize
        text: "Speed: " + (boatSpeed).toFixed(1) + "kts"
    }

    // Depth Label
    Label {
        id: depthLabel
        width: labelRightSideWidth
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.top: speedLabel.bottom
        anchors.topMargin: labelVerticalMargin
        anchors.right: parent.right
        anchors.rightMargin: labelLateralMargin
        padding: labelPadding
        background: Rectangle {
            color: labelColor
            radius :  labelBackgroundRadius
        }
        font.pixelSize: labelFontSize
        text: "Depth: " + (boatDepth).toFixed(1) + "m"
    }

    // WaterTemperature Label
    Label {
        id: waterTemperatureLabel
        width: labelRightSideWidth
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.top: depthLabel.bottom
        anchors.topMargin: labelVerticalMargin
        anchors.right: parent.right
        anchors.rightMargin: labelLateralMargin
        padding: labelPadding
        background: Rectangle {
            color: labelColor
            radius :  labelBackgroundRadius
        }
        font.pixelSize: labelFontSize
        text: "Water Temp: " + (boatWaterTemperature).toFixed(1) + "°C"
    }

    // Add labels for Wind speed & Dir, water temp, utc time, etc



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
    }

    //Update boat UTC Date
    function updateBoatDate(date) {
        boatDate = date
    }

    //Update boat position
    function updateBoatPositiong(lat, lon) {
        boatLatitude = lat
        boatLongitude = lon

        timeLastPosition = Date.now()
    }

    // Update boat heading
    function updateBoatHeading(head) {
        boatHeading = head
    }

    // Update boat depth
    function updateBoatDepth(depth) {
        boatDepth = depth
    }

    // Update boat speed
    function updateBoatSpeed(speed) {
        boatSpeed = speed
    }

    // Update boat course
    function updateBoatCourse(course) {
        boatCourse = course
    }

    // Update boat water temperature
    function updateBoatWaterTemperature(temp) {
        boatWaterTemperature = temp
    }


}
