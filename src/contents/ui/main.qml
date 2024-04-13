import AboutFG
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

Kirigami.ApplicationWindow {
    id: root

    pageStack.initialPage: environmentVariables
    pageStack.globalToolBar.style: Kirigami.Settings.isMobile ? Kirigami.ApplicationHeaderStyle.Titles : Kirigami.ApplicationHeaderStyle.Auto
    title: i18nc("@title:window", "FastGame")

    EnvironmentVariables {
        id: environmentVariables

        visible: true
    }

    CommandLineArguments {
        id: cmdArguments

        visible: false
    }

    Cpu {
        id: cpu

        visible: false
    }

    Memory {
        id: memory

        visible: false
    }

    Disk {
        id: disk

        visible: false
    }

    Kirigami.Page {
        id: amdgpu

        title: i18n("AMDGPU")
        visible: false
    }

    Kirigami.Dialog {
        id: aboutDialog

        Kirigami.AboutPage {
            implicitWidth: Kirigami.Units.gridUnit * 24
            implicitHeight: Kirigami.Units.gridUnit * 21
            aboutData: AboutFG
        }

    }

    Kirigami.OverlaySheet {
        id: presetsSheet

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

            model: ListModel {
                id: listModel

                Component.onCompleted: {
                    for (var i = 0; i < 100; ++i) {
                        listModel.append({
                            "title": "Item " + i
                        });
                    }
                }
            }

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
                        onTriggered: print("saved")
                    },
                    Kirigami.Action {
                        icon.name: "delete"
                        onTriggered: print("removed")
                    }
                ]

                contentItem: RowLayout {
                    Controls.Label {
                        Layout.fillWidth: true
                        height: Math.max(implicitHeight, Kirigami.Units.iconSizes.smallMedium)
                        text: model.title
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
                            presetsSheet.showPresetsMenuStatus(i18n("Preset file imported!"));
                        }
                    },
                    Kirigami.Action {
                        text: i18n("Create Preset")
                        icon.name: "list-add-symbolic"
                        onTriggered: {
                            presetName.accepted();
                            presetsSheet.showPresetsMenuStatus(i18n("New Preset Created: " + presetName.text));
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

    globalDrawer: Kirigami.GlobalDrawer {
        id: globalDrawer

        drawerOpen: true
        showHeaderWhenCollapsed: true
        collapsible: true
        modal: Kirigami.Settings.isMobile ? true : false
        actions: [
            Kirigami.Action {
                text: environmentVariables.title
                icon.name: "document-properties-symbolic"
                checked: environmentVariables.visible
                onTriggered: {
                    if (!environmentVariables.visible) {
                        while (pageStack.depth > 0)pageStack.pop()
                        pageStack.push(environmentVariables);
                    }
                }
            },
            Kirigami.Action {
                text: cmdArguments.title
                icon.name: "dialog-scripts"
                checked: cmdArguments.visible
                onTriggered: {
                    if (!cmdArguments.visible) {
                        while (pageStack.depth > 0)pageStack.pop()
                        pageStack.push(cmdArguments);
                    }
                }
            },
            Kirigami.Action {
                text: cpu.title
                icon.name: "computer"
                checked: cpu.visible
                onTriggered: {
                    if (!cpu.visible) {
                        while (pageStack.depth > 0)pageStack.pop()
                        pageStack.push(cpu);
                    }
                }
            },
            Kirigami.Action {
                text: memory.title
                icon.name: "media-flash-memory-stick"
                checked: memory.visible
                onTriggered: {
                    if (!memory.visible) {
                        while (pageStack.depth > 0)pageStack.pop()
                        pageStack.push(memory);
                    }
                }
            },
            Kirigami.Action {
                text: disk.title
                icon.name: "drive-harddisk"
                checked: disk.visible
                onTriggered: {
                    if (!disk.visible) {
                        while (pageStack.depth > 0)pageStack.pop()
                        pageStack.push(disk);
                    }
                }
            }
        ]

        header: Kirigami.AbstractApplicationHeader {

            contentItem: Kirigami.ActionToolBar {
                actions: [
                    Kirigami.Action {
                        text: i18n("Apply")
                        icon.name: "dialog-ok-apply-symbolic"
                        displayHint: Kirigami.DisplayHint.KeepVisible
                        onTriggered: showPassiveNotification("Apply!")
                    },
                    Kirigami.Action {
                        text: i18n("Presets")
                        icon.name: "bookmarks-symbolic"
                        displayHint: Kirigami.DisplayHint.KeepVisible
                        onTriggered: {
                            presetsSheet.open();
                        }
                    },
                    Kirigami.Action {
                        text: i18n("Preferences")
                        icon.name: "gtk-preferences"
                        displayHint: Kirigami.DisplayHint.AlwaysHide
                    },
                    Kirigami.Action {
                        text: i18n("About FastGame")
                        icon.name: "fastgame"
                        displayHint: Kirigami.DisplayHint.AlwaysHide
                        onTriggered: {
                            aboutDialog.open();
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

        footer: Controls.ToolBar {

            contentItem: RowLayout {
                Kirigami.ActionTextField {
                    id: executableName

                    visible: !globalDrawer.collapsed
                    Layout.fillWidth: true
                    placeholderText: i18n("Executable Name")
                    rightActions: [
                        Kirigami.Action {
                            icon.name: "edit-clear"
                            onTriggered: {
                                executableName.text = "";
                                executableName.accepted();
                            }
                        }
                    ]
                }

            }

        }

    }

}
