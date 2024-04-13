#include "presets_manager.hpp"
#include <qabstractitemmodel.h>
#include <qbytearray.h>
#include <qhash.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqml.h>
#include <qstandardpaths.h>
#include <qstring.h>
#include <qtmetamacros.h>
#include <qvariant.h>
#include <filesystem>
#include <iterator>
#include <vector>
#include "config.h"
#include "util.hpp"

namespace presets {

std::filesystem::path user_presets_dir =
    (QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/fastgame").toStdString();

MenuModel::MenuModel(QObject* parent) : QAbstractListModel(parent) {
  qmlRegisterSingletonInstance<MenuModel>("PresetsMenuModel", PROJECT_VERSION_MAJOR, 0, "PresetsMenuModel", this);
}

int MenuModel::rowCount(const QModelIndex& /*parent*/) const {
  return list.size();
}

QHash<int, QByteArray> MenuModel::roleNames() const {
  return {{Roles::Name, "name"}};
}

QVariant MenuModel::data(const QModelIndex& index, int role) const {
  if (list.empty()) {
    return "";
  }

  const auto it = std::next(list.begin(), index.row());

  switch (role) {
    case Roles::Name:
      return *it;
    default:
      return {};
  }
}

bool MenuModel::setData(const QModelIndex& index, const QVariant& value, int role) {
  if (!value.canConvert<QString>() && role != Qt::EditRole) {
    return false;
  }

  auto it = std::next(list.begin(), index.row());

  switch (role) {
    case Roles::Name: {
      *it = value.toString();

      emit dataChanged(index, index, {Roles::Name});

      break;
    }
    default:
      break;
  }

  return true;
}

void MenuModel::append(const QString& name) {
  int pos = list.empty() ? 0 : list.size() - 1;

  beginInsertRows(QModelIndex(), pos, pos);

  list.append(name);

  endInsertRows();

  emit dataChanged(index(0), index(list.size() - 1));
}

void MenuModel::remove(const int& rowIndex) {
  beginRemoveRows(QModelIndex(), rowIndex, rowIndex);

  list.remove(rowIndex);

  endRemoveRows();

  emit dataChanged(index(0), index(list.size() - 1));
}

Backend::Backend(QObject* parent) : QObject(parent) {
  qmlRegisterSingletonInstance<Backend>("FGPresetsMenuBackend", PROJECT_VERSION_MAJOR, 0, "FGPresetsMenuBackend", this);

  if (!std::filesystem::is_directory(user_presets_dir)) {
    if (std::filesystem::create_directories(user_presets_dir)) {
      util::debug("user presets directory created: " + user_presets_dir.string());
    } else {
      util::warning("failed to create user presets directory: " + user_presets_dir.string());
    }
  } else {
    util::debug("user presets directory already exists: " + user_presets_dir.string());
  }

  for (const auto& name : get_presets_names()) {
    menuModel.append(name);
  }
}

auto Backend::get_presets_names() -> std::vector<QString> {
  std::vector<QString> names;

  for (const auto& entry : std::filesystem::directory_iterator(user_presets_dir)) {
    names.emplace_back(QString::fromStdString(entry.path().stem().string()));
  }

  return names;
}

}  // namespace presets