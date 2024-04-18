#pragma once

#include <qabstractitemmodel.h>
#include <qbytearray.h>
#include <qcontainerfwd.h>
#include <qhash.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qstring.h>
#include <qtmetamacros.h>
#include <qvariant.h>
#include <vector>
#include "amdgpu.hpp"
#include "command_line_arguments.hpp"
#include "cpu.hpp"
#include "disk.hpp"
#include "environment_variables.hpp"
#include "memory.hpp"
#include "nvidia.hpp"

namespace presets {

class MenuModel : public QAbstractListModel {
  Q_OBJECT;

 public:
  explicit MenuModel(QObject* parent = nullptr);

  enum Roles { Name = Qt::UserRole };

  [[nodiscard]] int rowCount(const QModelIndex& /*parent*/) const override;

  [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

  [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;

  bool setData(const QModelIndex& index, const QVariant& value, int role) override;

  Q_INVOKABLE void append(const QString& name);

  Q_INVOKABLE void remove(const int& rowIndex);

 private:
  QList<QString> list;
};

class Backend : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString executableName MEMBER _executableName NOTIFY executableNameChanged)

 public:
  explicit Backend(QObject* parent = nullptr);

  Q_INVOKABLE bool loadPreset(const QString& name);

 signals:
  void executableNameChanged();

 private:
  QString _executableName;

  MenuModel menuModel;

  cmdargs::Model cmdArgsModel;
  envvars::Model envVarsModel;

  amdgpu::Backend amdgpuBackend;
  cpu::Backend cpuBackend;
  disk::Backend diskBackend;
  memory::Backend memoryBackend;
  nvidia::Backend nvidiaBackend;

  static auto get_presets_names() -> std::vector<QString>;
};

}  // namespace presets