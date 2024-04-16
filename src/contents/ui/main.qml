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

    Amdgpu {
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

    PresetsMenu {
        id: presetsMenu
    }

    Kirigami.OverlayDrawer {
        id: progressBottomDrawer

        edge: Qt.BottomEdge
        modal: false
        parent: applicationWindow().overlay
        drawerOpen: false

        contentItem: RowLayout {
            Controls.ProgressBar {
                from: 0
                to: 100
                indeterminate: true
                Layout.fillWidth: true
            }

            Controls.Button {
                Layout.alignment: Qt.AlignRight
                text: i18n("Close")
                onClicked: progressBottomDrawer.close()
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
                icon.name: "show-gpu-effects-symbolic"
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
            },
            Kirigami.Action {
                text: amdgpu.title
                icon.name: "show-gpu-effects-symbolic"
                checked: amdgpu.visible
                onTriggered: {
                    if (!amdgpu.visible) {
                        while (pageStack.depth > 0)pageStack.pop()
                        pageStack.push(amdgpu);
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
                        onTriggered: {
                            progressBottomDrawer.open();
                        }
                    },
                    Kirigami.Action {
                        text: i18n("Presets")
                        icon.name: "bookmarks-symbolic"
                        displayHint: Kirigami.DisplayHint.KeepVisible
                        onTriggered: {
                            presetsMenu.open();
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
