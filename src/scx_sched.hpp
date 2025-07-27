#pragma once

#include <qabstractitemmodel.h>
#include <qcontainerfwd.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <QAbstractListModel>
#include <string>
#include "combobox_model.hpp"

namespace scx_sched {

class Backend : public QObject {
  Q_OBJECT

  Q_PROPERTY(int scheduler MEMBER _scheduler NOTIFY schedulerChanged)

  Q_PROPERTY(int enable MEMBER _enable NOTIFY enableChanged)

 public:
  explicit Backend(QObject* parent = nullptr);

  [[nodiscard]] auto scheduler() -> std::string;
  [[nodiscard]] auto enable() const -> bool;

  void setEnable(const bool& value);
  void setScheduler(const std::string& value);

 signals:
  void schedulerChanged();
  void enableChanged();

 private:
  bool _enable = false;

  int _scheduler;

  ComboBoxModel schedulerModel;

  void init_scheduler();
};

}  // namespace scx_sched