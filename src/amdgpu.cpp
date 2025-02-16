#include "amdgpu.hpp"
#include <linux/prctl.h>
#include <qobject.h>
#include <qqml.h>
#include <qstring.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <filesystem>
#include <fstream>
#include <ios>
#include <sstream>
#include <string>
#include <vector>
#include "combobox_model.hpp"
#include "config.h"
#include "util.hpp"

namespace amdgpu {

Backend::Backend(QObject* parent) : QObject(parent) {
  qmlRegisterSingletonInstance<Backend>("FGAmdgpuBackend", VERSION_MAJOR, VERSION_MINOR, "FGAmdgpuBackend", this);

  qmlRegisterSingletonInstance<ComboBoxModel>("FGModelPerformanceLevel0", VERSION_MAJOR, VERSION_MINOR,
                                              "FGModelPerformanceLevel0", &performanceLevel0Model);

  qmlRegisterSingletonInstance<ComboBoxModel>("FGModelPerformanceLevel1", VERSION_MAJOR, VERSION_MINOR,
                                              "FGModelPerformanceLevel1", &performanceLevel1Model);

  qmlRegisterSingletonInstance<ComboBoxModel>("FGModelPowerProfile0", VERSION_MAJOR, VERSION_MINOR,
                                              "FGModelPowerProfile0", &powerProfile0Model);

  qmlRegisterSingletonInstance<ComboBoxModel>("FGModelPowerProfile1", VERSION_MAJOR, VERSION_MINOR,
                                              "FGModelPowerProfile1", &powerProfile1Model);

  card_indices = util::get_amdgpu_indices();

  util::debug("number of amdgpu cards: " + util::to_string(card_indices.size()));

  _available = !card_indices.empty();
  Q_EMIT availableChanged();

  /*
    Hybrid cards may be turned off at the moment we try to get information. So we open the corresponding dri device
    just to make sure they are active
  */

  auto boot_vga_idx = util::find_boot_vga(card_indices);

  for (auto n : card_indices) {
    auto is_boot_vga = boot_vga_idx == n;

    int fd = -1;

    if (!is_boot_vga) {
      fd = util::open_dri_device(n);
    }

    auto* model = (n == card_indices.front()) ? &performanceLevel0Model : &performanceLevel1Model;

    model->append("auto");
    model->append("low");
    model->append("high");
    model->append("manual");
    model->append("profile_standard");
    model->append("profile_min_sclk");
    model->append("profile_min_mclk");
    model->append("profile_peak");

    read_power_cap_max(n);
    read_power_cap(n);
    read_performance_level(n);
    read_power_profile(n);

    util::close_dri_device(fd);
  }
}

auto Backend::performanceLevel0() const -> int {
  return _performanceLevel0;
}

void Backend::setPerformanceLevel0(const int& value) {
  _performanceLevel0 = value;

  Q_EMIT performanceLevel0Changed();
}

auto Backend::powerProfile0() const -> int {
  return _powerProfile0;
}

void Backend::setPowerProfile0(const int& value) {
  _powerProfile0 = value;

  Q_EMIT powerProfile0Changed();
}

auto Backend::powerCap0() const -> int {
  return _powerCap0;
}

void Backend::setPowerCap0(const int& value) {
  _powerCap0 = value;

  Q_EMIT powerCap0Changed();
}

auto Backend::maxPowerCap0() const -> int {
  return _maxPowerCap0;
}

void Backend::setMaxPowerCap0(const int& value) {
  _maxPowerCap0 = value;

  Q_EMIT maxPowerCap0Changed();
}

auto Backend::performanceLevel1() const -> int {
  return _performanceLevel1;
}

void Backend::setPerformanceLevel1(const int& value) {
  _performanceLevel1 = value;

  Q_EMIT performanceLevel1Changed();
}

auto Backend::powerProfile1() const -> int {
  return _powerProfile1;
}

void Backend::setPowerProfile1(const int& value) {
  _powerProfile1 = value;

  Q_EMIT powerProfile1Changed();
}

auto Backend::powerCap1() const -> int {
  return _powerCap1;
}

void Backend::setPowerCap1(const int& value) {
  _powerCap1 = value;

  Q_EMIT powerCap1Changed();
}

auto Backend::maxPowerCap1() const -> int {
  return _maxPowerCap1;
}

void Backend::setMaxPowerCap1(const int& value) {
  _maxPowerCap1 = value;

  Q_EMIT maxPowerCap1Changed();
}

auto Backend::get_n_cards() -> int {
  return card_indices.size();
}

auto Backend::get_card_indices() -> std::vector<int> {
  return card_indices;
}

void Backend::set_performance_level(const std::string& name, const int& card_index) {
  if (card_index == card_indices.front()) {
    auto id = performanceLevel0Model.getId(QString::fromStdString(name));

    if (id != -1) {
      setPerformanceLevel0(id);
    }
  } else {
    auto id = performanceLevel1Model.getId(QString::fromStdString(name));

    if (id != -1) {
      setPerformanceLevel1(id);
    }
  }
}

auto Backend::get_performance_level(const int& card_index) -> std::string {
  if (card_index == card_indices.front()) {
    return performanceLevel0Model.getValue(_performanceLevel0).toStdString();
  } else {
    return performanceLevel1Model.getValue(_performanceLevel1).toStdString();
  }
}

void Backend::set_power_profile(const int& id, const int& card_index) {
  if (card_index == card_indices.front()) {
    setPowerProfile0(id);
  } else {
    setPowerProfile1(id);
  }
}

auto Backend::get_power_profile(const int& card_index) -> int {
  if (card_index == card_indices.front()) {
    return _powerProfile0;
  } else {
    return _powerProfile1;
  }
}

void Backend::set_power_cap(const int& value, const int& card_index) {
  if (card_index == card_indices.front()) {
    setPowerCap0(value);
  } else {
    setPowerCap1(value);
  }
}

auto Backend::get_power_cap(const int& card_index) -> int {
  if (card_index == card_indices.front()) {
    return _powerCap0;
  } else {
    return _powerCap1;
  }
}

void Backend::read_power_cap_max(const int& card_index) {
  auto hwmon_index = util::find_hwmon_index(card_index);

  auto path = std::filesystem::path("/sys/class/drm/card" + util::to_string(card_index) + "/device/hwmon/hwmon" +
                                    util::to_string(hwmon_index) + "/power1_cap_max");

  if (!std::filesystem::is_regular_file(path)) {
    util::debug("file " + path.string() + " does not exist!");
    util::debug("card " + util::to_string(card_index) + " could not read the maximum power cap!");
  } else {
    std::ifstream f;

    f.open(path, std::ios::in);

    int raw_value = 0;  // microWatts

    f >> raw_value;

    f.close();

    int power_cap_in_watts = raw_value / 1000000;

    if (card_index == card_indices.front()) {
      setMaxPowerCap0(power_cap_in_watts);
    } else {
      setMaxPowerCap1(power_cap_in_watts);
    }

    util::debug("card " + util::to_string(card_index) +
                " maximum allowed power cap: " + util::to_string(power_cap_in_watts) + " W");
  }
}

void Backend::read_power_cap(const int& card_index) {
  auto hwmon_index = util::find_hwmon_index(card_index);

  auto path = std::filesystem::path("/sys/class/drm/card" + util::to_string(card_index) + "/device/hwmon/hwmon" +
                                    util::to_string(hwmon_index) + "/power1_cap");

  if (!std::filesystem::is_regular_file(path)) {
    util::debug("file " + path.string() + " does not exist!");
    util::debug("card " + util::to_string(card_index) + " can not change the power cap!");
  } else {
    std::ifstream f;

    f.open(path, std::ios::in);

    int raw_value = 0;  // microWatts

    f >> raw_value;

    f.close();

    int power_cap_in_watts = raw_value / 1000000;

    if (card_index == card_indices.front()) {
      setPowerCap0(power_cap_in_watts);
    } else {
      setPowerCap1(power_cap_in_watts);
    }

    util::debug("card " + util::to_string(card_index) + " current power cap: " + util::to_string(power_cap_in_watts) +
                " W");
  }
}

void Backend::read_performance_level(const int& card_index) {
  auto path = std::filesystem::path("/sys/class/drm/card" + util::to_string(card_index) +
                                    "/device/power_dpm_force_performance_level");

  if (!std::filesystem::is_regular_file(path)) {
    util::debug("file " + path.string() + " does not exist!");
    util::debug("card " + util::to_string(card_index) + " could not read the power profile!");
  } else {
    std::ifstream f;

    f.open(path, std::ios::in);

    std::string level;

    f >> level;

    f.close();

    set_performance_level(level, card_index);

    util::debug("card " + util::to_string(card_index) + " current performance level: " + level);
  }
}

void Backend::read_power_profile(const int& card_index) {
  auto path =
      std::filesystem::path("/sys/class/drm/card" + util::to_string(card_index) + "/device/pp_power_profile_mode");

  if (!std::filesystem::is_regular_file(path)) {
    util::debug("file " + path.string() + " does not exist!");
    util::debug("card " + util::to_string(card_index) + " can not change the performance level!");
  } else {
    std::ifstream f;

    f.open(path, std::ios::in);

    std::stringstream buffer;

    buffer << f.rdbuf();

    f.close();

    if (card_index == card_indices.front()) {
      powerProfile0Model.reset();
    } else {
      powerProfile1Model.reset();
    }

    std::string line;

    std::getline(buffer, line);  // Discarding the header

    while (std::getline(buffer, line)) {
      if (line.ends_with(":")) {
        line.pop_back();

        std::istringstream ss(line);

        uint count = 0;

        std::string word;
        std::string profile;
        std::string id_str;
        bool is_selected = false;

        while (ss >> word) {
          if (word.ends_with("*")) {
            word.pop_back();

            util::debug("card " + util::to_string(card_index) + " current power profile: " + word);

            is_selected = true;
          }

          if (count == 0) {
            id_str = word;
          }

          if (count == 1) {
            profile = word;
          }

          count++;
        }

        // util::info(id + " -> " + profile);

        if (card_index == card_indices.front()) {
          powerProfile0Model.append(QString::fromStdString(profile));
        } else {
          powerProfile1Model.append(QString::fromStdString(profile));
        }

        if (is_selected) {
          uint id = 0;

          util::str_to_num(id_str, id);

          if (card_index == card_indices.front()) {
            setPowerProfile0(id);
          } else {
            setPowerProfile1(id);
          }
        }
      }
    }
  }
}

}  // namespace amdgpu
