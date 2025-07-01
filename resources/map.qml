import QtQuick
import QtQuick.Controls
import QtLocation
import QtPositioning

Item {

    //OSM Map
    Plugin {
        id: mapPlugin
        name: "osm"
        locales: "fr_FR"
    }

    Map {
        id: map
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(43.5, 5.3167)
        zoomLevel: 14
    }


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
            anchors.left: parent.left
            anchors.top: parent.top
            padding: 6
            background: Rectangle {
                color: "lightgray"
                radius: 4
            }
            text: "Lat: %1\nLon: %2".arg(parent.coordinate.latitude.toFixed(6)).arg(parent.coordinate.longitude.toFixed(6))
        }

    }

}
