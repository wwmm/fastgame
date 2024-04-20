#pragma once

#include <qdebug.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <string>
#include "combobox_model.hpp"

namespace disk {

class Backend : public QObject {
  Q_OBJECT

  Q_PROPERTY(int mountingPath MEMBER _mountingPath NOTIFY mountingPathChanged)

  Q_PROPERTY(int scheduler MEMBER _scheduler NOTIFY schedulerChanged)

  Q_PROPERTY(int readahead READ readahead WRITE setReadahead NOTIFY readaheadChanged)

  Q_PROPERTY(bool enableRealtimePriority READ enableRealtimePriority WRITE setEnableRealtimePriority NOTIFY
                 enableRealtimePriorityChanged)

  Q_PROPERTY(bool addRandom READ addRandom WRITE setAddRandom NOTIFY addRandomChanged)

  Q_PROPERTY(int nrRequests READ nrRequests WRITE setNrRequests NOTIFY nrRequestsChanged)

  Q_PROPERTY(int rqAffinity READ rqAffinity WRITE setRqAffinity NOTIFY rqAffinityChanged)

  Q_PROPERTY(int noMerges READ noMerges WRITE setNoMerges NOTIFY noMergesChanged)

  Q_PROPERTY(int wbtLatUsec READ wbtLatUsec WRITE setWbtLatUsec NOTIFY wbtLatUsecChanged)

 public:
  explicit Backend(QObject* parent = nullptr);

  [[nodiscard]] auto mountingPath() -> std::string;
  [[nodiscard]] auto scheduler() -> std::string;
  [[nodiscard]] auto readahead() const -> int;
  [[nodiscard]] auto enableRealtimePriority() const -> bool;
  [[nodiscard]] auto addRandom() const -> bool;
  [[nodiscard]] auto nrRequests() const -> int;
  [[nodiscard]] auto rqAffinity() const -> int;
  [[nodiscard]] auto noMerges() const -> int;
  [[nodiscard]] auto wbtLatUsec() const -> int;

  void setMountingPath(const std::string& value);
  void setScheduler(const std::string& value);
  void setReadahead(const int& value);
  void setEnableRealtimePriority(const bool& value);
  void setAddRandom(const bool& value);
  void setNrRequests(const int& value);
  void setRqAffinity(const int& value);
  void setNoMerges(const int& value);
  void setWbtLatUsec(const int& value);

 signals:
  void mountingPathChanged();
  void schedulerChanged();
  void readaheadChanged();
  void enableRealtimePriorityChanged();
  void addRandomChanged();
  void nrRequestsChanged();
  void rqAffinityChanged();
  void noMergesChanged();
  void wbtLatUsecChanged();

 private:
  bool _enableRealtimePriority = false;
  bool _addRandom = false;

  int _mountingPath = -1;
  int _scheduler;
  int _readahead;
  int _nrRequests;
  int _rqAffinity;
  int _noMerges;
  int _wbtLatUsec;

  ComboBoxModel mountingPathModel;
  ComboBoxModel schedulerModel;

  void init_scheduler(const std::string& sys_class_path);
};

}  // namespace disk