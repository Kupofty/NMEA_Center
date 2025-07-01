import QtQuick
import QtQuick.Controls
import QtLocation
import QtPositioning

Item {

    id: window

    property double latitude : 43.5
    property double longitude : 5.3167

    property Component locationmarker: locmarker

    //OSM Map
    Plugin {
        id: osmView
        name: "osm"
        locales: "fr_FR"
        PluginParameter {
            name:"osm.mapping.custom.host"
            value: "https://tile.openstreetmap.org/"
        }
    }

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

                //Fix zoom problem, "rotation" keep last state of wheel
                if (rotation > 0)
                    map.zoomLevel = Math.min(map.maximumZoomLevel, map.zoomLevel + 0.1);
                else if (rotation < 0)
                    map.zoomLevel = Math.max(map.minimumZoomLevel, map.zoomLevel - 0.1);
            }
        }
    }

    //Go To New Position
    function setCenterPosition(lat, lon) {
        map.pan(latitude - lat, longitude - lon) //add dx dy
        latitude = lat
        longitude = lon
    }

    //Set marker
    function setLocationMarking(lat, lon) {
        var item = locationmarker.createObject(window, {coordinate:QtPositioning.coordinate(lat, lon)})
        map.addMapItem(item)
    }

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
                source: "https://www.pngarts.com/files/3/Map-Marker-Pin-PNG-Image-Background.png"
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
            color: "darkgrey"
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
                color: "darkgrey"
                radius: 4
            }
            font.pixelSize: 14
            text: "Cursor Position\nLat: %1\nLon: %2".arg(parent.coordinate.latitude.toFixed(6)).arg(parent.coordinate.longitude.toFixed(6))
        }

    }

}
