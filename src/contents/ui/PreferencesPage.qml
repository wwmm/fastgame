import CfgWindow
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

FormCard.FormCardPage {
    FormCard.FormHeader {
        title: i18n("General")
    }

    FormCard.FormCard {
        FgSwitch {
            id: showTrayIcon

            label: i18n("Show Tray Icon")
            isChecked: CfgWindow.show_tray_icon
            onCheckedChanged: {
                if (isChecked !== CfgWindow.show_tray_icon)
                    CfgWindow.show_tray_icon = isChecked;

            }
        }

    }

}
