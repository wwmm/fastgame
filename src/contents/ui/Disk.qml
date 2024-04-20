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
                onActivated: (idx) => {
                    if (idx !== FGDiskBackend.mountingPath)
                        FGDiskBackend.mountingPath = idx;

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
                onValueModified: (v) => {
                    FGDiskBackend.readahead = v;
                }
            }

            FgSpinBox {
                id: nrRequests

                label: i18n("Maximum Read/Write requests")
                from: 32
                to: 4096
                value: FGDiskBackend.nrRequests
                decimals: 0
                stepSize: 1
                onValueModified: (v) => {
                    FGDiskBackend.nrRequests = v;
                }
            }

            FgSpinBox {
                id: rqAffinity

                label: i18n("RQ Affinity")
                from: 1
                to: 2
                value: FGDiskBackend.rqAffinity
                decimals: 0
                stepSize: 1
                onValueModified: (v) => {
                    FGDiskBackend.rqAffinity = v;
                }
            }

            FgSpinBox {
                id: noMerges

                label: i18n("No Merges")
                from: 0
                to: 2
                value: FGDiskBackend.noMerges
                decimals: 0
                stepSize: 1
                onValueModified: (v) => {
                    FGDiskBackend.noMerges = v;
                }
            }

            FgSpinBox {
                id: wbtLatUsec

                label: i18n("Writeback Throttling Latency")
                from: -1
                to: 1e+06
                value: FGDiskBackend.wbtLatUsec
                decimals: 0
                stepSize: 1
                onValueModified: (v) => {
                    FGDiskBackend.wbtLatUsec = v;
                }
            }

        }

        FormCard.FormCard {
            FgSwitch {
                id: enableRealtimePriority

                label: i18n("Realtime Priority")
                isChecked: FGDiskBackend.enableRealtimePriority
                onCheckedChanged: {
                    if (isChecked !== FGDiskBackend.enableRealtimePriority)
                        FGDiskBackend.enableRealtimePriority = isChecked;

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
