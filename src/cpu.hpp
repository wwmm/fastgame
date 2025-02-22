#pragma once

#include <qdebug.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <string>
#include "combobox_model.hpp"

namespace cpu {

class Backend : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool useSchedBatch READ useSchedBatch WRITE setUseSchedBatch NOTIFY useSchedBatchChanged)

  Q_PROPERTY(
      bool realtimeWineserver READ realtimeWineserver WRITE setRealtimeWineserver NOTIFY realtimeWineserverChanged)

  Q_PROPERTY(bool enableWatchdog READ enableWatchdog WRITE setEnableWatchdog NOTIFY enableWatchdogChanged)

  Q_PROPERTY(bool useCpuDmaLatency READ useCpuDmaLatency WRITE setUseCpuDmaLatency NOTIFY useCpuDmaLatencyChanged)

  Q_PROPERTY(bool enableSplitLockMitigation READ enableSplitLockMitigation WRITE setEnableSplitLockMitigation NOTIFY
                 enableSplitLockMitigationChanged)

  Q_PROPERTY(int cpuDmaLatency READ cpuDmaLatency WRITE setCpuDmaLatency NOTIFY cpuDmaLatencyChanged)

  Q_PROPERTY(int frequencyGovernor MEMBER _frequencyGovernor NOTIFY frequencyGovernorChanged)

  Q_PROPERTY(int pcieAspmPolicy MEMBER _pcieAspmPolicy NOTIFY pcieAspmPolicyChanged)

  Q_PROPERTY(int workqueueAffinityScope MEMBER _workqueueAffinityScope NOTIFY workqueueAffinityScopeChanged)

  Q_PROPERTY(int timerSlack READ timerSlack WRITE setTimerSlack NOTIFY timerSlackChanged)

  Q_PROPERTY(int cpuIntensiveThreshold READ cpuIntensiveThreshold WRITE setCpuIntensiveThreshold NOTIFY
                 cpuIntensiveThresholdChanged)

  Q_PROPERTY(int niceness READ niceness WRITE setNiceness NOTIFY nicenessChanged)

  Q_PROPERTY(int autogroupNiceness READ autogroupNiceness WRITE setAutogroupNiceness NOTIFY autogroupNicenessChanged)

  Q_PROPERTY(double schedRuntime READ schedRuntime WRITE setSchedRuntime NOTIFY schedRuntimeChanged)

  Q_PROPERTY(QString gameAffinity READ gameAffinity WRITE setGameAffinity NOTIFY gameAffinityChanged)

  Q_PROPERTY(
      QString wineServerAffinity READ wineServerAffinity WRITE setWineServerAffinity NOTIFY wineServerAffinityChanged)

 public:
  explicit Backend(QObject* parent = nullptr);

  [[nodiscard]] auto useSchedBatch() const -> bool;
  [[nodiscard]] auto realtimeWineserver() const -> bool;
  [[nodiscard]] auto enableWatchdog() const -> bool;
  [[nodiscard]] auto useCpuDmaLatency() const -> bool;
  [[nodiscard]] auto enableSplitLockMitigation() const -> bool;
  [[nodiscard]] auto frequencyGovernor() -> std::string;
  [[nodiscard]] auto pcieAspmPolicy() -> std::string;
  [[nodiscard]] auto workqueueAffinityScope() -> std::string;
  [[nodiscard]] auto cpuDmaLatency() const -> int;
  [[nodiscard]] auto timerSlack() const -> int;
  [[nodiscard]] auto cpuIntensiveThreshold() const -> int;
  [[nodiscard]] auto niceness() const -> int;
  [[nodiscard]] auto autogroupNiceness() const -> int;
  [[nodiscard]] auto schedRuntime() const -> double;
  [[nodiscard]] auto gameAffinity() const -> QString;
  [[nodiscard]] auto wineServerAffinity() const -> QString;

  void setUseSchedBatch(const bool& value);
  void setRealtimeWineserver(const bool& value);
  void setEnableWatchdog(const bool& value);
  void setUseCpuDmaLatency(const bool& value);
  void setEnableSplitLockMitigation(const bool& value);
  void setFrequencyGovernor(const std::string& value);
  void setPcieAspmPolicy(const std::string& value);
  void setWorkqueueAffinityScope(const std::string& value);
  void setCpuDmaLatency(const int& value);
  void setCpuIntensiveThreshold(const int& value);
  void setNiceness(const int& value);
  void setAutogroupNiceness(const int& value);
  void setTimerSlack(const int& value);
  void setSchedRuntime(const double& value);
  void setGameAffinity(const QString& value);
  void setWineServerAffinity(const QString& value);

 signals:
  void useSchedBatchChanged();
  void realtimeWineserverChanged();
  void enableWatchdogChanged();
  void useCpuDmaLatencyChanged();
  void cpuDmaLatencyChanged();
  void enableSplitLockMitigationChanged();
  void frequencyGovernorChanged();
  void pcieAspmPolicyChanged();
  void workqueueAffinityScopeChanged();
  void timerSlackChanged();
  void schedRuntimeChanged();
  void cpuIntensiveThresholdChanged();
  void nicenessChanged();
  void autogroupNicenessChanged();
  void gameAffinityChanged();
  void wineServerAffinityChanged();

 private:
  bool _useSchedBatch = false;
  bool _realtimeWineserver = false;
  bool _enableWatchdog = false;
  bool _useCpuDmaLatency = false;
  bool _enableSplitLockMitigation = false;

  int _cpuDmaLatency = 0;
  int _frequencyGovernor;
  int _pcieAspmPolicy;
  int _workqueueAffinityScope;
  int _timerSlack;
  int _cpuIntensiveThreshold;
  int _niceness;
  int _autogroupNiceness;

  double _schedRuntime;

  QString _gameAffinity;
  QString _wineServerAffinity;

  ComboBoxModel frequencyGovernorModel;
  ComboBoxModel pciAspmPolicyModel;
  ComboBoxModel workqueueAffinityScopeModel;

  void initFreqGovernor();
  void initPcieAspm();
  void init_workqueue_affinity_scope();
};

}  // namespace cpu