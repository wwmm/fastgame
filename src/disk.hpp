#pragma once

#include <qdebug.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include "combobox_model.hpp"

namespace disk {

class Backend : public QObject {
  Q_OBJECT

  Q_PROPERTY(int mountingPath READ mountingPath WRITE setMountingPath NOTIFY mountingPathChanged)

  Q_PROPERTY(int scheduler READ scheduler WRITE setScheduler NOTIFY schedulerChanged)

  Q_PROPERTY(bool useSchedBatch READ useSchedBatch WRITE setUseSchedBatch NOTIFY useSchedBatchChanged)

  Q_PROPERTY(
      bool realtimeWineserver READ realtimeWineserver WRITE setRealtimeWineserver NOTIFY realtimeWineserverChanged)

  Q_PROPERTY(bool enableWatchdog READ enableWatchdog WRITE setEnableWatchdog NOTIFY enableWatchdogChanged)

  Q_PROPERTY(bool useDiskDmaLatency READ useDiskDmaLatency WRITE setUseDiskDmaLatency NOTIFY useDiskDmaLatencyChanged)

  Q_PROPERTY(int timerSlack READ timerSlack WRITE setTimerSlack NOTIFY timerSlackChanged)

  Q_PROPERTY(QString gameAffinity READ gameAffinity WRITE setGameAffinity NOTIFY gameAffinityChanged)

  Q_PROPERTY(
      QString wineServerAffinity READ wineServerAffinity WRITE setWineServerAffinity NOTIFY wineServerAffinityChanged)

 public:
  explicit Backend(QObject* parent = nullptr);

  [[nodiscard]] auto mountingPath() const -> int;
  [[nodiscard]] auto scheduler() const -> int;
  [[nodiscard]] auto useSchedBatch() const -> bool;
  [[nodiscard]] auto realtimeWineserver() const -> bool;
  [[nodiscard]] auto enableWatchdog() const -> bool;
  [[nodiscard]] auto useDiskDmaLatency() const -> bool;
  [[nodiscard]] auto timerSlack() const -> int;
  [[nodiscard]] auto gameAffinity() const -> QString;
  [[nodiscard]] auto wineServerAffinity() const -> QString;

  void setMountingPath(const int& value);
  void setScheduler(const int& value);
  void setUseSchedBatch(const bool& value);
  void setRealtimeWineserver(const bool& value);
  void setEnableWatchdog(const bool& value);
  void setUseDiskDmaLatency(const bool& value);
  void setTimerSlack(const int& value);
  void setGameAffinity(const QString& value);
  void setWineServerAffinity(const QString& value);

 signals:
  void mountingPathChanged();
  void schedulerChanged();
  void useSchedBatchChanged();
  void realtimeWineserverChanged();
  void enableWatchdogChanged();
  void useDiskDmaLatencyChanged();
  void timerSlackChanged();
  void gameAffinityChanged();
  void wineServerAffinityChanged();

 private:
  bool _useSchedBatch = false;
  bool _realtimeWineserver = false;
  bool _enableWatchdog = false;
  bool _useDiskDmaLatency = false;

  int _mountingPath = -1;
  int _scheduler;
  int _timerSlack;

  QString _gameAffinity;
  QString _wineServerAffinity;

  ComboBoxModel mountingPathModel;
  ComboBoxModel schedulerModel;

  void init_scheduler();
};

}  // namespace disk