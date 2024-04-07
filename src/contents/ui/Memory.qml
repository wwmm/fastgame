import FGMemoryBackend
import FGModelThpDefrag
import FGModelThpEnabled
import FGModelThpShmemEnabled
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

Kirigami.ScrollablePage {
    id: cpu

    title: i18n("Memory")

    ColumnLayout {
        FormCard.FormHeader {
            title: i18n("Transparent HugePages")
        }

        FormCard.FormCard {
            FormCard.FormComboBoxDelegate {
                id: thpEnabled

                text: i18n("Enabled")
                displayMode: FormCard.FormComboBoxDelegate.ComboBox
                currentIndex: FGMemoryBackend.thpEnabled
                editable: false
                model: FGModelThpEnabled

                Binding {
                    target: FGMemoryBackend
                    property: "thpEnabled"
                    value: thpEnabled.currentIndex
                }

            }

            FormCard.FormComboBoxDelegate {
                id: thpDefrag

                text: i18n("Defrag")
                displayMode: FormCard.FormComboBoxDelegate.ComboBox
                currentIndex: 0
                editable: false
                model: FGModelThpDefrag

                Binding {
                    target: FGMemoryBackend
                    property: "thpDefrag"
                    value: thpDefrag.currentIndex
                }

            }

            FormCard.FormComboBoxDelegate {
                id: thpShmemEnabled

                text: i18n("Shared Memory Enabled")
                displayMode: FormCard.FormComboBoxDelegate.ComboBox
                currentIndex: 0
                editable: false
                model: FGModelThpShmemEnabled

                Binding {
                    target: FGMemoryBackend
                    property: "thpShmemEnabled"
                    value: thpShmemEnabled.currentIndex
                }

            }

            FgSpinBox {
                id: scanSleep

                label: i18n("Scan Sleep")
                from: 0
                to: 3.6e+06
                value: FGMemoryBackend.scanSleep
                decimals: 0
                stepSize: 1
                unit: "ms"

                Binding {
                    target: FGMemoryBackend
                    property: "scanSleep"
                    value: scanSleep.value
                }

            }

            FgSpinBox {
                id: allocSleep

                label: i18n("Allocation Sleep")
                from: 0
                to: 3.6e+06
                value: FGMemoryBackend.allocSleep
                decimals: 0
                stepSize: 1
                unit: "ms"

                Binding {
                    target: FGMemoryBackend
                    property: "allocSleep"
                    value: allocSleep.value
                }

            }

        }

        FormCard.FormHeader {
            title: i18n("Multi-Gen LRU")
        }

        FormCard.FormCard {
            FgSpinBox {
                id: mglruMinTtlMs

                label: i18n("min_ttl_ms")
                unit: "ms"
                from: 0
                to: 60000
                value: FGMemoryBackend.mglruMinTtlMs
                decimals: 0
                stepSize: 1

                Binding {
                    target: FGMemoryBackend
                    property: "mglruMinTtlMs"
                    value: mglruMinTtlMs.value
                }

            }

        }

        FormCard.FormHeader {
            title: i18n("Virtual")
        }

        FormCard.FormCard {
            FgSpinBox {
                id: swappiness

                label: i18n("Swappiness")
                from: 0
                to: 200
                value: FGMemoryBackend.swappiness
                decimals: 0
                stepSize: 1

                Binding {
                    target: FGMemoryBackend
                    property: "swappiness"
                    value: swappiness.value
                }

            }

            FgSpinBox {
                id: cachePressure

                label: i18n("Cache Pressure")
                from: 0
                to: 200
                value: FGMemoryBackend.cachePressure
                decimals: 0
                stepSize: 1

                Binding {
                    target: FGMemoryBackend
                    property: "cachePressure"
                    value: cachePressure.value
                }

            }

            FgSpinBox {
                id: compactionProactiveness

                label: i18n("Compaction Proactiveness")
                from: 0
                to: 200
                value: FGMemoryBackend.compactionProactiveness
                decimals: 0
                stepSize: 1

                Binding {
                    target: FGMemoryBackend
                    property: "compactionProactiveness"
                    value: compactionProactiveness.value
                }

            }

            FgSpinBox {
                id: minFreeKbytes

                label: i18n("Minimum Free Kbytes")
                from: 1024
                to: 1e+08
                value: FGMemoryBackend.minFreeKbytes
                decimals: 0
                stepSize: 1
                unit: "KB"

                Binding {
                    target: FGMemoryBackend
                    property: "minFreeKbytes"
                    value: minFreeKbytes.value
                }

            }

            FgSpinBox {
                id: pageLockUnfairness

                label: i18n("Page Lock Unfairness")
                from: 0
                to: 10000
                value: FGMemoryBackend.pageLockUnfairness
                decimals: 0
                stepSize: 1

                Binding {
                    target: FGMemoryBackend
                    property: "pageLockUnfairness"
                    value: pageLockUnfairness.value
                }

            }

            FgSpinBox {
                id: perCpuPagelistHighFraction

                label: i18n("Per-CPU Page List Fraction")
                from: 0
                to: 1000
                value: FGMemoryBackend.perCpuPagelistHighFraction
                decimals: 0
                stepSize: 1

                Binding {
                    target: FGMemoryBackend
                    property: "perCpuPagelistHighFraction"
                    value: perCpuPagelistHighFraction.value
                }

            }

        }

    }

}
