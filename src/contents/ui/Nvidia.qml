import FGModelPowermizeMode0
import FGNvidiaBackend
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

Kirigami.ScrollablePage {
    id: nvidia

    title: i18n("Nvidia GPU")

    ColumnLayout {
        FormCard.FormHeader {
            title: i18n("Powermize")
        }

        FormCard.FormCard {
            FormCard.FormComboBoxDelegate {
                id: powermizeMode0

                text: i18n("Mode")
                displayMode: FormCard.FormComboBoxDelegate.ComboBox
                currentIndex: FGNvidiaBackend.powermizeMode0
                editable: false
                model: FGModelPowermizeMode0
                onActivated: (idx) => {
                    if (idx !== FGNvidiaBackend.powermizeMode0)
                        FGNvidiaBackend.powermizeMode0 = idx;

                }
            }

            FgSpinBox {
                id: powerLimit

                label: i18n("Power Limit")
                from: 0
                to: 1000
                value: FGNvidiaBackend.powerLimit
                decimals: 0
                stepSize: 1
                unit: "W"
                onValueModified: (v) => {
                    FGNvidiaBackend.powerLimit = v;
                }
            }

        }

        FormCard.FormHeader {
            title: i18n("Clock Offset")
        }

        FormCard.FormCard {
            FgSpinBox {
                id: gpuClockOffset0

                label: i18n("GPU")
                from: FGNvidiaBackend.gpuClockOffset0Min
                to: FGNvidiaBackend.gpuClockOffset0Max
                value: FGNvidiaBackend.gpuClockOffset0
                decimals: 0
                stepSize: 1
                unit: "MHz"
                onValueModified: (v) => {
                    FGNvidiaBackend.gpuClockOffset0 = v;
                }
            }

            FgSpinBox {
                id: memoryClockOffset0

                label: i18n("Memory")
                from: FGNvidiaBackend.memoryClockOffset0Min
                to: FGNvidiaBackend.memoryClockOffset0Max
                value: FGNvidiaBackend.memoryClockOffset0
                decimals: 0
                stepSize: 1
                unit: "MHz"
                onValueModified: (v) => {
                    FGNvidiaBackend.memoryClockOffset0 = v;
                }
            }

        }

    }

}
