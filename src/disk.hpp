#pragma once

#include <qdebug.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <string>
#include "combobox_model.hpp"

namespace disk {

class Backend : public QObject {
  Q_OBJECT

  Q_PROPERTY(int mountingPath READ mountingPath WRITE setMountingPath NOTIFY mountingPathChanged)

  Q_PROPERTY(int scheduler READ scheduler WRITE setScheduler NOTIFY schedulerChanged)

  Q_PROPERTY(int readahead READ readahead WRITE setReadahead NOTIFY readaheadChanged)

  Q_PROPERTY(bool enableRealtimePriority READ enableRealtimePriority WRITE setEnableRealtimePriority NOTIFY
                 enableRealtimePriorityChanged)

  Q_PROPERTY(bool addRandom READ addRandom WRITE setAddRandom NOTIFY addRandomChanged)

  Q_PROPERTY(bool useDiskDmaLatency READ useDiskDmaLatency WRITE setUseDiskDmaLatency NOTIFY useDiskDmaLatencyChanged)

  Q_PROPERTY(int timerSlack READ timerSlack WRITE setTimerSlack NOTIFY timerSlackChanged)

  Q_PROPERTY(QString gameAffinity READ gameAffinity WRITE setGameAffinity NOTIFY gameAffinityChanged)

  Q_PROPERTY(
      QString wineServerAffinity READ wineServerAffinity WRITE setWineServerAffinity NOTIFY wineServerAffinityChanged)

 public:
  explicit Backend(QObject* parent = nullptr);

  [[nodiscard]] auto mountingPath() const -> int;
  [[nodiscard]] auto scheduler() const -> int;
  [[nodiscard]] auto readahead() const -> int;
  [[nodiscard]] auto enableRealtimePriority() const -> bool;
  [[nodiscard]] auto addRandom() const -> bool;
  [[nodiscard]] auto useDiskDmaLatency() const -> bool;
  [[nodiscard]] auto timerSlack() const -> int;
  [[nodiscard]] auto gameAffinity() const -> QString;
  [[nodiscard]] auto wineServerAffinity() const -> QString;

  void setMountingPath(const int& value);
  void setScheduler(const int& value);
  void setReadahead(const int& value);
  void setEnableRealtimePriority(const bool& value);
  void setAddRandom(const bool& value);
  void setUseDiskDmaLatency(const bool& value);
  void setTimerSlack(const int& value);
  void setGameAffinity(const QString& value);
  void setWineServerAffinity(const QString& value);

 signals:
  void mountingPathChanged();
  void schedulerChanged();
  void readaheadChanged();
  void enableRealtimePriorityChanged();
  void addRandomChanged();
  void useDiskDmaLatencyChanged();
  void timerSlackChanged();
  void gameAffinityChanged();
  void wineServerAffinityChanged();

 private:
  bool _enableRealtimePriority = false;
  bool _addRandom = false;
  bool _useDiskDmaLatency = false;

  int _mountingPath = -1;
  int _scheduler;
  int _readahead;
  int _timerSlack;

  QString _gameAffinity;
  QString _wineServerAffinity;

  ComboBoxModel mountingPathModel;
  ComboBoxModel schedulerModel;

  void init_scheduler(const std::string& sys_class_path);
};

}  // namespace disk