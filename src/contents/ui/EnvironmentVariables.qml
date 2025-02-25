import CppModelEnvVars
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.ScrollablePage {
    id: environmentVariables

    title: i18n("Environment Variables")
    actions: [
        Kirigami.Action {
            id: addAction

            icon.name: "list-add"
            text: i18nc("@action:button", "Add Row")
            onTriggered: CppModelEnvVars.append("", "")
        }
    ]

    Kirigami.CardsListView {
        id: envVarListView

        model: CppModelEnvVars

        delegate: Item {
            id: delegateItem

            width: parent ? parent.width : card.implicitWidth
            height: card.height

            Kirigami.AbstractCard {
                id: card

                width: parent.width

                contentItem: GridLayout {
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
                            placeholderText: i18n("Name")
                            text: name
                            wrapMode: TextInput.WrapAnywhere
                            onTextEdited: model.name = this.text
                        }

                        Kirigami.Separator {
                            Layout.fillWidth: true
                            visible: true
                        }

                        Controls.TextField {
                            Layout.fillWidth: true
                            placeholderText: i18n("Value")
                            text: value
                            wrapMode: TextInput.WrapAnywhere
                            onTextEdited: model.value = this.text
                        }

                    }

                    ColumnLayout {
                        Controls.Button {
                            onClicked: CppModelEnvVars.remove(index)
                            Layout.alignment: Qt.AlignRight
                            icon.name: "delete"
                        }

                    }

                    ColumnLayout {
                        Kirigami.ListItemDragHandle {
                            listItem: card
                            listView: envVarListView
                            onMoveRequested: (oldIndex, newIndex) => {
                                CppModelEnvVars.move(oldIndex, newIndex);
                            }
                        }

                    }

                }

            }

        }

    }

}
