#include "environment_variables.hpp"
#include <qabstractitemmodel.h>
#include <qbytearray.h>
#include <qcontainerfwd.h>
#include <qhash.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqml.h>
#include <qtmetamacros.h>
#include <qvariant.h>
#include <iterator>
#include <utility>
#include "config.h"

namespace envvars {

Model::Model(QObject* parent) : QAbstractListModel(parent) {
  qmlRegisterSingletonInstance<envvars::Model>("CppModelEnvVars", VERSION_MAJOR, VERSION_MINOR, "CppModelEnvVars",
                                               this);
}

int Model::rowCount(const QModelIndex& /*parent*/) const {
  return list.size();
}

QHash<int, QByteArray> Model::roleNames() const {
  return {{Roles::Name, "name"}, {Roles::Value, "value"}};
}

QVariant Model::data(const QModelIndex& index, int role) const {
  if (list.empty()) {
    return "";
  }

  const auto it = std::next(list.begin(), index.row());

  switch (role) {
    case Roles::Name:
      return it->first;
    case Roles::Value:
      return it->second;
    default:
      return {};
  }
}

bool Model::setData(const QModelIndex& index, const QVariant& value, int role) {
  if (!value.canConvert<QString>() && role != Qt::EditRole) {
    return false;
  }

  auto it = std::next(list.begin(), index.row());

  switch (role) {
    case Roles::Name: {
      it->first = value.toString();

      emit dataChanged(index, index, {Roles::Name});

      break;
    }
    case Roles::Value: {
      it->second = value.toString();

      emit dataChanged(index, index, {Roles::Value});

      break;
    }
    default:
      break;
  }

  return true;
}

void Model::reset() {
  beginResetModel();

  list.clear();

  endResetModel();
}

auto Model::getList() -> QList<QPair<QString, QString>> {
  return list;
}

void Model::append(const QString& name, const QString& value) {
  if (list.contains({name, value})) {
    return;
  }

  int pos = list.empty() ? 0 : list.size() - 1;

  beginInsertRows(QModelIndex(), pos, pos);

  list.append({name, value});

  endInsertRows();

  emit dataChanged(index(0), index(list.size() - 1));
}

void Model::remove(const int& rowIndex) {
  beginRemoveRows(QModelIndex(), rowIndex, rowIndex);

  list.remove(rowIndex);

  endRemoveRows();

  emit dataChanged(index(0), index(list.size() - 1));
}

void Model::move(const int& from, const int& to) {
  int destination = from < to ? (to + 1) : to;

  beginMoveRows(QModelIndex(), from, from, QModelIndex(), destination);

  list.move(from, to);

  endMoveRows();

  emit dataChanged(index(0), index(list.size() - 1));
}

}  // namespace envvars