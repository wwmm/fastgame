import FGMemoryBackend
import FGModelThpDefrag
import FGModelThpEnabled
import FGModelThpShmemEnabled
import FGModelZoneReclaimMode
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

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
                onActivated: (idx) => {
                    if (idx !== FGMemoryBackend.thpEnabled)
                        FGMemoryBackend.thpEnabled = idx;

                }
            }

            FormCard.FormComboBoxDelegate {
                id: thpDefrag

                text: i18n("Defrag")
                displayMode: FormCard.FormComboBoxDelegate.ComboBox
                currentIndex: FGMemoryBackend.thpDefrag
                editable: false
                model: FGModelThpDefrag
                onActivated: (idx) => {
                    if (idx !== FGMemoryBackend.thpDefrag)
                        FGMemoryBackend.thpDefrag = idx;

                }
            }

            FormCard.FormComboBoxDelegate {
                id: thpShmemEnabled

                text: i18n("Shared Memory Enabled")
                displayMode: FormCard.FormComboBoxDelegate.ComboBox
                currentIndex: FGMemoryBackend.thpShmemEnabled
                editable: false
                model: FGModelThpShmemEnabled
                onActivated: (idx) => {
                    if (idx !== FGMemoryBackend.thpShmemEnabled)
                        FGMemoryBackend.thpShmemEnabled = idx;

                }
            }

            FgSwitch {
                id: optimizeHugeTLB

                label: i18n("Optimize HugeTLB Vmemmap")
                isChecked: FGMemoryBackend.optimizeHugeTLB
                onCheckedChanged: {
                    if (isChecked !== FGMemoryBackend.optimizeHugeTLB)
                        FGMemoryBackend.optimizeHugeTLB = isChecked;

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
                onValueModified: (v) => {
                    FGMemoryBackend.scanSleep = v;
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
                onValueModified: (v) => {
                    FGMemoryBackend.allocSleep = v;
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
                onValueModified: (v) => {
                    FGMemoryBackend.mglruMinTtlMs = v;
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
                onValueModified: (v) => {
                    FGMemoryBackend.swappiness = v;
                }
            }

            FgSpinBox {
                id: pageCluster

                label: i18n("Page-cluster")
                from: 0
                to: 100
                value: FGMemoryBackend.pageCluster
                decimals: 0
                stepSize: 1
                onValueModified: (v) => {
                    FGMemoryBackend.pageCluster = v;
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
                onValueModified: (v) => {
                    FGMemoryBackend.cachePressure = v;
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
                onValueModified: (v) => {
                    FGMemoryBackend.compactionProactiveness = v;
                }
            }

            FgSpinBox {
                id: extfragThreshold

                label: i18n("External Fragmentation Threshold")
                from: 0
                to: 1000
                value: FGMemoryBackend.extfragThreshold
                decimals: 0
                stepSize: 1
                onValueModified: (v) => {
                    FGMemoryBackend.extfragThreshold = v;
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
                onValueModified: (v) => {
                    FGMemoryBackend.minFreeKbytes = v;
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
                onValueModified: (v) => {
                    FGMemoryBackend.pageLockUnfairness = v;
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
                onValueModified: (v) => {
                    FGMemoryBackend.perCpuPagelistHighFraction = v;
                }
            }

            FormCard.FormComboBoxDelegate {
                id: zoneReclaimMode

                text: i18n("Zone Reclaim Mode")
                displayMode: FormCard.FormComboBoxDelegate.ComboBox
                currentIndex: FGMemoryBackend.zoneReclaimMode
                editable: false
                model: FGModelZoneReclaimMode
                onActivated: (idx) => {
                    if (idx !== FGMemoryBackend.zoneReclaimMode)
                        FGMemoryBackend.zoneReclaimMode = idx;

                }
            }

        }

    }

}
