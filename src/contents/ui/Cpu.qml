import FGCpuBackend
import FGModelFreqGovernor
import FGModelPcieAspm
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

Kirigami.ScrollablePage {
    id: cpu

    title: i18n("CPU")

    ColumnLayout {
        FormCard.FormCard {
            FormCard.FormComboBoxDelegate {
                id: frequencyGovernor

                text: i18n("Frequency Governor")
                displayMode: FormCard.FormComboBoxDelegate.ComboBox
                currentIndex: FGCpuBackend.frequencyGovernor
                editable: false
                model: FGModelFreqGovernor

                Binding {
                    target: FGCpuBackend
                    property: "frequencyGovernor"
                    value: frequencyGovernor.currentIndex
                }

            }

            FormCard.FormComboBoxDelegate {
                id: pcieAspmPolicy

                text: i18n("PCIe ASPM")
                displayMode: FormCard.FormComboBoxDelegate.ComboBox
                currentIndex: 0
                editable: false
                model: FGModelPcieAspm

                Binding {
                    target: FGCpuBackend
                    property: "pcieAspmPolicy"
                    value: pcieAspmPolicy.currentIndex
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
                value: FGCpuBackend.timerSlack
                decimals: 0
                stepSize: 1

                Binding {
                    target: FGCpuBackend
                    property: "timerSlack"
                    value: timerSlack.value
                }

            }

        }

        FormCard.FormCard {
            FgSwitch {
                id: useSchedBatch

                label: i18n("Use the Batch Scheduler")
                isChecked: FGCpuBackend.useSchedBatch
                onCheckedChanged: {
                    if (isChecked !== FGCpuBackend.useSchedBatch)
                        FGCpuBackend.useSchedBatch = isChecked;

                }
            }

            FgSwitch {
                id: realtimeWineserver

                label: i18n("Realtime Wineserver")
                isChecked: FGCpuBackend.realtimeWineserver
                onCheckedChanged: {
                    if (isChecked !== FGCpuBackend.realtimeWineserver)
                        FGCpuBackend.realtimeWineserver = isChecked;

                }
            }

            FgSwitch {
                id: enableWatchdog

                label: i18n("Enable Watchdog")
                isChecked: FGCpuBackend.enableWatchdog
                onCheckedChanged: {
                    if (isChecked !== FGCpuBackend.enableWatchdog)
                        FGCpuBackend.enableWatchdog = isChecked;

                }
            }

            FgSwitch {
                id: useCpuDmaLatency

                label: i18n("Use /dev/cpu_dma_latency")
                isChecked: FGCpuBackend.useCpuDmaLatency
                onCheckedChanged: {
                    if (isChecked !== FGCpuBackend.useCpuDmaLatency)
                        FGCpuBackend.useCpuDmaLatency = isChecked;

                }
            }

        }

        FormCard.FormCard {
            FormCard.FormTextFieldDelegate {
                id: gameAffinity

                label: i18n("Game Affinity")
                placeholderText: i18n("List of Cores. Example: 2,9,13,15,3")
                text: FGCpuBackend.gameAffinity

                validator: RegularExpressionValidator {
                    regularExpression: /^[,0-9]+$/
                }

            }

            FormCard.FormTextFieldDelegate {
                id: wineServerAffinity

                label: i18n("Wine Server Affinity")
                placeholderText: i18n("List of Cores. Example: 2,9,13,15,3")
                text: FGCpuBackend.wineServerAffinity

                validator: RegularExpressionValidator {
                    regularExpression: /^[,0-9]+$/
                }

            }

        }

    }

}
