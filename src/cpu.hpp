#pragma once

#include <qdebug.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include "combobox_model.hpp"

namespace cpu {

class Backend : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool useSchedBatch READ useSchedBatch WRITE setUseSchedBatch NOTIFY useSchedBatchChanged)

  Q_PROPERTY(
      bool realtimeWineserver READ realtimeWineserver WRITE setRealtimeWineserver NOTIFY realtimeWineserverChanged)

  Q_PROPERTY(bool enableWatchdog READ enableWatchdog WRITE setEnableWatchdog NOTIFY enableWatchdogChanged)

  Q_PROPERTY(bool useCpuDmaLatency READ useCpuDmaLatency WRITE setUseCpuDmaLatency NOTIFY useCpuDmaLatencyChanged)

  Q_PROPERTY(int frequencyGovernor READ frequencyGovernor WRITE setFrequencyGovernor NOTIFY frequencyGovernorChanged)

  Q_PROPERTY(int pcieAspmPolicy READ pcieAspmPolicy WRITE setPcieAspmPolicy NOTIFY pcieAspmPolicyChanged)

  Q_PROPERTY(int timerSlack READ timerSlack WRITE setTimerSlack NOTIFY timerSlackChanged)

 public:
  explicit Backend(QObject* parent = nullptr);

  [[nodiscard]] auto useSchedBatch() const -> bool;
  [[nodiscard]] auto realtimeWineserver() const -> bool;
  [[nodiscard]] auto enableWatchdog() const -> bool;
  [[nodiscard]] auto useCpuDmaLatency() const -> bool;
  [[nodiscard]] auto frequencyGovernor() const -> int;
  [[nodiscard]] auto pcieAspmPolicy() const -> int;
  [[nodiscard]] auto timerSlack() const -> int;

  void setUseSchedBatch(const bool& value);
  void setRealtimeWineserver(const bool& value);
  void setEnableWatchdog(const bool& value);
  void setUseCpuDmaLatency(const bool& value);
  void setFrequencyGovernor(const int& value);
  void setPcieAspmPolicy(const int& value);
  void setTimerSlack(const int& value);

 signals:
  void useSchedBatchChanged();
  void realtimeWineserverChanged();
  void enableWatchdogChanged();
  void useCpuDmaLatencyChanged();
  void frequencyGovernorChanged();
  void pcieAspmPolicyChanged();
  void timerSlackChanged();

 private:
  bool _useSchedBatch = false;
  bool _realtimeWineserver = false;
  bool _enableWatchdog = false;
  bool _useCpuDmaLatency = false;

  int _frequencyGovernor;
  int _pcieAspmPolicy;
  int _timerSlack;

  ComboBoxModel frequencyGovernorModel;
  ComboBoxModel pciAspmPolicyModel;

  void initFreqGovernor();
  void initPcieAspm();
};

}  // namespace cpu