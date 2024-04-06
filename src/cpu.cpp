#include "cpu.hpp"
#include <linux/prctl.h>
#include <qobject.h>
#include <qqml.h>
#include <qstring.h>
#include <qtmetamacros.h>
#include <sys/prctl.h>
#include <cstddef>
#include <string>
#include <thread>
#include "combobox_model.hpp"
#include "config.h"
#include "util.hpp"

namespace cpu {

using namespace std::string_literals;

Backend::Backend(QObject* parent) : QObject(parent) {
  qmlRegisterSingletonInstance<Backend>("FGCpuBackend", PROJECT_VERSION_MAJOR, 0, "FGCpuBackend", this);

  qmlRegisterSingletonInstance<ComboBoxModel>("FGModelFreqGovernor", PROJECT_VERSION_MAJOR, 0, "FGModelFreqGovernor",
                                              &frequencyGovernorModel);

  qmlRegisterSingletonInstance<ComboBoxModel>("FGModelPcieAspm", PROJECT_VERSION_MAJOR, 0, "FGModelPcieAspm",
                                              &pciAspmPolicyModel);

  if (const auto list = util::read_system_setting("/proc/sys/kernel/watchdog"); !list.empty()) {
    setEnableWatchdog(std::stoi(list[0]) != 0);
  }

  setTimerSlack(prctl(PR_GET_TIMERSLACK));

  initFreqGovernor();
  initPcieAspm();
}

auto Backend::useSchedBatch() const -> bool {
  return _useSchedBatch;
}

void Backend::setUseSchedBatch(const bool& value) {
  _useSchedBatch = value;

  Q_EMIT useSchedBatchChanged();
}

auto Backend::realtimeWineserver() const -> bool {
  return _realtimeWineserver;
}

void Backend::setRealtimeWineserver(const bool& value) {
  _realtimeWineserver = value;

  Q_EMIT realtimeWineserverChanged();
}

auto Backend::enableWatchdog() const -> bool {
  return _enableWatchdog;
}

void Backend::setEnableWatchdog(const bool& value) {
  _enableWatchdog = value;

  Q_EMIT enableWatchdogChanged();
}

auto Backend::useCpuDmaLatency() const -> bool {
  return _useCpuDmaLatency;
}

void Backend::setUseCpuDmaLatency(const bool& value) {
  _useCpuDmaLatency = value;

  Q_EMIT useCpuDmaLatencyChanged();
}

auto Backend::frequencyGovernor() const -> int {
  return _frequencyGovernor;
}

void Backend::setFrequencyGovernor(const int& value) {
  _frequencyGovernor = value;

  Q_EMIT frequencyGovernorChanged();
}

auto Backend::pcieAspmPolicy() const -> int {
  return _pcieAspmPolicy;
}

void Backend::setPcieAspmPolicy(const int& value) {
  _pcieAspmPolicy = value;

  Q_EMIT pcieAspmPolicyChanged();
}

auto Backend::timerSlack() const -> int {
  return _timerSlack;
}

void Backend::setTimerSlack(const int& value) {
  _timerSlack = value;

  Q_EMIT timerSlackChanged();
}

void Backend::initFreqGovernor() {
  /* We assume that all cores are set to the same frequency governor and that the system has at least one core. In
     this case reading the core 0 property should be enough
  */

  auto list_governors = util::read_system_setting("/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors");

  auto selected_governor = util::read_system_setting("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor")[0];

  int selected_id = 0;

  for (size_t n = 0; n < list_governors.size(); n++) {
    auto value = list_governors[n];

    if (value.empty()) {
      continue;
    }

    frequencyGovernorModel.append(QString::fromStdString(value));

    if (value == selected_governor) {
      selected_id = n;
    }
  }

  setFrequencyGovernor(selected_id);

  auto n_cores = std::thread::hardware_concurrency();

  util::debug("number of cores: "s + std::to_string(n_cores));
}

void Backend::initPcieAspm() {
  auto list_policies = util::read_system_setting("/sys/module/pcie_aspm/parameters/policy");

  auto selected_policy = util::get_selected_value(list_policies);

  util::debug("The current pcie_aspm policy is: "s + selected_policy);

  int selected_id = 0;

  for (size_t n = 0; n < list_policies.size(); n++) {
    auto value = list_policies[n];

    if (value.empty()) {
      continue;
    }

    if (value.find('[') != std::string::npos) {
      value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
    }

    pciAspmPolicyModel.append(QString::fromStdString(value));

    if (value == selected_policy) {
      selected_id = n;
    }
  }

  setPcieAspmPolicy(selected_id);
}

}  // namespace cpu
