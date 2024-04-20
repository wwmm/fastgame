#include "memory.hpp"
#include <linux/prctl.h>
#include <qobject.h>
#include <qqml.h>
#include <qstring.h>
#include <qtmetamacros.h>
#include <sys/prctl.h>
#include <cstddef>
#include <string>
#include "combobox_model.hpp"
#include "config.h"
#include "util.hpp"

namespace memory {

using namespace std::string_literals;

Backend::Backend(QObject* parent) : QObject(parent) {
  qmlRegisterSingletonInstance<Backend>("FGMemoryBackend", PROJECT_VERSION_MAJOR, 0, "FGMemoryBackend", this);

  qmlRegisterSingletonInstance<ComboBoxModel>("FGModelThpEnabled", PROJECT_VERSION_MAJOR, 0, "FGModelThpEnabled",
                                              &thpEnabledModel);

  qmlRegisterSingletonInstance<ComboBoxModel>("FGModelThpDefrag", PROJECT_VERSION_MAJOR, 0, "FGModelThpDefrag",
                                              &thpDefragModel);

  qmlRegisterSingletonInstance<ComboBoxModel>("FGModelThpShmemEnabled", PROJECT_VERSION_MAJOR, 0,
                                              "FGModelThpShmemEnabled", &thpShmemEnabledModel);

  if (const auto list = util::read_system_setting("/sys/kernel/mm/lru_gen/min_ttl_ms"); !list.empty()) {
    setMglruMinTtlMs(std::stoi(list[0]));
  }

  if (const auto list = util::read_system_setting("/proc/sys/vm/swappiness"); !list.empty()) {
    setSwappiness(std::stoi(list[0]));
  }

  if (const auto list = util::read_system_setting("/proc/sys/vm/vfs_cache_pressure"); !list.empty()) {
    setCachePressure(std::stoi(list[0]));
  }

  if (const auto list = util::read_system_setting("/proc/sys/vm/compaction_proactiveness"); !list.empty()) {
    setCompactionProactiveness(std::stoi(list[0]));
  }

  if (const auto list = util::read_system_setting("/proc/sys/vm/min_free_kbytes"); !list.empty()) {
    setMinFreeKbytes(std::stoi(list[0]));
  }

  if (const auto list = util::read_system_setting("/proc/sys/vm/page_lock_unfairness"); !list.empty()) {
    setPageLockUnfairness(std::stoi(list[0]));
  }

  if (const auto list = util::read_system_setting("/proc/sys/vm/percpu_pagelist_high_fraction"); !list.empty()) {
    setPerCpuPagelistHighFraction(std::stoi(list[0]));
  }

  initThp();
}

auto Backend::swappiness() const -> int {
  return _swappiness;
}

void Backend::setSwappiness(const int& value) {
  _swappiness = value;

  Q_EMIT swappinessChanged();
}

auto Backend::thpEnabled() -> std::string {
  return thpEnabledModel.getValue(_thpEnabled).toStdString();
}

void Backend::setThpEnabled(const std::string& value) {
  auto id = thpEnabledModel.getId(QString::fromStdString(value));

  if (id == -1) {
    return;
  }

  _thpEnabled = id;

  Q_EMIT thpEnabledChanged();
}

auto Backend::thpDefrag() -> std::string {
  return thpDefragModel.getValue(_thpDefrag).toStdString();
}

void Backend::setThpDefrag(const std::string& value) {
  auto id = thpDefragModel.getId(QString::fromStdString(value));

  if (id == -1) {
    return;
  }

  _thpDefrag = id;

  Q_EMIT thpDefragChanged();
}

auto Backend::thpShmemEnabled() -> std::string {
  return thpShmemEnabledModel.getValue(_thpShmemEnabled).toStdString();
}

void Backend::setThpShmemEnabled(const std::string& value) {
  auto id = thpShmemEnabledModel.getId(QString::fromStdString(value));

  if (id == -1) {
    return;
  }

  _thpShmemEnabled = id;

  Q_EMIT thpShmemEnabledChanged();
}

auto Backend::scanSleep() const -> int {
  return _scanSleep;
}

void Backend::setScanSleep(const int& value) {
  _scanSleep = value;

  Q_EMIT scanSleepChanged();
}

auto Backend::allocSleep() const -> int {
  return _allocSleep;
}

void Backend::setAllocSleep(const int& value) {
  _allocSleep = value;

  Q_EMIT allocSleepChanged();
}

auto Backend::mglruMinTtlMs() const -> int {
  return _mglruMinTtlMs;
}

void Backend::setMglruMinTtlMs(const int& value) {
  _mglruMinTtlMs = value;

  Q_EMIT mglruMinTtlMsChanged();
}

auto Backend::cachePressure() const -> int {
  return _cachePressure;
}

void Backend::setCachePressure(const int& value) {
  _cachePressure = value;

  Q_EMIT cachePressureChanged();
}

auto Backend::compactionProactiveness() const -> int {
  return _compactionProactiveness;
}

void Backend::setCompactionProactiveness(const int& value) {
  _compactionProactiveness = value;

  Q_EMIT compactionProactivenessChanged();
}

auto Backend::minFreeKbytes() const -> int {
  return _minFreeKbytes;
}

void Backend::setMinFreeKbytes(const int& value) {
  _minFreeKbytes = value;

  Q_EMIT minFreeKbytesChanged();
}

auto Backend::pageLockUnfairness() const -> int {
  return _pageLockUnfairness;
}

void Backend::setPageLockUnfairness(const int& value) {
  _pageLockUnfairness = value;

  Q_EMIT pageLockUnfairnessChanged();
}

auto Backend::perCpuPagelistHighFraction() const -> int {
  return _perCpuPagelistHighFraction;
}

void Backend::setPerCpuPagelistHighFraction(const int& value) {
  _perCpuPagelistHighFraction = value;

  Q_EMIT perCpuPagelistHighFractionChanged();
}

void Backend::initThp() {
  // parameter: enabled

  {
    auto enabled_list = util::read_system_setting("/sys/kernel/mm/transparent_hugepage/enabled");

    auto enabled_value = util::get_selected_value(enabled_list);

    util::debug("transparent huge pages state: " + enabled_value);

    int selected_id = 0;

    for (size_t n = 0; n < enabled_list.size(); n++) {
      auto value = enabled_list[n];

      if (value.empty()) {
        continue;
      }

      if (value.find('[') != std::string::npos) {
        value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
      }

      thpEnabledModel.append(QString::fromStdString(value));

      if (value == enabled_value) {
        selected_id = n;
      }
    }

    _thpEnabled = selected_id;
    Q_EMIT thpEnabledChanged();
  }

  // parameter: defrag

  {
    auto defrag_list = util::read_system_setting("/sys/kernel/mm/transparent_hugepage/defrag");

    auto defrag_value = util::get_selected_value(defrag_list);

    util::debug("transparent huge pages defrag: " + defrag_value);

    int selected_id = 0;

    for (size_t n = 0; n < defrag_list.size(); n++) {
      auto value = defrag_list[n];

      if (value.empty()) {
        continue;
      }

      if (value.find('[') != std::string::npos) {
        value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
      }

      thpDefragModel.append(QString::fromStdString(value));

      if (value == defrag_value) {
        selected_id = n;
      }
    }

    _thpDefrag = selected_id;
    Q_EMIT thpDefragChanged();
  }

  // parameter: shmem_enabled

  {
    auto shmem_enabled_list = util::read_system_setting("/sys/kernel/mm/transparent_hugepage/shmem_enabled");

    auto shmem_enabled_value = util::get_selected_value(shmem_enabled_list);

    util::debug("transparent huge pages state: " + shmem_enabled_value);

    int selected_id = 0;

    for (size_t n = 0; n < shmem_enabled_list.size(); n++) {
      auto value = shmem_enabled_list[n];

      if (value.empty()) {
        continue;
      }

      if (value.find('[') != std::string::npos) {
        value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
      }

      thpShmemEnabledModel.append(QString::fromStdString(value));

      if (value == shmem_enabled_value) {
        selected_id = n;
      }
    }

    _thpShmemEnabled = selected_id;
    Q_EMIT thpShmemEnabledChanged();
  }

  if (const auto list =
          util::read_system_setting("/sys/kernel/mm/transparent_hugepage/khugepaged/scan_sleep_millisecs");
      !list.empty()) {
    setScanSleep(std::stoi(list[0]));
  }

  if (const auto list =
          util::read_system_setting("/sys/kernel/mm/transparent_hugepage/khugepaged/alloc_sleep_millisecs");
      !list.empty()) {
    setAllocSleep(std::stoi(list[0]));
  }
}

}  // namespace memory
