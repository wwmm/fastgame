#include "presets_manager.hpp"
#include <qabstractitemmodel.h>
#include <qbytearray.h>
#include <qfilesystemwatcher.h>
#include <qhash.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqml.h>
#include <qsortfilterproxymodel.h>
#include <qstandardpaths.h>
#include <qstring.h>
#include <qtmetamacros.h>
#include <qvariant.h>
#include <QTimer>
#include <algorithm>
#include <boost/asio/io_context.hpp>
#include <boost/process.hpp>
#include <boost/process/v1/detail/child_decl.hpp>
#include <boost/process/v1/search_path.hpp>
#include <boost/process/v2/environment.hpp>
#include <boost/process/v2/process.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include <exception>
#include <filesystem>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
#include "config.h"
#include "util.hpp"

namespace presets {

static std::filesystem::path user_presets_dir =
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

void MenuModel::remove(const QString& name) {
  auto rowIndex = list.indexOf(name);

  if (rowIndex == -1) {
    return;
  }

  beginRemoveRows(QModelIndex(), rowIndex, rowIndex);

  list.remove(rowIndex);

  endRemoveRows();

  emit dataChanged(index(0), index(list.size() - 1));
}

void MenuModel::reset() {
  beginResetModel();

  list.clear();

  endResetModel();
}

void MenuModel::begin_reset() {
  beginResetModel();
}

void MenuModel::end_reset() {
  endResetModel();
}

auto MenuModel::getList() -> QList<QString> {
  return list;
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

  qmlRegisterSingletonInstance<QSortFilterProxyModel>("FGPresetsMenuModel", VERSION_MAJOR, VERSION_MINOR,
                                                      "FGPresetsMenuModel", proxyModel);

  qmlRegisterSingletonInstance<Backend>("FGPresetsBackend", VERSION_MAJOR, VERSION_MINOR, "FGPresetsBackend", this);

  for (const auto& name : get_presets_names()) {
    menuModel.append(name);
  }

  watcher.addPath(QString::fromStdString(user_presets_dir.string()));

  connect(&watcher, &QFileSystemWatcher::directoryChanged, [&]() {
    auto model_list = menuModel.getList();
    auto folder_list = get_presets_names();

    menuModel.begin_reset();

    for (const auto& v : folder_list) {
      if (!model_list.contains(v)) {
        menuModel.append(v);
      }
    }

    for (const auto& v : model_list) {
      if (!folder_list.contains(v)) {
        menuModel.remove(v);
      }
    }

    menuModel.end_reset();
  });
}

Backend::~Backend() {
  // We remove this file so that the fastgame_apply instance knows it is time to exit

  auto file_path = std::filesystem::temp_directory_path() / std::filesystem::path{"fastgame.json"};

  std::filesystem::remove(file_path);

  util::debug("removed the file: " + file_path.string());
}

auto Backend::get_presets_names() -> QList<QString> {
  QList<QString> names;

  for (const auto& entry : std::filesystem::directory_iterator(user_presets_dir)) {
    names.emplace_back(QString::fromStdString(entry.path().stem().string()));
  }

  return names;
}

bool Backend::createPreset(const QString& name) {
  if (std::ranges::all_of(get_presets_names(), [=](auto v) { return v != name; })) {
    return savePreset(name);
  }

  return false;
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

  cpuBackend.setCpuDmaLatency(root.get<int>("cpu.cpu-dma-latency", cpuBackend.cpuDmaLatency()));

  cpuBackend.setGameAffinity(
      QString::fromStdString(root.get<std::string>("cpu.game-cores", cpuBackend.gameAffinity().toStdString())));

  cpuBackend.setWineServerAffinity(QString::fromStdString(
      root.get<std::string>("cpu.wineserver-cores", cpuBackend.wineServerAffinity().toStdString())));

  cpuBackend.setWorkqueueAffinityScope(
      root.get<std::string>("cpu.workqueue.affinity-scope", cpuBackend.workqueueAffinityScope()));

  cpuBackend.setCpuIntensiveThreshold(
      root.get<int>("cpu.workqueue.cpu-intensive-threshold", cpuBackend.cpuIntensiveThreshold()));

  cpuBackend.setSchedRuntime(root.get<double>("cpu.sched-runtime", cpuBackend.schedRuntime()));

  cpuBackend.setEnableSplitLockMitigation(
      root.get<bool>("cpu.enable-split-lock-mitigation", cpuBackend.enableSplitLockMitigation()));

  // scx_sched

  scxSchedBackend.setScheduler(root.get<std::string>("scx_sched.scheduler", scxSchedBackend.scheduler()));
  scxSchedBackend.setEnable(root.get<bool>("scx_sched.enable", scxSchedBackend.enable()));

  try {
    scxSchedModel.reset();

    for (const auto& c : root.get_child("scx_sched.arguments")) {
      scxSchedModel.append(QString::fromStdString(c.second.data()));
    }
  } catch (const boost::property_tree::ptree_error& e) {
    util::warning("error when parsing the command line arguments list");

    status = false;
  }

  // memory

  memoryBackend.setThpEnabled(
      root.get<std::string>("memory.transparent-hugepages.enabled", memoryBackend.thpEnabled()));

  memoryBackend.setThpDefrag(root.get<std::string>("memory.transparent-hugepages.defrag", memoryBackend.thpDefrag()));

  memoryBackend.setThpShmemEnabled(
      root.get<std::string>("memory.transparent-hugepages.shmem_enabled", memoryBackend.thpShmemEnabled()));

  memoryBackend.setScanSleep(root.get<int>("memory.transparent-hugepages.scan-sleep", memoryBackend.scanSleep()));

  memoryBackend.setAllocSleep(root.get<int>("memory.transparent-hugepages.alloc-sleep", memoryBackend.allocSleep()));

  memoryBackend.setOptimizeHugeTLB(
      root.get<bool>("memory.transparent-hugepages.hugetlb-optimize-vmemmap", memoryBackend.optimizeHugeTLB()));

  memoryBackend.setMglruMinTtlMs(root.get<int>("memory.mglru.min_ttl_ms", memoryBackend.mglruMinTtlMs()));

  memoryBackend.setSwappiness(root.get<int>("memory.virtual-memory.swappiness", memoryBackend.swappiness()));

  memoryBackend.setPageCluster(root.get<int>("memory.virtual-memory.page-cluster", memoryBackend.pageCluster()));

  memoryBackend.setCachePressure(root.get<int>("memory.virtual-memory.cache-pressure", memoryBackend.cachePressure()));

  memoryBackend.setCompactionProactiveness(
      root.get<int>("memory.virtual-memory.compaction-proactiveness", memoryBackend.compactionProactiveness()));

  memoryBackend.setExtfragThreshold(
      root.get<int>("memory.virtual-memory.extfrag-threshold", memoryBackend.extfragThreshold()));

  memoryBackend.setMinFreeKbytes(root.get<int>("memory.virtual-memory.min-free-kbytes", memoryBackend.minFreeKbytes()));

  memoryBackend.setMinUnmappedRatio(
      root.get<int>("memory.virtual-memory.min-unmapped-ratio", memoryBackend.minUnmappedRatio()));

  memoryBackend.setMinSlabRatio(root.get<int>("memory.virtual-memory.min-slab-ratio", memoryBackend.minSlabRatio()));

  memoryBackend.setPageLockUnfairness(
      root.get<int>("memory.virtual-memory.page-lock-unfairness", memoryBackend.pageLockUnfairness()));

  memoryBackend.setPerCpuPagelistHighFraction(
      root.get<int>("memory.virtual-memory.percpu-pagelist-high-fraction", memoryBackend.perCpuPagelistHighFraction()));

  memoryBackend.setZoneReclaimMode(
      root.get<int>("memory.virtual-memory.zone-reclaim-mode", memoryBackend.zoneReclaimMode()));

  // disk

  diskBackend.setMountingPath(root.get<std::string>("disk.mounting-path", diskBackend.mountingPath()));

  diskBackend.setScheduler(root.get<std::string>("disk.scheduler", diskBackend.scheduler()));

  diskBackend.setReadahead(root.get<int>("disk.readahead", diskBackend.readahead()));

  diskBackend.setNrRequests(root.get<int>("disk.nr-requests", diskBackend.nrRequests()));

  diskBackend.setRqAffinity(root.get<int>("disk.rq-affinity", diskBackend.rqAffinity()));

  diskBackend.setNoMerges(root.get<int>("disk.nomerges", diskBackend.noMerges()));

  diskBackend.setWbtLatUsec(root.get<int>("disk.wbt-lat-usec", diskBackend.wbtLatUsec()));

  diskBackend.setEnableRealtimePriority(
      root.get<bool>("disk.enable-realtime-priority", diskBackend.enableRealtimePriority()));

  diskBackend.setAddRandom(root.get<bool>("disk.add_random", diskBackend.addRandom()));

  // amdgpu

  if (amdgpuBackend.get_n_cards() > 0) {
    auto card_indices = amdgpuBackend.get_card_indices();

    auto card0 = card_indices[0];

    amdgpuBackend.set_performance_level(
        root.get<std::string>("amdgpu.performance-level", amdgpuBackend.get_performance_level(card0)), card0);

    amdgpuBackend.set_power_profile(root.get<int>("amdgpu.power-profile", amdgpuBackend.get_power_profile(card0)),
                                    card0);

    amdgpuBackend.set_power_cap(root.get<int>("amdgpu.power-cap", amdgpuBackend.get_power_cap(card0)), card0);

    if (amdgpuBackend.get_n_cards() >= 2) {
      auto card1 = card_indices[1];

      amdgpuBackend.set_performance_level(
          root.get<std::string>("amdgpu.card1.performance-level", amdgpuBackend.get_performance_level(card1)), card1);

      amdgpuBackend.set_power_profile(
          root.get<int>("amdgpu.card1.power-profile", amdgpuBackend.get_power_profile(card1)), card1);

      amdgpuBackend.set_power_cap(root.get<int>("amdgpu.card1.power-cap", amdgpuBackend.get_power_cap(card1)), card1);
    }
  }

  // nvidia

#ifdef USE_NVIDIA

  if (nvidiaBackend.has_gpu()) {
    nvidiaBackend.setPowermizeMode0(root.get<int>("nvidia.powermize-mode", nvidiaBackend.powermizeMode0()));

    nvidiaBackend.setPowerLimit0(root.get<int>("nvidia.power-limit", nvidiaBackend.powerLimit0()));

    nvidiaBackend.setGpuClockOffset0(root.get<int>("nvidia.clock-offset.gpu", nvidiaBackend.gpuClockOffset0()));

    nvidiaBackend.setMemoryClockOffset0(
        root.get<int>("nvidia.clock-offset.memory", nvidiaBackend.memoryClockOffset0()));
  }

#endif

  return status;
}

bool Backend::save_preset(const QString& name, const std::filesystem::path& output_path) {
  boost::property_tree::ptree root;
  boost::property_tree::ptree node;

  // game executable

  root.put("game-executable", _executableName.toStdString());

  // environment variables

  for (const auto& p : envVarsModel.getList()) {
    boost::property_tree::ptree local_node;

    local_node.put("", p.first.toStdString() + "=" + p.second.toStdString());

    node.push_back(std::make_pair("", local_node));
  }

  root.add_child("environment-variables", node);

  // command line arguments

  node.clear();

  for (const auto& v : cmdArgsModel.getList()) {
    boost::property_tree::ptree local_node;

    local_node.put("", v.toStdString());

    node.push_back(std::make_pair("", local_node));
  }

  root.add_child("command-line-arguments", node);

  // cpu

  root.put("cpu.use-batch-scheduler", cpuBackend.useSchedBatch());
  root.put("cpu.frequency-governor", cpuBackend.frequencyGovernor());
  root.put("cpu.pcie-aspm-policy", cpuBackend.pcieAspmPolicy());
  root.put("cpu.use-cpu-dma-latency", cpuBackend.useCpuDmaLatency());
  root.put("cpu.cpu-dma-latency", cpuBackend.cpuDmaLatency());
  root.put("cpu.use-realtime-wineserver", cpuBackend.realtimeWineserver());
  root.put("cpu.enable-watchdog", cpuBackend.enableWatchdog());
  root.put("cpu.niceness", cpuBackend.niceness());
  root.put("cpu.autogroup-niceness", cpuBackend.autogroupNiceness());
  root.put("cpu.timer-slack", cpuBackend.timerSlack());
  root.put("cpu.game-cores", cpuBackend.gameAffinity().toStdString());
  root.put("cpu.wineserver-cores", cpuBackend.wineServerAffinity().toStdString());
  root.put("cpu.workqueue.affinity-scope", cpuBackend.workqueueAffinityScope());
  root.put("cpu.workqueue.cpu-intensive-threshold", cpuBackend.cpuIntensiveThreshold());
  root.put("cpu.sched-runtime", cpuBackend.schedRuntime());
  root.put("cpu.enable-split-lock-mitigation", cpuBackend.enableSplitLockMitigation());

  // scx_sched

  root.put("scx_sched.enable", scxSchedBackend.enable());
  root.put("scx_sched.scheduler", scxSchedBackend.scheduler());

  node.clear();

  for (const auto& v : scxSchedModel.getList()) {
    boost::property_tree::ptree local_node;

    local_node.put("", v.toStdString());

    node.push_back(std::make_pair("", local_node));
  }

  root.add_child("scx_sched.arguments", node);

  // memory

  root.put("memory.virtual-memory.swappiness", memoryBackend.swappiness());
  root.put("memory.virtual-memory.page-cluster", memoryBackend.pageCluster());
  root.put("memory.virtual-memory.cache-pressure", memoryBackend.cachePressure());
  root.put("memory.virtual-memory.compaction-proactiveness", memoryBackend.compactionProactiveness());
  root.put("memory.virtual-memory.extfrag-threshold", memoryBackend.extfragThreshold());
  root.put("memory.virtual-memory.min-free-kbytes", memoryBackend.minFreeKbytes());
  root.put("memory.virtual-memory.min-unmapped-ratio", memoryBackend.minUnmappedRatio());
  root.put("memory.virtual-memory.min-slab-ratio", memoryBackend.minSlabRatio());
  root.put("memory.virtual-memory.page-lock-unfairness", memoryBackend.pageLockUnfairness());
  root.put("memory.virtual-memory.percpu-pagelist-high-fraction", memoryBackend.perCpuPagelistHighFraction());
  root.put("memory.virtual-memory.zone-reclaim-mode", memoryBackend.zoneReclaimMode());

  root.put("memory.mglru.min_ttl_ms", memoryBackend.mglruMinTtlMs());

  root.put("memory.transparent-hugepages.enabled", memoryBackend.thpEnabled());
  root.put("memory.transparent-hugepages.defrag", memoryBackend.thpDefrag());
  root.put("memory.transparent-hugepages.shmem_enabled", memoryBackend.thpShmemEnabled());
  root.put("memory.transparent-hugepages.scan-sleep", memoryBackend.scanSleep());
  root.put("memory.transparent-hugepages.alloc-sleep", memoryBackend.allocSleep());
  root.put("memory.transparent-hugepages.hugetlb-optimize-vmemmap", memoryBackend.optimizeHugeTLB());

  // disk

  root.put("disk.mounting-path", diskBackend.mountingPath());
  root.put("disk.scheduler", diskBackend.scheduler());
  root.put("disk.enable-realtime-priority", diskBackend.enableRealtimePriority());
  root.put("disk.readahead", diskBackend.readahead());
  root.put("disk.nr-requests", diskBackend.nrRequests());
  root.put("disk.rq-affinity", diskBackend.rqAffinity());
  root.put("disk.nomerges", diskBackend.noMerges());
  root.put("disk.wbt-lat-usec", diskBackend.wbtLatUsec());
  root.put("disk.add_random", diskBackend.addRandom());

  // amdgpu

  if (amdgpuBackend.get_n_cards() > 0) {
    auto card_indices = amdgpuBackend.get_card_indices();

    auto card0 = card_indices[0];

    root.put("amdgpu.performance-level", amdgpuBackend.get_performance_level(card0));
    root.put("amdgpu.power-profile", amdgpuBackend.get_power_profile(card0));
    root.put("amdgpu.power-cap", amdgpuBackend.get_power_cap(card0));

    if (amdgpuBackend.get_n_cards() >= 2) {
      auto card1 = card_indices[1];

      root.put("amdgpu.card1.performance-level", amdgpuBackend.get_performance_level(card1));
      root.put("amdgpu.card1.power-profile", amdgpuBackend.get_power_profile(card1));
      root.put("amdgpu.card1.power-cap", amdgpuBackend.get_power_cap(card1));
    }
  }

  // nvidia

#ifdef USE_NVIDIA

  if (nvidiaBackend.has_gpu()) {
    root.put("nvidia.powermize-mode", nvidiaBackend.powermizeMode0());
    root.put("nvidia.clock-offset.gpu", nvidiaBackend.gpuClockOffset0());
    root.put("nvidia.clock-offset.memory", nvidiaBackend.memoryClockOffset0());
    root.put("nvidia.power-limit", nvidiaBackend.powerLimit0());
  }

#endif

  // saving the properties to a file

  auto output_file = output_path / std::filesystem::path{name.toStdString() + ".json"};

  boost::property_tree::write_json(output_file, root);

  util::debug("saved preset: " + output_file.string());

  return true;
}

bool Backend::savePreset(const QString& name) {
  return save_preset(name, user_presets_dir);
}

bool Backend::removePreset(const QString& name) {
  auto file_path = user_presets_dir / std::filesystem::path{name.toStdString() + ".json"};

  if (std::filesystem::remove(file_path)) {
    util::debug("removed: " + file_path.string());

    return true;
  } else {
    util::debug("could not remove: " + file_path.string());

    return false;
  }
}

bool Backend::importPresets(const QList<QString>& url_list) {
  return std::ranges::all_of(url_list, [](auto u) {
    auto url = QUrl(u);

    if (url.isLocalFile()) {
      auto input_path = std::filesystem::path{url.toLocalFile().toStdString()};

      auto output_path = user_presets_dir / std::filesystem::path{input_path.filename()};

      if (std::filesystem::copy_file(input_path, output_path, std::filesystem::copy_options::overwrite_existing)) {
        util::debug("imported preset to: " + output_path.string());
      } else {
        util::warning("could not copy the file " + input_path.string() + " to our preset folder");

        return false;
      }

      return true;
    }

    util::warning("could not copy " + u.toStdString() + " to our preset folder");

    return false;
  });
}

bool Backend::applySettings() {
  // First we remove the file in case a server instance is already running. This will make it exit.

  auto file_path = std::filesystem::temp_directory_path() / std::filesystem::path{"fastgame.json"};

  std::filesystem::remove(file_path);

  util::debug("removed the file: " + file_path.string());

  /*
    After a timout of 3 seconds we launch the new server. In case a server instance is already running this should
    give it enough time to exit.
  */

  QTimer::singleShot(3000, this, [this]() {
    save_preset("fastgame", std::filesystem::temp_directory_path());

    if (process_apply_settings != nullptr && process_apply_settings->running()) {
      process_apply_settings->request_exit();

      delete process_apply_settings;

      process_apply_settings = nullptr;
    }

    try {
      boost::asio::io_context ctx;

      auto exe = boost::process::environment::find_executable("pkexec");

      process_apply_settings = new boost::process::process(ctx, exe, {"fastgame_apply"});

      Q_EMIT settingsApplied();
    } catch (std::exception& e) {
      Q_EMIT settingsApplied();

      util::warning(e.what());
    }
  });

  return true;
}

}  // namespace presets