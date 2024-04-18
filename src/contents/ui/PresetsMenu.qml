import FGPresetsBackend
import FGPresetsMenuModel
import QtCore
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Dialogs
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

        Layout.fillWidth: true
        clip: true
        model: FGPresetsMenuModel
        delegate: listDelegate

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            width: parent.width - (Kirigami.Units.largeSpacing * 4)
            visible: presetsListView.count === 0
            text: i18n("No Preset")
        }

    }

    Component {
        id: listDelegate

        Controls.ItemDelegate {
            id: listItemDelegate

            property string presetName: model.name
            property int wrapMode: Text.WrapAnywhere
            property int elide: Text.ElideRight
            property bool selected: listItemDelegate.highlighted || listItemDelegate.down
            property color color: selected ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor

            hoverEnabled: true
            width: parent ? parent.width : implicitWidth
            onClicked: {
                let res = FGPresetsBackend.loadPreset(presetName);
                if (res === true) {
                    showPresetsMenuStatus(i18n("The Preset " + presetName + " Has Been Loaded"));
                    presetsMenuFooter.lastUsedPreset = presetName;
                }
            }

            contentItem: Kirigami.ActionToolBar {
                actions: [
                    Kirigami.Action {
                        text: presetName
                        enabled: false
                        displayHint: Kirigami.DisplayHint.KeepVisible
                    },
                    Kirigami.Action {
                        text: i18n("Save Settings to this Preset")
                        icon.name: "document-save-symbolic"
                        displayHint: Kirigami.DisplayHint.AlwaysHide
                        onTriggered: {
                            showPresetsMenuStatus(i18n("Settings Saved to: " + presetName));
                        }
                    },
                    Kirigami.Action {
                        text: i18n("Delete this Preset")
                        icon.name: "delete"
                        displayHint: Kirigami.DisplayHint.AlwaysHide
                        onTriggered: {
                            showPresetsMenuStatus(i18n("The Preset " + presetName + " Has Been Removed"));
                        }
                    }
                ]

                anchors {
                    left: parent.left
                    leftMargin: Kirigami.Units.smallSpacing
                    right: parent.right
                    rightMargin: Kirigami.Units.smallSpacing
                }

            }

        }

    }

    FileDialog {
        id: fileDialog

        fileMode: FileDialog.OpenFile
        currentFolder: StandardPaths.standardLocations(StandardPaths.DownloadLocation)[0]
        nameFilters: ["JSON files (*.json)"]
        onAccepted: showPresetsMenuStatus(i18n("Preset file imported!"))
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
                        fileDialog.open();
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
                FGPresetsMenuModel.filterRegularExpression = RegExp(presetSearch.text, "i");
            }
        }

    }

    footer: ColumnLayout {
        id: presetsMenuFooter

        property string lastUsedPreset

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
            text: i18n("Last Used Preset:    ") + presetsMenuFooter.lastUsedPreset
            color: Kirigami.Theme.disabledTextColor
        }

    }

}
