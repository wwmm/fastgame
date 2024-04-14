#include "nvidia.hpp"
#include <linux/prctl.h>
#include <qobject.h>
#include <qqml.h>
#include <qstring.h>
#include <qtmetamacros.h>
#include <sys/prctl.h>
#include "config.h"

namespace nvidia {

Backend::Backend(QObject* parent) : QObject(parent) {
  qmlRegisterSingletonInstance<Backend>("FGNvidiaBackend", PROJECT_VERSION_MAJOR, 0, "FGNvidiaBackend", this);

  //   qmlRegisterSingletonInstance<ComboBoxModel>("FGModelMountingPath", PROJECT_VERSION_MAJOR, 0,
  //   "FGModelMountingPath",
  //                                               &mountingPathModel);

  //   qmlRegisterSingletonInstance<ComboBoxModel>("FGModelScheduler", PROJECT_VERSION_MAJOR, 0, "FGModelScheduler",
  //                                               &schedulerModel);

  connect(this, &Backend::mountingPathChanged, []() {
    // auto sys_class_path =
    //     util::mounting_path_to_sys_class_path(mountingPathModel.getValue(_mountingPath).toStdString());

    // if (const auto list = util::read_system_setting(sys_class_path + "/queue/read_ahead_kb"); !list.empty()) {
    //   setReadahead(std::stoi(list[0]));
    // }

    // if (const auto list = util::read_system_setting(sys_class_path + "/queue/add_random"); !list.empty()) {
    //   setAddRandom(static_cast<bool>(std::stoi(list[0])));
    // } else {
    //   setAddRandom(false);
    // }

    // if (const auto list = util::read_system_setting(sys_class_path + "/queue/nr_requests"); !list.empty()) {
    //   setNrRequests(std::stoi(list[0]));
    // }

    // if (const auto list = util::read_system_setting(sys_class_path + "/queue/rq_affinity"); !list.empty()) {
    //   setRqAffinity(std::stoi(list[0]));
    // }

    // if (const auto list = util::read_system_setting(sys_class_path + "/queue/nomerges"); !list.empty()) {
    //   setNoMerges(std::stoi(list[0]));
    // }

    // if (const auto list = util::read_system_setting(sys_class_path + "/queue/wbt_lat_usec"); !list.empty()) {
    //   setWbtLatUsec(std::stoi(list[0]));
    // }
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

auto Backend::rqAffinity() const -> int {
  return _rqAffinity;
}

void Backend::setRqAffinity(const int& value) {
  _rqAffinity = value;

  Q_EMIT rqAffinityChanged();
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

auto Backend::nrRequests() const -> int {
  return _nrRequests;
}

void Backend::setNrRequests(const int& value) {
  _nrRequests = value;

  Q_EMIT nrRequestsChanged();
}

auto Backend::noMerges() const -> int {
  return _noMerges;
}

void Backend::setNoMerges(const int& value) {
  _noMerges = value;

  Q_EMIT noMergesChanged();
}

auto Backend::wbtLatUsec() const -> int {
  return _wbtLatUsec;
}

void Backend::setWbtLatUsec(const int& value) {
  _wbtLatUsec = value;

  Q_EMIT wbtLatUsecChanged();
}

}  // namespace nvidia
