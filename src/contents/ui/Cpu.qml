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

                label: "Niceness"
                from: -20
                to: 19
                value: 0
                decimals: 0
                stepSize: 1
            }

            FgSpinBox {
                id: autogroupNiceness

                label: "Autogroup Niceness"
                from: -20
                to: 19
                value: 0
                decimals: 0
                stepSize: 1
            }

            FgSpinBox {
                id: timerSlack

                label: "Timer Slack"
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
            FormCard.FormSwitchDelegate {
                id: useSchedBatch

                text: i18n("Use the Batch Scheduler")
                checked: FGCpuBackend.useSchedBatch

                Binding {
                    target: FGCpuBackend
                    property: "useSchedBatch"
                    value: useSchedBatch.checked
                }

            }

            FormCard.FormSwitchDelegate {
                id: realtimeWineserver

                text: i18n("Realtime Wineserver")
                checked: FGCpuBackend.realtimeWineserver

                Binding {
                    target: FGCpuBackend
                    property: "realtimeWineserver"
                    value: realtimeWineserver.checked
                }

            }

            FormCard.FormSwitchDelegate {
                id: enableWatchdog

                text: i18n("Enable Watchdog")
                checked: FGCpuBackend.enableWatchdog

                Binding {
                    target: FGCpuBackend
                    property: "enableWatchdog"
                    value: enableWatchdog.checked
                }

            }

            FormCard.FormSwitchDelegate {
                id: useCpuDmaLatency

                text: i18n("Use /dev/cpu_dma_latency")
                checked: FGCpuBackend.useCpuDmaLatency

                Binding {
                    target: FGCpuBackend
                    property: "useCpuDmaLatency"
                    value: useCpuDmaLatency.checked
                }

            }

        }

    }

}
