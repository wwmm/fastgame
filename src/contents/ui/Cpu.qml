import FGCpuBackend
import FGModelFreqGovernor
import FGModelPcieAspm
import FGModelWorkqueueAffinityScope
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

Kirigami.ScrollablePage {
    id: cpu

    title: i18n("CPU")

    ColumnLayout {
        FormCard.FormHeader {
            title: i18n("Scheduler")
        }

        FormCard.FormCard {
            FormCard.FormComboBoxDelegate {
                id: frequencyGovernor

                text: i18n("Frequency Governor")
                displayMode: FormCard.FormComboBoxDelegate.ComboBox
                currentIndex: FGCpuBackend.frequencyGovernor
                editable: false
                model: FGModelFreqGovernor
                onActivated: (idx) => {
                    if (idx !== FGCpuBackend.frequencyGovernor)
                        FGCpuBackend.frequencyGovernor = idx;

                }
            }

            FormCard.FormComboBoxDelegate {
                id: pcieAspmPolicy

                text: i18n("PCIe ASPM")
                displayMode: FormCard.FormComboBoxDelegate.ComboBox
                currentIndex: FGCpuBackend.pcieAspmPolicy
                editable: false
                model: FGModelPcieAspm
                onActivated: (idx) => {
                    if (idx !== FGCpuBackend.pcieAspmPolicy)
                        FGCpuBackend.pcieAspmPolicy = idx;

                }
            }

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
                id: useCpuDmaLatency

                label: i18n("Use /dev/cpu_dma_latency")
                isChecked: FGCpuBackend.useCpuDmaLatency
                onCheckedChanged: {
                    if (isChecked !== FGCpuBackend.useCpuDmaLatency)
                        FGCpuBackend.useCpuDmaLatency = isChecked;

                }
            }

        }

        FormCard.FormHeader {
            title: i18n("Priority")
        }

        FormCard.FormCard {
            FgSpinBox {
                id: niceness

                label: i18n("Niceness")
                from: -20
                to: 19
                value: FGCpuBackend.niceness
                decimals: 0
                stepSize: 1
                onValueModified: (v) => {
                    FGCpuBackend.niceness = v;
                }
            }

            FgSpinBox {
                id: autogroupNiceness

                label: i18n("Autogroup Niceness")
                from: -20
                to: 19
                value: FGCpuBackend.autogroupNiceness
                decimals: 0
                stepSize: 1
                onValueModified: (v) => {
                    FGCpuBackend.autogroupNiceness = v;
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

        }

        FormCard.FormHeader {
            title: i18n("Affinity")
        }

        FormCard.FormCard {
            FormCard.FormTextFieldDelegate {
                id: gameAffinity

                label: i18n("Game")
                placeholderText: i18n("List of Cores. Example: 2,9,13,15,3")
                onTextChanged: {
                    if (text !== FGCpuBackend.gameAffinity)
                        FGCpuBackend.gameAffinity = text;

                }

                Binding {
                    target: gameAffinity
                    property: "text"
                    value: FGCpuBackend.gameAffinity
                }

                validator: RegularExpressionValidator {
                    regularExpression: /^[1-9]+(,[1-9]+)+$/
                }

            }

            FormCard.FormTextFieldDelegate {
                id: wineServerAffinity

                label: i18n("Wine Server")
                placeholderText: i18n("List of Cores. Example: 2,9,13,15,3")
                onTextChanged: {
                    if (text !== FGCpuBackend.wineServerAffinity)
                        FGCpuBackend.wineServerAffinity = text;

                }

                Binding {
                    target: wineServerAffinity
                    property: "text"
                    value: FGCpuBackend.wineServerAffinity
                }

                validator: RegularExpressionValidator {
                    regularExpression: /^[1-9]+(,[1-9]+)+$/
                }

            }

            FormCard.FormComboBoxDelegate {
                id: workqueueAffinityScope

                text: i18n("Workqueue Scope")
                displayMode: FormCard.FormComboBoxDelegate.ComboBox
                currentIndex: FGCpuBackend.workqueueAffinityScope
                editable: false
                model: FGModelWorkqueueAffinityScope
                onActivated: (idx) => {
                    if (idx !== FGCpuBackend.workqueueAffinityScope)
                        FGCpuBackend.workqueueAffinityScope = idx;

                }
            }

        }

        FormCard.FormHeader {
            title: i18n("General")
        }

        FormCard.FormCard {
            FgSpinBox {
                id: timerSlack

                label: i18n("Timer Slack")
                unit: "ns"
                from: 1
                to: 1e+08
                value: FGCpuBackend.timerSlack
                decimals: 0
                stepSize: 1
                onValueModified: (v) => {
                    FGCpuBackend.timerSlack = v;
                }
            }

            FgSpinBox {
                id: cpuIntensiveThreshold

                label: i18n("CPU Intensive Threshold (Workqueue)")
                unit: "us"
                from: 0
                to: 1e+08
                value: FGCpuBackend.cpuIntensiveThreshold
                decimals: 0
                stepSize: 1
                onValueModified: (v) => {
                    FGCpuBackend.cpuIntensiveThreshold = v;
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

        }

    }

}
