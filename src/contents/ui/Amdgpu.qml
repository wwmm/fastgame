import FGAmdgpuBackend
import FGModelPerformanceLevel0
import FGModelPerformanceLevel1
import FGModelPowerProfile0
import FGModelPowerProfile1
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

Kirigami.ScrollablePage {
    id: amdgpu

    title: i18n("AMD GPU")

    ColumnLayout {
        FormCard.FormHeader {
            title: i18n("First Card")
        }

        FormCard.FormCard {
            FormCard.FormComboBoxDelegate {
                id: performanceLevel0

                text: i18n("Performance Level")
                displayMode: FormCard.FormComboBoxDelegate.ComboBox
                currentIndex: FGAmdgpuBackend.performanceLevel0
                editable: false
                model: FGModelPerformanceLevel0
                onActivated: (idx) => {
                    if (idx !== FGAmdgpuBackend.performanceLevel0)
                        FGAmdgpuBackend.performanceLevel0 = idx;

                }
            }

            FormCard.FormComboBoxDelegate {
                id: powerProfile0

                text: i18n("Power Profile")
                displayMode: FormCard.FormComboBoxDelegate.ComboBox
                currentIndex: FGAmdgpuBackend.powerProfile0
                editable: false
                model: FGModelPowerProfile0
                enabled: FGModelPowerProfile0.rowCount() > 0 && performanceLevel0.currentIndex === 3 // manual
                onActivated: (idx) => {
                    if (idx !== FGAmdgpuBackend.powerProfile0)
                        FGAmdgpuBackend.powerProfile0 = idx;

                }
            }

            FgSpinBox {
                id: powerCap0

                label: i18n("Power Cap")
                from: 0
                to: FGAmdgpuBackend.maxPowerCap0
                value: FGAmdgpuBackend.powerCap0
                decimals: 0
                stepSize: 1
                unit: "W"
                enabled: (FGAmdgpuBackend.maxPowerCap0 == -1) ? false : true
                onValueModified: (v) => {
                    FGAmdgpuBackend.powerCap0 = v;
                }
            }

        }

        FormCard.FormHeader {
            title: i18n("Second Card")
        }

        FormCard.FormCard {
            FormCard.FormComboBoxDelegate {
                id: performanceLevel1

                text: i18n("Performance Level")
                displayMode: FormCard.FormComboBoxDelegate.ComboBox
                currentIndex: FGAmdgpuBackend.performanceLevel1
                editable: false
                model: FGModelPerformanceLevel1
                onActivated: (idx) => {
                    if (idx !== FGAmdgpuBackend.performanceLevel1)
                        FGAmdgpuBackend.performanceLevel1 = idx;

                }
            }

            FormCard.FormComboBoxDelegate {
                id: powerProfile1

                text: i18n("Power Profile")
                displayMode: FormCard.FormComboBoxDelegate.ComboBox
                currentIndex: FGAmdgpuBackend.powerProfile1
                editable: false
                model: FGModelPowerProfile1
                enabled: FGModelPowerProfile1.rowCount() > 0 && performanceLevel1.currentIndex === 3 // manual
                onActivated: (idx) => {
                    if (idx !== FGAmdgpuBackend.powerProfile1)
                        FGAmdgpuBackend.powerProfile1 = idx;

                }
            }

            FgSpinBox {
                id: powerCap1

                label: i18n("Power Cap")
                from: 0
                to: FGAmdgpuBackend.maxPowerCap1
                value: FGAmdgpuBackend.powerCap1
                decimals: 0
                stepSize: 1
                unit: "W"
                enabled: (FGAmdgpuBackend.maxPowerCap1 == -1) ? false : true
                onValueModified: (v) => {
                    FGAmdgpuBackend.powerCap1 = v;
                }
            }

        }

    }

}
