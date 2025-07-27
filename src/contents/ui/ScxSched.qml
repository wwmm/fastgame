import ScxSchedBackend
import CppModelScxArgs
import FGModelScxSched
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

Kirigami.ScrollablePage {
    id: scxSched

    title: i18n("Scx Schedulers")

    Component {
        id: listDelegate

        Kirigami.AbstractCard {

            contentItem: Item {
                implicitWidth: delegateLayout.implicitWidth
                implicitHeight: delegateLayout.implicitHeight

                GridLayout {
                    id: delegateLayout

                    rowSpacing: Kirigami.Units.largeSpacing
                    columnSpacing: Kirigami.Units.largeSpacing
                    columns: root.wideScreen ? 4 : 2

                    anchors {
                        left: parent.left
                        top: parent.top
                        right: parent.right
                    }

                    ColumnLayout {
                        Controls.TextField {
                            Layout.fillWidth: true
                            placeholderText: i18n("Argument")
                            text: value
                            wrapMode: TextInput.WrapAnywhere
                            onTextEdited: model.value = this.text
                        }
                    }

                    ColumnLayout {
                        Controls.Button {
                            onClicked: CppModelScxArgs.remove(index)
                            Layout.alignment: Qt.AlignRight
                            icon.name: "delete"
                        }
                    }
                }
            }
        }
    }

    ColumnLayout {

        anchors.fill: parent

        FormCard.FormCard {
            FgSwitch {
                id: enable

                label: i18n("Enable")
                isChecked: ScxSchedBackend.enable
                onCheckedChanged: {
                    if (isChecked !== ScxSchedBackend.enable)
                        ScxSchedBackend.enable = isChecked;
                }
            }

            FormCard.FormComboBoxDelegate {
                id: scheduler

                text: i18n("Scheduler")
                displayMode: FormCard.FormComboBoxDelegate.ComboBox
                currentIndex: ScxSchedBackend.scheduler
                editable: false
                model: FGModelScxSched
                enabled: enable.isChecked
                onActivated: idx => {
                    if (idx !== ScxSchedBackend.scheduler)
                        ScxSchedBackend.scheduler = idx;
                }
            }
        }

        Kirigami.CardsListView {
            id: cmdArgListView

            Layout.fillHeight: true
            Layout.fillWidth: true

            model: CppModelScxArgs
            delegate: listDelegate
        }
    }

    footer: Kirigami.ActionToolBar {
        Layout.margins: Kirigami.Units.smallSpacing
        alignment: Qt.AlignRight
        position: Controls.ToolBar.Footer
        flat: true
        actions: [
            Kirigami.Action {
                id: addAction

                icon.name: "list-add"
                text: i18n("Command Line Option")
                onTriggered: CppModelScxArgs.append("")
            }
        ]
    }
}
