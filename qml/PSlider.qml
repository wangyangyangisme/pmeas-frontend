import QtQuick 2.7
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0

import Theme 1.0

Slider {
    id: slider;

    stepSize: 1;

    hoverEnabled: true;

    implicitHeight: 16;

    implicitWidth: 150;

    background: Rectangle {
              x: slider.leftPadding
              y: slider.topPadding + slider.availableHeight / 2 - height / 2
              width: slider.availableWidth
              height: slider.implicitHeight / 3;

              radius: 2
              color: "#7a7a7a"

              Rectangle {
                  id: valueHighlighter
                  width: slider.visualPosition * parent.width
                  height: parent.height
                  color: Theme.highlighterColor
                  radius: 2
              }
          }

    handle: Item {
              x: slider.leftPadding + valueHighlighter.width - 5
              y: slider.topPadding + slider.availableHeight / 2 - height / 2

              implicitWidth: parent.height - 2;
              implicitHeight: implicitWidth;

              Rectangle {
                  id: handleBackground;
                  anchors.fill: parent;
                  radius: height / 2;
              }

              DropShadow {
                  anchors.fill: source;
                  horizontalOffset: 0;
                  verticalOffset: slider.hovered ? 2 : 0;
                  radius: slider.hovered ? 10.0 : 2.0;
                  samples: radius * 2;
                  color: "black";
                  source: handleBackground;

                  Behavior on verticalOffset {
                      PropertyAnimation {
                          duration: 50;
                          easing.type: Easing.InCubic;
                      }
                  }

                  cached: true;


              }

          }

}
