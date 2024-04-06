#include "combobox_model.hpp"
#include <qabstractitemmodel.h>
#include <qbytearray.h>
#include <qhash.h>
#include <qnamespace.h>
#include <qstring.h>
#include <qtmetamacros.h>
#include <qvariant.h>
#include <iterator>

int ComboBoxModel::rowCount(const QModelIndex& /*parent*/) const {
  return list.size();
}

QHash<int, QByteArray> ComboBoxModel::roleNames() const {
  return {{Roles::Value, "value"}};
}

QVariant ComboBoxModel::data(const QModelIndex& index, int role) const {
  if (list.empty()) {
    return "";
  }

  const auto it = std::next(list.begin(), index.row());

  switch (role) {
    case Roles::Value:
      return *it;
    default:
      return {};
  }
}

bool ComboBoxModel::setData(const QModelIndex& index, const QVariant& value, int role) {
  if (!value.canConvert<QString>() && role != Qt::EditRole) {
    return false;
  }

  auto it = std::next(list.begin(), index.row());

  switch (role) {
    case Roles::Value: {
      *it = value.toString();

      emit dataChanged(index, index, {Roles::Value});

      break;
    }
    default:
      break;
  }

  return true;
}

auto ComboBoxModel::getValue(const int& id) -> QString {
  return list[id];
}

void ComboBoxModel::append(const QString& value) {
  int pos = list.empty() ? 0 : list.size() - 1;

  beginInsertRows(QModelIndex(), pos, pos);

  list.append(value);

  endInsertRows();

  emit dataChanged(index(0), index(list.size() - 1));
}

void ComboBoxModel::remove(const int& rowIndex) {
  beginRemoveRows(QModelIndex(), rowIndex, rowIndex);

  list.remove(rowIndex);

  endRemoveRows();

  emit dataChanged(index(0), index(list.size() - 1));
}