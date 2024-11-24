#include "cpu.hpp"
#include <linux/prctl.h>
#include <qobject.h>
#include <qqml.h>
#include <qstring.h>
#include <qtmetamacros.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <cstddef>
#include <string>
#include <thread>
#include "combobox_model.hpp"
#include "config.h"
#include "util.hpp"

namespace cpu {

Backend::Backend(QObject* parent) : QObject(parent) {
  qmlRegisterSingletonInstance<Backend>("FGCpuBackend", VERSION_MAJOR, VERSION_MINOR, "FGCpuBackend", this);

  qmlRegisterSingletonInstance<ComboBoxModel>("FGModelFreqGovernor", VERSION_MAJOR, VERSION_MINOR,
                                              "FGModelFreqGovernor", &frequencyGovernorModel);

  qmlRegisterSingletonInstance<ComboBoxModel>("FGModelPcieAspm", VERSION_MAJOR, VERSION_MINOR, "FGModelPcieAspm",
                                              &pciAspmPolicyModel);

  qmlRegisterSingletonInstance<ComboBoxModel>("FGModelWorkqueueAffinityScope", VERSION_MAJOR, VERSION_MINOR,
                                              "FGModelWorkqueueAffinityScope", &workqueueAffinityScopeModel);

  if (const auto list = util::read_system_setting("/proc/sys/kernel/watchdog"); !list.empty()) {
    setEnableWatchdog(std::stoi(list[0]) != 0);
  }

  if (const auto list = util::read_system_setting("/sys/module/workqueue/parameters/cpu_intensive_thresh_us");
      !list.empty()) {
    setCpuIntensiveThreshold(std::stoi(list[0]));
  }

  setTimerSlack(prctl(PR_GET_TIMERSLACK));

  initFreqGovernor();
  initPcieAspm();
  init_workqueue_affinity_scope();

  setSchedRuntime(util::get_sched_runtime(0, 0) * 0.000001);
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

auto Backend::frequencyGovernor() -> std::string {
  return frequencyGovernorModel.getValue(_frequencyGovernor).toStdString();
}

void Backend::setFrequencyGovernor(const std::string& value) {
  auto id = frequencyGovernorModel.getId(QString::fromStdString(value));

  if (id == -1) {
    return;
  }

  _frequencyGovernor = id;

  Q_EMIT frequencyGovernorChanged();
}

auto Backend::pcieAspmPolicy() -> std::string {
  return pciAspmPolicyModel.getValue(_pcieAspmPolicy).toStdString();
}

void Backend::setPcieAspmPolicy(const std::string& value) {
  auto id = pciAspmPolicyModel.getId(QString::fromStdString(value));

  if (id == -1) {
    return;
  }

  _pcieAspmPolicy = id;

  Q_EMIT pcieAspmPolicyChanged();
}

auto Backend::workqueueAffinityScope() -> std::string {
  return workqueueAffinityScopeModel.getValue(_workqueueAffinityScope).toStdString();
}

void Backend::setWorkqueueAffinityScope(const std::string& value) {
  auto id = workqueueAffinityScopeModel.getId(QString::fromStdString(value));

  if (id == -1) {
    return;
  }

  _workqueueAffinityScope = id;

  Q_EMIT workqueueAffinityScopeChanged();
}

auto Backend::timerSlack() const -> int {
  return _timerSlack;
}

void Backend::setTimerSlack(const int& value) {
  _timerSlack = value;

  Q_EMIT timerSlackChanged();
}

auto Backend::schedRuntime() const -> int {
  return _schedRuntime;
}

void Backend::setSchedRuntime(const int& value) {
  _schedRuntime = value;

  Q_EMIT schedRuntimeChanged();
}

auto Backend::cpuIntensiveThreshold() const -> int {
  return _cpuIntensiveThreshold;
}

void Backend::setCpuIntensiveThreshold(const int& value) {
  _cpuIntensiveThreshold = value;

  Q_EMIT cpuIntensiveThresholdChanged();
}

auto Backend::niceness() const -> int {
  return _niceness;
}

void Backend::setNiceness(const int& value) {
  _niceness = value;

  Q_EMIT nicenessChanged();
}

auto Backend::autogroupNiceness() const -> int {
  return _autogroupNiceness;
}

void Backend::setAutogroupNiceness(const int& value) {
  _autogroupNiceness = value;

  Q_EMIT autogroupNicenessChanged();
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

  _frequencyGovernor = selected_id;

  Q_EMIT frequencyGovernorChanged();

  auto n_cores = std::thread::hardware_concurrency();

  util::debug("number of cores: " + util::to_string(n_cores));
}

void Backend::initPcieAspm() {
  auto list_policies = util::read_system_setting("/sys/module/pcie_aspm/parameters/policy");

  auto selected_policy = util::get_selected_value(list_policies);

  util::debug("The current pcie_aspm policy is: " + selected_policy);

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

  _pcieAspmPolicy = selected_id;
  Q_EMIT pcieAspmPolicyChanged();
}

void Backend::init_workqueue_affinity_scope() {
  // https://www.kernel.org/doc/html/latest/core-api/workqueue.html

  if (const auto scope = util::read_system_setting("/sys/module/workqueue/parameters/default_affinity_scope");
      !scope.empty()) {
    workqueueAffinityScopeModel.append("cpu");
    workqueueAffinityScopeModel.append("smt");
    workqueueAffinityScopeModel.append("cache");
    workqueueAffinityScopeModel.append("numa");
    workqueueAffinityScopeModel.append("system");

    _workqueueAffinityScope = workqueueAffinityScopeModel.getId(QString::fromStdString(scope[0]));
    Q_EMIT workqueueAffinityScopeChanged();
  }
}

}  // namespace cpu
