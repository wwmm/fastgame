import PresetsMenuModel
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.OverlaySheet {
    id: control

    function showPresetsMenuStatus(label) {
        presetsMenuStatus.text = label;
        presetsMenuStatus.visible = true;
    }

    parent: applicationWindow().overlay
    showCloseButton: false
    implicitWidth: Kirigami.Units.gridUnit * 30
    implicitHeight: root.height * 0.75

    ListView {
        id: presetsListView

        model: PresetsMenuModel

        moveDisplaced: Transition {
            YAnimator {
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutQuad
            }

        }

        delegate: Kirigami.SwipeListItem {
            id: listItem

            separatorVisible: true
            // alternatingBackground: true
            backgroundColor: Kirigami.Theme.neutralBackgroundColor
            width: ListView.view ? ListView.view.width : implicitWidth
            actions: [
                Kirigami.Action {
                    icon.name: "document-save"
                    onTriggered: print("saved:" + model.name)
                },
                Kirigami.Action {
                    icon.name: "delete"
                    onTriggered: print("removed: " + model.name)
                }
            ]

            contentItem: RowLayout {
                Controls.Label {
                    Layout.fillWidth: true
                    height: Math.max(implicitHeight, Kirigami.Units.iconSizes.smallMedium)
                    text: model.name
                }

            }

        }

    }

    header: ColumnLayout {
        Kirigami.ActionTextField {
            id: presetName

            Layout.fillWidth: true
            placeholderText: i18n("New Preset Name")
            // based on https://github.com/KDE/kirigami/blob/master/src/controls/SearchField.qml
            leftPadding: {
                if (effectiveHorizontalAlignment === TextInput.AlignRight)
                    return _rightActionsRow.width + Kirigami.Units.smallSpacing;
                else
                    return presetCreationIcon.width + Kirigami.Units.smallSpacing * 3;
            }
            rightPadding: {
                if (effectiveHorizontalAlignment === TextInput.AlignRight)
                    return presetCreationIcon.width + Kirigami.Units.smallSpacing * 3;
                else
                    return _rightActionsRow.width + Kirigami.Units.smallSpacing;
            }
            rightActions: [
                Kirigami.Action {
                    text: i18n("Import Preset File")
                    icon.name: "document-import-symbolic"
                    onTriggered: {
                        presetName.text = "";
                        presetName.accepted();
                        showPresetsMenuStatus(i18n("Preset file imported!"));
                    }
                },
                Kirigami.Action {
                    text: i18n("Create Preset")
                    icon.name: "list-add-symbolic"
                    onTriggered: {
                        presetName.accepted();
                        showPresetsMenuStatus(i18n("New Preset Created: " + presetName.text));
                        presetName.text = "";
                    }
                }
            ]

            Kirigami.Icon {
                id: presetCreationIcon

                LayoutMirroring.enabled: presetName.effectiveHorizontalAlignment === TextInput.AlignRight
                anchors.left: presetName.left
                anchors.leftMargin: Kirigami.Units.smallSpacing * 2
                anchors.verticalCenter: presetName.verticalCenter
                anchors.verticalCenterOffset: Math.round((presetName.topPadding - presetName.bottomPadding) / 2)
                implicitHeight: Kirigami.Units.iconSizes.sizeForLabels
                implicitWidth: Kirigami.Units.iconSizes.sizeForLabels
                color: presetName.placeholderTextColor
                source: "bookmarks-symbolic"
            }

        }

        Kirigami.SearchField {
            id: presetSearch

            Layout.fillWidth: true
            placeholderText: i18n("Search")
            onAccepted: {
                console.log(presetSearch.text);
            }
        }

    }

    footer: ColumnLayout {
        Kirigami.InlineMessage {
            id: presetsMenuStatus

            Layout.fillWidth: true
            visible: false
            showCloseButton: true
            Layout.maximumWidth: parent.width

            Timer {
                interval: 3000
                running: presetsMenuStatus.visible
                repeat: false
                onTriggered: presetsMenuStatus.visible = false
            }

        }

        Controls.Label {
            Layout.fillWidth: true
            text: i18n("Last used Preset:")
        }

    }

}
