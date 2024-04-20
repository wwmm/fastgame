#pragma once

#include <qabstractitemmodel.h>
#include <qcontainerfwd.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <QAbstractListModel>

namespace cmdargs {

class Model : public QAbstractListModel {
  Q_OBJECT;

 public:
  explicit Model(QObject* parent = nullptr);

  enum Roles { Value = Qt::UserRole };

  [[nodiscard]] int rowCount(const QModelIndex& /*parent*/) const override;

  [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

  [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;

  bool setData(const QModelIndex& index, const QVariant& value, int role) override;

  void reset();

  auto getList() -> QList<QString>;

  Q_INVOKABLE void append(const QString& value);

  Q_INVOKABLE void remove(const int& rowIndex);

 private:
  QList<QString> list;
};

}  // namespace cmdargs