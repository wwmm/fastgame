#include "presets_manager.hpp"
#include <qabstractitemmodel.h>
#include <qbytearray.h>
#include <qhash.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqml.h>
#include <qsortfilterproxymodel.h>
#include <qstandardpaths.h>
#include <qstring.h>
#include <qtmetamacros.h>
#include <qvariant.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include <filesystem>
#include <iterator>
#include <string>
#include <vector>
#include "config.h"
#include "util.hpp"

namespace presets {

std::filesystem::path user_presets_dir =
    (QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/fastgame").toStdString();

MenuModel::MenuModel(QObject* parent) : QAbstractListModel(parent) {}

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
  // creating the presets folder if it does not exist

  if (!std::filesystem::is_directory(user_presets_dir)) {
    if (std::filesystem::create_directories(user_presets_dir)) {
      util::debug("user presets directory created: " + user_presets_dir.string());
    } else {
      util::warning("failed to create user presets directory: " + user_presets_dir.string());
    }
  } else {
    util::debug("user presets directory already exists: " + user_presets_dir.string());
  }

  auto* proxyModel = new QSortFilterProxyModel(this);

  proxyModel->setSourceModel(&menuModel);
  proxyModel->setFilterRole(MenuModel::Roles::Name);
  proxyModel->setSortRole(MenuModel::Roles::Name);
  proxyModel->setDynamicSortFilter(true);
  proxyModel->sort(0);

  qmlRegisterSingletonInstance<QSortFilterProxyModel>("FGPresetsMenuModel", PROJECT_VERSION_MAJOR, 0,
                                                      "FGPresetsMenuModel", proxyModel);

  qmlRegisterSingletonInstance<Backend>("FGPresetsBackend", PROJECT_VERSION_MAJOR, 0, "FGPresetsBackend", this);

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

bool Backend::loadPreset(const QString& name) {
  bool status = true;

  auto input_file = user_presets_dir / std::filesystem::path{name.toStdString() + ".json"};

  boost::property_tree::ptree root;

  boost::property_tree::read_json(input_file.string(), root);

  // game executable

  _executableName = QString::fromStdString(root.get<std::string>("game-executable", ""));
  Q_EMIT executableNameChanged();

  // environmental variables

  try {
    envVarsModel.reset();

    for (const auto& c : root.get_child("environment-variables")) {
      auto key_and_value = c.second.data();

      int delimiter_position = key_and_value.find('=');

      auto key = key_and_value.substr(0, delimiter_position);
      auto value = key_and_value.substr(delimiter_position + 1);

      envVarsModel.append(QString::fromStdString(key), QString::fromStdString(value));
    }
  } catch (const boost::property_tree::ptree_error& e) {
    util::warning("error when parsing the environmental variables list");

    status = false;
  }

  // command line arguments

  try {
    cmdArgsModel.reset();

    for (const auto& c : root.get_child("command-line-arguments")) {
      cmdArgsModel.append(QString::fromStdString(c.second.data()));
    }
  } catch (const boost::property_tree::ptree_error& e) {
    util::warning("error when parsing the command line arguments list");

    status = false;
  }

  // cpu

  cpuBackend.setFrequencyGovernor(root.get<std::string>("cpu.frequency-governor", cpuBackend.frequencyGovernor()));

  cpuBackend.setPcieAspmPolicy(root.get<std::string>("cpu.pcie-aspm-policy", cpuBackend.pcieAspmPolicy()));

  cpuBackend.setNiceness(root.get<int>("cpu.niceness", cpuBackend.niceness()));

  cpuBackend.setAutogroupNiceness(root.get<int>("cpu.autogroup-niceness", cpuBackend.autogroupNiceness()));

  cpuBackend.setTimerSlack(root.get<int>("cpu.timer-slack", cpuBackend.timerSlack()));

  cpuBackend.setUseSchedBatch(root.get<bool>("cpu.use-batch-scheduler", cpuBackend.useSchedBatch()));

  cpuBackend.setRealtimeWineserver(root.get<bool>("cpu.use-realtime-wineserver", cpuBackend.realtimeWineserver()));

  cpuBackend.setEnableWatchdog(root.get<bool>("cpu.enable-watchdog", cpuBackend.enableWatchdog()));

  cpuBackend.setUseCpuDmaLatency(root.get<bool>("cpu.use-cpu-dma-latency", cpuBackend.useCpuDmaLatency()));

  try {
    std::string joined_list;

    for (const auto& c : root.get_child("cpu.game-cores")) {
      joined_list.append(c.second.data() + ",");
    }

    if (!joined_list.empty()) {
      joined_list.pop_back();  // removing the "," put at the end
    }

    cpuBackend.setGameAffinity(QString::fromStdString(joined_list));

    joined_list.clear();

    for (const auto& c : root.get_child("cpu.wineserver-cores")) {
      joined_list.append(c.second.data() + ",");
    }

    if (!joined_list.empty()) {
      joined_list.pop_back();  // removing the "," put at the end
    }

    cpuBackend.setWineServerAffinity(QString::fromStdString(joined_list));

    joined_list.clear();
  } catch (const boost::property_tree::ptree_error& e) {
    util::warning("error when parsing the cpu core list");
  }

  return status;
}

}  // namespace presets