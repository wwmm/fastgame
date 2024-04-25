import AboutFG
import CfgWindow
import FGPresetsBackend
import Qt.labs.platform
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

Kirigami.ApplicationWindow {
    id: root

    width: CfgWindow.width
    height: CfgWindow.height
    pageStack.initialPage: environmentVariables
    pageStack.globalToolBar.style: Kirigami.Settings.isMobile ? Kirigami.ApplicationHeaderStyle.Titles : Kirigami.ApplicationHeaderStyle.Auto
    title: i18nc("@title:window", "FastGame")
    onWidthChanged: {
        CfgWindow.width = applicationWindow().width;
    }
    onHeightChanged: {
        CfgWindow.height = applicationWindow().height;
    }
    onVisibleChanged: {
        if (!root.visible)
            CfgWindow.save();

    }

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

    Nvidia {
        id: nvidia

        title: i18n("Nvidia")
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

        }

    }

    SystemTrayIcon {
        id: tray

        visible: CfgWindow.showTrayIcon
        icon.name: "fastgame"
        onActivated: {
            if (!root.visible) {
                root.show();
                root.raise();
                root.requestActivate();
            } else {
                root.hide();
            }
        }

        menu: Menu {
            visible: false

            MenuItem {
                text: i18n("Preset: " + CfgWindow.lastUsedPreset)
                enabled: false
            }

            MenuSeparator {
            }

            MenuItem {
                text: i18n("Quit")
                onTriggered: Qt.quit()
            }

        }

    }

    Component {
        id: preferencesPage

        PreferencesPage {
        }

    }

    Connections {
        function onSettingsApplied() {
            progressBottomDrawer.close();
        }

        target: FGPresetsBackend
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
                enabled: amdgpu.available
                visible: amdgpu.available
                onTriggered: {
                    if (!amdgpu.visible) {
                        while (pageStack.depth > 0)pageStack.pop()
                        pageStack.push(amdgpu);
                    }
                }
            },
            Kirigami.Action {
                text: nvidia.title
                icon.name: "show-gpu-effects-symbolic"
                checked: nvidia.visible
                enabled: nvidia.available
                visible: nvidia.available
                onTriggered: {
                    if (!nvidia.visible) {
                        while (pageStack.depth > 0)pageStack.pop()
                        pageStack.push(nvidia);
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
                            FGPresetsBackend.applySettings();
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
                        onTriggered: {
                            root.pageStack.layers.push(preferencesPage);
                        }
                    },
                    Kirigami.Action {
                        text: i18n("About FastGame")
                        icon.name: "fastgame"
                        displayHint: Kirigami.DisplayHint.AlwaysHide
                        onTriggered: {
                            aboutDialog.open();
                        }
                    },
                    Kirigami.Action {
                        text: i18n("Quit")
                        icon.name: "gtk-quit"
                        displayHint: Kirigami.DisplayHint.AlwaysHide
                        onTriggered: {
                            Qt.quit();
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
                    text: FGPresetsBackend.executableName
                    onTextChanged: {
                        if (text !== FGPresetsBackend.executableName)
                            FGPresetsBackend.executableName = text;

                    }
                    rightActions: [
                        Kirigami.Action {
                            icon.name: "edit-clear"
                            onTriggered: {
                                executableName.text = "";
                                executableName.accepted();
                            }
                        }
                    ]

                    Binding {
                        target: executableName
                        property: "text"
                        value: FGPresetsBackend.executableName
                    }

                }

            }

        }

    }

}
