#include <kaboutdata.h>
#include <klocalizedcontext.h>
#include <qcoreapplication.h>
#include <qqml.h>
#include <qqmlapplicationengine.h>
#include <qquickstyle.h>
#include <qstringliteral.h>
#include <qtenvironmentvariables.h>
#include <qurl.h>
#include <KAboutData>
#include <KLocalizedString>
#include <QGuiApplication>
#include <QtQml>
#include "command_line_arguments.hpp"
#include "config.h"
#include "cpu.hpp"
#include "environment_variables.hpp"

void construct_about_window() {
  KAboutData aboutData(
      QStringLiteral(COMPONENT_NAME), i18nc("@title", APPLICATION_NAME), QStringLiteral(PROJECT_VERSION),
      i18n("Optimize system performance for games"), KAboutLicense::GPL_V3, i18n("(c) 2024"), QStringLiteral(""),
      QStringLiteral("https://github.com/wwmm/fastgame"), QStringLiteral("https://github.com/wwmm/fastgame/issues"));

  aboutData.addAuthor(i18nc("@info:credit", "Wellington Wallace"), i18nc("@info:credit", "Developer"),
                      QStringLiteral("wellingtonwallace@gmail.com"));

  // Set aboutData as information about the app
  KAboutData::setApplicationData(aboutData);

  int VERSION_MINOR = 0;

#ifdef PROJECT_VERSION_MINOR
  VERSION_MINOR = PROJECT_VERSION_MINOR;
#endif

  qmlRegisterSingletonType(
      "AboutFG",  // How the import statement should look like
      PROJECT_VERSION_MAJOR, VERSION_MINOR, "AboutFG",
      [](QQmlEngine* engine, QJSEngine*) -> QJSValue { return engine->toScriptValue(KAboutData::applicationData()); });
}

int main(int argc, char* argv[]) {
  QGuiApplication app(argc, argv);

  KLocalizedString::setApplicationDomain(APPLICATION_DOMAIN);
  QCoreApplication::setOrganizationName(QStringLiteral(ORGANIZATION_NAME));
  QCoreApplication::setOrganizationDomain(QStringLiteral(ORGANIZATION_DOMAIN));
  QCoreApplication::setApplicationName(QStringLiteral("FastGame"));
  QGuiApplication::setApplicationDisplayName(QStringLiteral("FastGame"));

  if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
    QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
  }

  construct_about_window();

  envvars::Model envVarsModel;
  cmdargs::Model cmdArgsModel;
  cpu::Backend cpuBackend;

  QQmlApplicationEngine engine;

  engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
  engine.load(QUrl(QStringLiteral("qrc:/ui/main.qml")));

  if (engine.rootObjects().isEmpty()) {
    return -1;
  }

  return QGuiApplication::exec();
}