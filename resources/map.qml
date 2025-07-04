
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
    property double latitude : 43.5
    property double longitude : 5.3

    property double boatLatitude : 0
    property double boatLongitude : 0
    property double boatHeading : 0

    property Component locationmarker: locmarker
    property Component boatMapMarker: boatmarker


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
        zoomLevel: 10
        activeMapType: map.supportedMapTypes[map.supportedMapTypes.length - 1]

        //Zoom
        WheelHandler {
            id: wheelZoom
            target: map

            onActiveChanged: {
                if (!active) return;

                //Need to fix zoom problem, "rotation" keep last state of wheel
                if (rotation > 0)
                    map.zoomLevel = Math.min(map.maximumZoomLevel, map.zoomLevel + 0.1);
                else if (rotation < 0)
                    map.zoomLevel = Math.max(map.minimumZoomLevel, map.zoomLevel - 0.1);
            }
        }
    }



    ////////////////////////
    /// Marker Component ///
    ////////////////////////
    Component {
        id: locmarker
        MapQuickItem {
            id: markerImg
            anchorPoint.x: image.width/4
            anchorPoint.y: image.height
            coordinate: position
            sourceItem: Image {
                id: image
                width: 20
                height: 20
                source: "https://www.pngarts.com/files/3/Map-Marker-Pin-PNG-Image-Background.png" //blue marker
            }
        }
    }

    Component {
        id: boatmarker
        MapQuickItem {
            id: boatMarkerImg
            anchorPoint.x: image.width/4
            anchorPoint.y: image.height
            coordinate: position
            sourceItem: Image {
                id: image
                width: 35
                height: 45
                source: "https://static.vecteezy.com/system/resources/previews/008/501/633/non_2x/wooden-boat-clip-art-free-png.png" //wooden boat
            }
        }
    }



    ///////////////////
    /// Data Labels ///
    ///////////////////

    // Label showing zoom level
    Label {
        id: zoomLabel
        anchors.top: parent.top
        anchors.topMargin: 90
        anchors.left: parent.left
        anchors.leftMargin: 8
        padding: 8
        background: Rectangle {
            color: "darkgrey"
            radius: 4
        }
        font.pixelSize: 14
        text: "Zoom Level: " + map.zoomLevel.toFixed(1)
    }

    // Boat Position Label
    Label {
        id: positionLabel
        anchors.top: parent.top
        anchors.topMargin: 8
        anchors.right: parent.right
        anchors.rightMargin: 8
        padding: 8
        background: Rectangle {
            color: "darkgrey"
            radius: 4
        }
        font.pixelSize: 14
        text: "Boat Position\nLat: " + boatLatitude.toFixed(5) + "\nLon: " + boatLongitude.toFixed(5)
    }

    // Heading Label
    Label {
        id: headingLabel
        anchors.top: positionLabel.bottom
        anchors.topMargin: 8
        anchors.right: parent.right
                anchors.rightMargin: 8
        padding: 8
        background: Rectangle {
            color: "darkgrey"
            radius: 4
        }
        font.pixelSize: 14
        text: "Heading: " + (boatHeading).toFixed(1) + "°"
    }

    // Add labels for SOG, COG, Depth, Wind speed & Dir, water temp, utc time


    //////////////////
    /// Mouse Area ///
    //////////////////
    MouseArea
    {
        id: mouseArea
        anchors.fill: map
        hoverEnabled: true
        property var lastCoord
        property bool dragging: false

        property var coordinate: map.toCoordinate(Qt.point(mouseX, mouseY))

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
        }

        // Label showing cursor position
        Label {
            id: cursorPosition
            anchors.left: parent.left
            anchors.leftMargin: 8
            anchors.top: parent.top
            anchors.topMargin: 8
            padding: 8
            background: Rectangle {
                color: "darkgrey"
                radius: 4
            }
            font.pixelSize: 14
            text: "Cursor Position\nLat: %1\nLon: %2".arg(parent.coordinate.latitude.toFixed(6)).arg(parent.coordinate.longitude.toFixed(6))
        }

    }



    ////////////////////////
    /// Functions / SLOT ///
    ////////////////////////
    //Go To New Position
    function setCenterPosition(lat, lon) {
        map.pan(latitude - lat, longitude - lon) //add dx dy
        latitude = lat
        longitude = lon
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
            rotation: boatHeading+180 //boat png is upside down
        });
        map.addMapItem(item)
    }

    //Remove all markers from map
    function clearMarkers() {
        map.clearMapItems()
    }

    //Update boat positoin
    function updateBoatPositiong(lat, lon) {
        boatLatitude = lat
        boatLongitude = lon
    }

    // Update boat heading
    function updateBoatHeading(head) {
        boatHeading = head
    }

}
