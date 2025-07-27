#include "scx_sched.hpp"
#include <qabstractitemmodel.h>
#include <qbytearray.h>
#include <qhash.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqml.h>
#include <qtmetamacros.h>
#include <qvariant.h>
#include <string>
#include "combobox_model.hpp"
#include "config.h"

namespace scx_sched {

Backend::Backend(QObject* parent) : QObject(parent) {
  qmlRegisterSingletonInstance<Backend>("ScxSchedBackend", VERSION_MAJOR, VERSION_MINOR, "ScxSchedBackend", this);

  qmlRegisterSingletonInstance<ComboBoxModel>("FGModelScxSched", VERSION_MAJOR, VERSION_MINOR, "FGModelScxSched",
                                              &schedulerModel);
}

auto Backend::enable() const -> bool {
  return _enable;
}

void Backend::setEnable(const bool& value) {
  _enable = value;

  Q_EMIT enableChanged();
}

auto Backend::scheduler() -> std::string {
  return schedulerModel.getValue(_scheduler).toStdString();
}

void Backend::setScheduler(const std::string& value) {
  auto id = schedulerModel.getId(QString::fromStdString(value));

  if (id == -1) {
    return;
  }
  _scheduler = id;

  Q_EMIT schedulerChanged();
}

void Backend::init_scheduler() {}

}  // namespace scx_sched