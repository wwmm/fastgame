import FGDiskBackend
import FGModelMountingPath
import FGModelScheduler
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

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
                onActivated: (idx) => {
                    if (idx !== FGDiskBackend.scheduler)
                        FGDiskBackend.scheduler = idx;

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
            FgSwitch {
                id: enableRealtimePriority

                label: i18n("Realtime Priority")
                checked: FGDiskBackend.enableRealtimePriority
                onCheckedChanged: {
                    if (checked !== FGDiskBackend.enableRealtimePriority)
                        FGDiskBackend.enableRealtimePriority = checked;

                }
            }

            FgSwitch {
                id: addRandom

                label: i18n("Add Random")
                isChecked: FGDiskBackend.addRandom
                onCheckedChanged: {
                    if (isChecked !== FGDiskBackend.addRandom)
                        FGDiskBackend.addRandom = isChecked;

                }
            }

        }

    }

}
