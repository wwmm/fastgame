import CppModelScxArgs
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.ScrollablePage {
    id: commandLineArguments

    title: i18n("Scx Schedulers")
    actions: [
        Kirigami.Action {
            id: addAction

            icon.name: "list-add"
            text: i18nc("@action:button", "Add Row")
            onTriggered: CppModelScxArgs.append("")
        }
    ]

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

    Kirigami.CardsListView {
        id: cmdArgListView

        model: CppModelScxArgs
        delegate: listDelegate
    }
}
