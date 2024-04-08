import FGDiskBackend
import FGModelMountingPath
import FGModelScheduler
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

Kirigami.ScrollablePage {
    id: disk

    title: i18n("Disk")

    ColumnLayout {
        FormCard.FormCard {
            FormCard.FormComboBoxDelegate {
                id: mountingPath

                text: i18n("Mounting Path")
                displayMode: FormCard.FormComboBoxDelegate.ComboBox
                currentIndex: FGDiskBackend.mountingPath
                editable: false
                model: FGModelMountingPath

                Binding {
                    target: FGDiskBackend
                    property: "mountingPath"
                    value: mountingPath.currentIndex
                }

            }

            FormCard.FormComboBoxDelegate {
                id: scheduler

                text: i18n("Scheduler")
                displayMode: FormCard.FormComboBoxDelegate.ComboBox
                currentIndex: FGDiskBackend.scheduler
                editable: false
                model: FGModelScheduler

                Binding {
                    target: FGDiskBackend
                    property: "scheduler"
                    value: scheduler.currentIndex
                }

            }

            FgSpinBox {
                id: readahead

                label: i18n("Readahead")
                from: 0
                to: 1e+06
                value: FGDiskBackend.readahead
                decimals: 0
                stepSize: 1
                unit: "KB"

                Binding {
                    target: FGDiskBackend
                    property: "readahead"
                    value: readahead.value
                }

            }

            FgSpinBox {
                id: autogroupNiceness

                label: i18n("Autogroup Niceness")
                from: -20
                to: 19
                value: 0
                decimals: 0
                stepSize: 1
            }

            FgSpinBox {
                id: timerSlack

                label: i18n("Timer Slack")
                unit: "ns"
                from: 1
                to: 1e+08
                value: FGDiskBackend.timerSlack
                decimals: 0
                stepSize: 1

                Binding {
                    target: FGDiskBackend
                    property: "timerSlack"
                    value: timerSlack.value
                }

            }

        }

        FormCard.FormCard {
            FormCard.FormSwitchDelegate {
                id: enableRealtimePriority

                text: i18n("Realtime Priority")
                checked: FGDiskBackend.enableRealtimePriority

                Binding {
                    target: FGDiskBackend
                    property: "enableRealtimePriority"
                    value: enableRealtimePriority.checked
                }

            }

            FormCard.FormSwitchDelegate {
                id: addRandom

                text: i18n("Add Random")
                checked: FGDiskBackend.addRandom

                Binding {
                    target: FGDiskBackend
                    property: "addRandom"
                    value: addRandom.checked
                }

            }

            Controls.Switch {
                checked: FGDiskBackend.addRandom

                Binding {
                    target: FGDiskBackend
                    property: "addRandom"
                    value: addRandom.checked
                }

            }

        }

    }

}
