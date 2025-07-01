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
        zoomLevel: 10

        //Zoom
        WheelHandler {
            id: wheelZoom
            target: map

            onActiveChanged: {
                if (!active) return;

                if (rotation > 0)
                    map.zoomLevel = Math.min(map.maximumZoomLevel, map.zoomLevel + 0.1);
                else if (rotation < 0)
                    map.zoomLevel = Math.max(map.minimumZoomLevel, map.zoomLevel - 0.1);
            }
        }

    }

    // Label showing zoom level
    Label {
        id: zoomLabel
        anchors.top: parent.top
        anchors.right: parent.right
        padding: 8
        background: Rectangle {
            color: "lightgray"
            radius: 4
        }
        font.pixelSize: 14
        text: "Zoom Level: " + map.zoomLevel.toFixed(1)
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
            padding: 8
            background: Rectangle {
                color: "lightgray"
                radius: 4
            }
            font.pixelSize: 14
            text: "Cursor Position\nLat: %1\nLon: %2".arg(parent.coordinate.latitude.toFixed(6)).arg(parent.coordinate.longitude.toFixed(6))
        }

    }

}
