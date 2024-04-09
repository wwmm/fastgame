#include "disk.hpp"
#include <linux/prctl.h>
#include <qobject.h>
#include <qqml.h>
#include <qstring.h>
#include <qtmetamacros.h>
#include <sys/prctl.h>
#include <cstddef>
#include <fstream>
#include <string>
#include "combobox_model.hpp"
#include "config.h"
#include "util.hpp"

namespace disk {

using namespace std::string_literals;

Backend::Backend(QObject* parent) : QObject(parent) {
  qmlRegisterSingletonInstance<Backend>("FGDiskBackend", PROJECT_VERSION_MAJOR, 0, "FGDiskBackend", this);

  qmlRegisterSingletonInstance<ComboBoxModel>("FGModelMountingPath", PROJECT_VERSION_MAJOR, 0, "FGModelMountingPath",
                                              &mountingPathModel);

  qmlRegisterSingletonInstance<ComboBoxModel>("FGModelScheduler", PROJECT_VERSION_MAJOR, 0, "FGModelScheduler",
                                              &schedulerModel);

  std::ifstream infile("/proc/self/mounts");

  std::string psm_dev_path;
  std::string mounting_path;

  while (infile >> psm_dev_path >> mounting_path) {
    if (psm_dev_path.starts_with("/dev/")) {
      mountingPathModel.append(QString::fromStdString(mounting_path));
    }
  }

  connect(this, &Backend::mountingPathChanged, [this]() {
    auto sys_class_path =
        util::mounting_path_to_sys_class_path(mountingPathModel.getValue(_mountingPath).toStdString());

    init_scheduler(sys_class_path);

    if (const auto list = util::read_system_setting(sys_class_path + "/queue/read_ahead_kb"s); !list.empty()) {
      setReadahead(std::stoi(list[0]));
    }

    if (const auto list = util::read_system_setting(sys_class_path + "/queue/add_random"s); !list.empty()) {
      setAddRandom(static_cast<bool>(std::stoi(list[0])));
    } else {
      setAddRandom(false);
    }
  });

  setMountingPath(0);
}

auto Backend::readahead() const -> int {
  return _readahead;
}

void Backend::setReadahead(const int& value) {
  _readahead = value;

  Q_EMIT readaheadChanged();
}

auto Backend::enableRealtimePriority() const -> bool {
  return _enableRealtimePriority;
}

void Backend::setEnableRealtimePriority(const bool& value) {
  _enableRealtimePriority = value;

  Q_EMIT enableRealtimePriorityChanged();
}

auto Backend::addRandom() const -> bool {
  return _addRandom;
}

void Backend::setAddRandom(const bool& value) {
  _addRandom = value;

  Q_EMIT addRandomChanged();
}

auto Backend::useDiskDmaLatency() const -> bool {
  return _useDiskDmaLatency;
}

void Backend::setUseDiskDmaLatency(const bool& value) {
  _useDiskDmaLatency = value;

  Q_EMIT useDiskDmaLatencyChanged();
}

auto Backend::mountingPath() const -> int {
  return _mountingPath;
}

void Backend::setMountingPath(const int& value) {
  if (_mountingPath == value) {
    return;
  }

  _mountingPath = value;

  Q_EMIT mountingPathChanged();
}

auto Backend::scheduler() const -> int {
  return _scheduler;
}

void Backend::setScheduler(const int& value) {
  _scheduler = value;

  Q_EMIT schedulerChanged();
}

auto Backend::timerSlack() const -> int {
  return _timerSlack;
}

void Backend::setTimerSlack(const int& value) {
  _timerSlack = value;

  Q_EMIT timerSlackChanged();
}

auto Backend::gameAffinity() const -> QString {
  return _gameAffinity;
}

void Backend::setGameAffinity(const QString& value) {
  _gameAffinity = value;

  Q_EMIT gameAffinityChanged();
}

auto Backend::wineServerAffinity() const -> QString {
  return _wineServerAffinity;
}

void Backend::setWineServerAffinity(const QString& value) {
  _wineServerAffinity = value;

  Q_EMIT wineServerAffinityChanged();
}

void Backend::init_scheduler(const std::string& sys_class_path) {
  auto scheduler_list = util::read_system_setting(sys_class_path + "/queue/scheduler");

  auto scheduler_value = util::get_selected_value(scheduler_list);

  schedulerModel.reset();

  int selected_id = 0;

  for (size_t n = 0; n < scheduler_list.size(); n++) {
    auto value = scheduler_list[n];

    if (value.find('[') != std::string::npos) {
      value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
    }

    schedulerModel.append(QString::fromStdString(value));

    if (value == scheduler_value) {
      selected_id = n;
    }
  }

  setScheduler(selected_id);
}

}  // namespace disk
