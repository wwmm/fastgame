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

        parent: applicationWindow().overlay

        ListView {
            id: presetsListView

            implicitWidth: Kirigami.Units.gridUnit * 30

            model: ListModel {
                id: listModel

                Component.onCompleted: {
                    for (var i = 0; i < 200; ++i) {
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

                // separatorVisible: true
                // alternatingBackground: true
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
                    Kirigami.ListItemDragHandle {
                        listItem: listItem
                        listView: presetsListView
                        onMoveRequested: (oldIndex, newIndex) => {
                            listModel.move(oldIndex, newIndex, 1);
                        }
                    }

                    Controls.Label {
                        Layout.fillWidth: true
                        height: Math.max(implicitHeight, Kirigami.Units.iconSizes.smallMedium)
                        text: model.title
                    }

                }

            }

        }

        background: Rectangle {
            color: Kirigami.Theme.backgroundColor
        }

        header: Controls.ToolBar {

            contentItem: ColumnLayout {
                Kirigami.ActionTextField {
                    id: presetName

                    visible: !globalDrawer.collapsed
                    Layout.fillWidth: true
                    placeholderText: i18n("New Preset Name")
                    rightActions: [
                        Kirigami.Action {
                            icon.name: "gtk-add-symbolic"
                            onTriggered: {
                                presetName.text = "";
                                presetName.accepted();
                                showPassiveNotification("New Preset Created!");
                            }
                        },
                        Kirigami.Action {
                            icon.name: "document-import-symbolic"
                            onTriggered: {
                                presetName.text = "";
                                presetName.accepted();
                                showPassiveNotification("Preset file imported!");
                            }
                        }
                    ]
                }

                Kirigami.SearchField {
                    id: presetSearch

                    visible: !globalDrawer.collapsed
                    Layout.fillWidth: true
                    placeholderText: i18n("Preset Name")
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

        footer: RowLayout {
            Controls.Label {
                text: qsTr("Last used Preset:")
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
                        icon.name: "file-library-symbolic"
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
