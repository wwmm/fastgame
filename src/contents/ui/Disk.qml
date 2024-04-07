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
                id: niceness

                label: i18n("Niceness")
                from: -20
                to: 19
                value: 0
                decimals: 0
                stepSize: 1
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
                id: useSchedBatch

                text: i18n("Use the Batch Scheduler")
                checked: FGDiskBackend.useSchedBatch

                Binding {
                    target: FGDiskBackend
                    property: "useSchedBatch"
                    value: useSchedBatch.checked
                }

            }

            FormCard.FormSwitchDelegate {
                id: realtimeWineserver

                text: i18n("Realtime Wineserver")
                checked: FGDiskBackend.realtimeWineserver

                Binding {
                    target: FGDiskBackend
                    property: "realtimeWineserver"
                    value: realtimeWineserver.checked
                }

            }

            FormCard.FormSwitchDelegate {
                id: enableWatchdog

                text: i18n("Enable Watchdog")
                checked: FGDiskBackend.enableWatchdog

                Binding {
                    target: FGDiskBackend
                    property: "enableWatchdog"
                    value: enableWatchdog.checked
                }

            }

            FormCard.FormSwitchDelegate {
                id: useDiskDmaLatency

                text: i18n("Use /dev/disk_dma_latency")
                checked: FGDiskBackend.useDiskDmaLatency

                Binding {
                    target: FGDiskBackend
                    property: "useDiskDmaLatency"
                    value: useDiskDmaLatency.checked
                }

            }

        }

        FormCard.FormCard {
            FormCard.FormTextFieldDelegate {
                id: gameAffinity

                label: i18n("Game Affinity")
                placeholderText: i18n("List of Cores. Example: 2,9,13,15,3")
                text: FGDiskBackend.gameAffinity

                validator: RegularExpressionValidator {
                    regularExpression: /^[,0-9]+$/
                }

            }

            FormCard.FormTextFieldDelegate {
                id: wineServerAffinity

                label: i18n("Wine Server Affinity")
                placeholderText: i18n("List of Cores. Example: 2,9,13,15,3")
                text: FGDiskBackend.wineServerAffinity

                validator: RegularExpressionValidator {
                    regularExpression: /^[,0-9]+$/
                }

            }

        }

    }

}
