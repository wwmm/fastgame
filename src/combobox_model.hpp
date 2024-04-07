#pragma once

#include <qabstractitemmodel.h>
#include <qbytearray.h>
#include <qhash.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qtmetamacros.h>
#include <qvariant.h>

class ComboBoxModel : public QAbstractListModel {
  Q_OBJECT;

 public:
  enum Roles { Value = Qt::UserRole };

  [[nodiscard]] int rowCount(const QModelIndex& /*parent*/) const override;

  [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

  [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;

  bool setData(const QModelIndex& index, const QVariant& value, int role) override;

  auto getValue(const int& id) -> QString;

  void reset();

  Q_INVOKABLE void append(const QString& value);

  Q_INVOKABLE void remove(const int& rowIndex);

 private:
  QList<QString> list;
};