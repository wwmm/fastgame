#include "command_line_arguments.hpp"
#include <qabstractitemmodel.h>
#include <qbytearray.h>
#include <qhash.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqml.h>
#include <qtmetamacros.h>
#include <qvariant.h>
#include <iterator>
#include "config.h"

namespace cmdargs {

Model::Model(QObject* parent) : QAbstractListModel(parent) {
  qmlRegisterSingletonInstance<cmdargs::Model>("CppModelCmdLineArgs", PROJECT_VERSION_MAJOR, 0, "CppModelCmdLineArgs",
                                               this);
}

int Model::rowCount(const QModelIndex& /*parent*/) const {
  return list.size();
}

QHash<int, QByteArray> Model::roleNames() const {
  return {{Roles::Value, "value"}};
}

QVariant Model::data(const QModelIndex& index, int role) const {
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

bool Model::setData(const QModelIndex& index, const QVariant& value, int role) {
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

void Model::reset() {
  beginResetModel();

  list.clear();

  endResetModel();
}

auto Model::getList() -> QList<QString> {
  return list;
}

void Model::append(const QString& value) {
  int pos = list.empty() ? 0 : list.size() - 1;

  beginInsertRows(QModelIndex(), pos, pos);

  list.append(value);

  endInsertRows();

  emit dataChanged(index(0), index(list.size() - 1));
}

void Model::remove(const int& rowIndex) {
  beginRemoveRows(QModelIndex(), rowIndex, rowIndex);

  list.remove(rowIndex);

  endRemoveRows();

  emit dataChanged(index(0), index(list.size() - 1));
}

}  // namespace cmdargs