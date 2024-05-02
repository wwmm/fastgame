import CfgWindow
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

FormCard.FormCardPage {
    FormCard.FormHeader {
        title: i18n("General")
    }

    FormCard.FormCard {
        FgSwitch {
            id: showTrayIcon

            label: i18n("Show the Tray Icon")
            isChecked: CfgWindow.showTrayIcon
            onCheckedChanged: {
                if (isChecked !== CfgWindow.showTrayIcon)
                    CfgWindow.showTrayIcon = isChecked;

            }
        }

    }

}
