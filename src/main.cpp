#include <kaboutdata.h>
#include <klocalizedcontext.h>
#include <qlockfile.h>
#include <qobject.h>
#include <qqml.h>
#include <qqmlapplicationengine.h>
#include <qquickstyle.h>
#include <qstandardpaths.h>
#include <qstringliteral.h>
#include <qtenvironmentvariables.h>
#include <qurl.h>
#include <KAboutData>
#include <KLocalizedString>
#include <QApplication>
#include <QtQml>
#include <memory>
#include "config.h"
#include "presets_manager.hpp"
#include "util.hpp"

auto get_lock_file() -> std::unique_ptr<QLockFile> {
  auto lockFile = std::make_unique<QLockFile>(QString::fromStdString(
      QStandardPaths::writableLocation(QStandardPaths::TempLocation).toStdString() + "/fastgame.lock"));

  lockFile->setStaleLockTime(0);

  bool status = lockFile->tryLock(100);

  if (!status) {
    util::critical("Could not lock the file: " + lockFile->fileName().toStdString());

    switch (lockFile->error()) {
      case QLockFile::NoError:
        break;
      case QLockFile::LockFailedError: {
        util::critical("Another instance already has the lock");
        break;
      }
      case QLockFile::PermissionError: {
        util::critical("No permission to reate the lock file");
        break;
      }
      case QLockFile::UnknownError: {
        util::critical("Unknown error");
        break;
      }
    }
  }

  return lockFile;
}

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
  auto lockFile = get_lock_file();

  if (!lockFile->isLocked()) {
    return -1;
  }

  QApplication app(argc, argv);

  KLocalizedString::setApplicationDomain(APPLICATION_DOMAIN);
  QCoreApplication::setOrganizationName(QStringLiteral(ORGANIZATION_NAME));
  QCoreApplication::setOrganizationDomain(QStringLiteral(ORGANIZATION_DOMAIN));
  QCoreApplication::setApplicationName(QStringLiteral("FastGame"));

  if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
    QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
  }

  construct_about_window();

  presets::Backend presetsBackend;

  QQmlApplicationEngine engine;

  engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
  engine.load(QUrl(QStringLiteral("qrc:/ui/main.qml")));

  if (engine.rootObjects().isEmpty()) {
    return -1;
  }

  return QApplication::exec();
}