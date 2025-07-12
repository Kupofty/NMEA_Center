
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
    property double boatLatitude: -9999
    property double boatLongitude: -9999
    property double boatHeading: -9999
    property double boatDepth: -9999
    property double boatSpeed: -9999
    property double boatCourse: -9999
    property double boatWaterTemperature: -9999

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
            visible: boatDate !== ""
            width: labelRightSideWidth
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            padding: labelPadding
            background: Rectangle {
                color: labelColor
                radius :  labelBackgroundRadius
            }
            font.pixelSize: labelFontSize
            text: boatDate === "" ? ("Date: "+ noData) : "Date: " + boatDate
        }

        // Boat Time Label
        Label {
            id: timeLabel
            visible: boatTime !== ""
            width: labelRightSideWidth
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            padding: labelPadding
            background: Rectangle {
                color: labelColor
                radius :  labelBackgroundRadius
            }
            font.pixelSize: labelFontSize
            text: boatTime === "" ? ("Time: "+ noData): "Time: " + boatTime
        }

        // Boat Position Label
        Label {
            id: positionLabel
            visible: boatLatitude !== -9999
            width: labelRightSideWidth
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            padding: labelPadding
            background: Rectangle {
                color: labelColor
                radius :  labelBackgroundRadius
            }
            font.pixelSize: labelFontSize
            text: boatLatitude === -9999 ? ("Boat Position\n" + noData) : "Boat Position\nLat: " + boatLatitude.toFixed(6) + "\nLon: " + boatLongitude.toFixed(6)
        }

        // Heading Label
        Label {
            id: headingLabel
            visible: boatHeading !== -9999
            width: labelRightSideWidth
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            padding: labelPadding
            background: Rectangle {
                color: labelColor
                radius :  labelBackgroundRadius
            }
            font.pixelSize: labelFontSize
            text: boatHeading === -9999 ? ("Heading: " + noData) : "Heading: " + boatHeading.toFixed(1) + "°"
        }

        // Course Label
        Label {
            id: courseLabel
            visible: boatCourse !== -9999
            width: labelRightSideWidth
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            padding: labelPadding
            background: Rectangle {
                color: labelColor
                radius :  labelBackgroundRadius
            }
            font.pixelSize: labelFontSize
            text: boatCourse === -9999 ? ("Course: " + noData) : "Course: " + boatCourse.toFixed(1) + "°"
        }

        // Speed Label
        Label {
            id: speedLabel
            visible: boatSpeed !== -9999
            width: labelRightSideWidth
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            padding: labelPadding
            background: Rectangle {
                color: labelColor
                radius :  labelBackgroundRadius
            }
            font.pixelSize: labelFontSize
            text: boatSpeed === -9999 ? ("Speed: " + noData) : "Speed: " + boatSpeed.toFixed(1) + "kts"
        }

        // Depth Label
        Label {
            id: depthLabel
            visible: boatDepth !== -9999
            width: labelRightSideWidth
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            padding: labelPadding
            background: Rectangle {
                color: labelColor
                radius :  labelBackgroundRadius
            }
            font.pixelSize: labelFontSize
            text: boatDepth === -9999 ? ("Depth: " + noData) : "Depth: " + boatDepth.toFixed(1) + "m"
        }

        // WaterTemperature Label
        Label {
            id: waterTemperatureLabel
            visible: boatWaterTemperature !== -9999
            width: labelRightSideWidth
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            padding: labelPadding
            background: Rectangle {
                color: labelColor
                radius :  labelBackgroundRadius
            }
            font.pixelSize: labelFontSize
            text: boatWaterTemperature === -9999 ? ("Water Temp: " + noData) : "Water Temp: " + boatWaterTemperature.toFixed(1) + "°C"
        }

        // Add labels for Wind speed & Dir, water temp, utc time, etc
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
