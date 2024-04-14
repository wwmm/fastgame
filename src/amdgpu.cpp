#include "amdgpu.hpp"
#include <linux/prctl.h>
#include <qobject.h>
#include <qqml.h>
#include <qstring.h>
#include <qtmetamacros.h>
#include <sys/prctl.h>
#include <vector>
#include "combobox_model.hpp"
#include "config.h"

namespace amdgpu {

Backend::Backend(QObject* parent) : QObject(parent) {
  qmlRegisterSingletonInstance<Backend>("FGAmdgpuBackend", PROJECT_VERSION_MAJOR, 0, "FGAmdgpuBackend", this);

  qmlRegisterSingletonInstance<ComboBoxModel>("FGModelPerformanceLevel0", PROJECT_VERSION_MAJOR, 0,
                                              "FGModelPerformanceLevel0", &performanceLevel0Model);

  qmlRegisterSingletonInstance<ComboBoxModel>("FGModelPerformanceLevel1", PROJECT_VERSION_MAJOR, 0,
                                              "FGModelPerformanceLevel1", &performanceLevel1Model);

  qmlRegisterSingletonInstance<ComboBoxModel>("FGModelPowerProfile0", PROJECT_VERSION_MAJOR, 0, "FGModelPowerProfile0",
                                              &powerProfile0Model);

  qmlRegisterSingletonInstance<ComboBoxModel>("FGModelPowerProfile1", PROJECT_VERSION_MAJOR, 0, "FGModelPowerProfile1",
                                              &powerProfile1Model);

  connect(this, &Backend::performanceLevel0Changed, []() {
    // auto sys_class_path =
    //     util::mounting_path_to_sys_class_path(performanceLevel0Model.getValue(_performanceLevel0).toStdString());

    // if (const auto list = util::read_system_setting(sys_class_path + "/queue/read_ahead_kb"); !list.empty()) {
    //   setPowerCap0(std::stoi(list[0]));
    // }

    // if (const auto list = util::read_system_setting(sys_class_path + "/queue/add_random"); !list.empty()) {
    //   setAddRandom(static_cast<bool>(std::stoi(list[0])));
    // } else {
    //   setAddRandom(false);
    // }

    // if (const auto list = util::read_system_setting(sys_class_path + "/queue/nr_requests"); !list.empty()) {
    //   setNrRequests(std::stoi(list[0]));
    // }

    // if (const auto list = util::read_system_setting(sys_class_path + "/queue/rq_affinity"); !list.empty()) {
    //   setRqAffinity(std::stoi(list[0]));
    // }

    // if (const auto list = util::read_system_setting(sys_class_path + "/queue/nomerges"); !list.empty()) {
    //   setNoMerges(std::stoi(list[0]));
    // }

    // if (const auto list = util::read_system_setting(sys_class_path + "/queue/wbt_lat_usec"); !list.empty()) {
    //   setWbtLatUsec(std::stoi(list[0]));
    // }
  });

  setPerformanceLevel0(0);
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

auto Backend::get_n_cards() -> int {
  return card_indices.size();
}

auto Backend::get_card_indices() -> std::vector<int> {
  return card_indices;
}

void Backend::set_performance_level(const QString& name, const int& card_index) {}

}  // namespace amdgpu
