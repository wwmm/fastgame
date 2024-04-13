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

 public:
  explicit Backend(QObject* parent = nullptr);

 signals:

 private:
  MenuModel menuModel;

  static auto get_presets_names() -> std::vector<QString>;
};

}  // namespace presets