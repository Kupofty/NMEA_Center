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


}
