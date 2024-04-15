#pragma once

#include <qdebug.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <vector>
#include "combobox_model.hpp"

namespace amdgpu {

class Backend : public QObject {
  Q_OBJECT

  Q_PROPERTY(int performanceLevel0 READ performanceLevel0 WRITE setPerformanceLevel0 NOTIFY performanceLevel0Changed)

  Q_PROPERTY(int powerProfile0 READ powerProfile0 WRITE setPowerProfile0 NOTIFY powerProfile0Changed)

  Q_PROPERTY(int powerCap0 READ powerCap0 WRITE setPowerCap0 NOTIFY powerCap0Changed)

  Q_PROPERTY(int maxPowerCap0 READ maxPowerCap0 WRITE setMaxPowerCap0 NOTIFY maxPowerCap0Changed)

  Q_PROPERTY(int performanceLevel1 READ performanceLevel1 WRITE setPerformanceLevel1 NOTIFY performanceLevel1Changed)

  Q_PROPERTY(int powerProfile1 READ powerProfile1 WRITE setPowerProfile1 NOTIFY powerProfile1Changed)

  Q_PROPERTY(int powerCap1 READ powerCap1 WRITE setPowerCap1 NOTIFY powerCap1Changed)

  Q_PROPERTY(int maxPowerCap1 READ maxPowerCap1 WRITE setMaxPowerCap1 NOTIFY maxPowerCap1Changed)

 public:
  explicit Backend(QObject* parent = nullptr);

  [[nodiscard]] auto performanceLevel0() const -> int;
  [[nodiscard]] auto powerProfile0() const -> int;
  [[nodiscard]] auto powerCap0() const -> int;
  [[nodiscard]] auto maxPowerCap0() const -> int;
  [[nodiscard]] auto performanceLevel1() const -> int;
  [[nodiscard]] auto powerProfile1() const -> int;
  [[nodiscard]] auto powerCap1() const -> int;
  [[nodiscard]] auto maxPowerCap1() const -> int;

  void setPerformanceLevel0(const int& value);
  void setPowerProfile0(const int& value);
  void setPowerCap0(const int& value);
  void setMaxPowerCap0(const int& value);
  void setPerformanceLevel1(const int& value);
  void setPowerProfile1(const int& value);
  void setPowerCap1(const int& value);
  void setMaxPowerCap1(const int& value);

  auto get_n_cards() -> int;
  auto get_card_indices() -> std::vector<int>;
  void set_performance_level(const QString& name, const int& card_index);
  auto get_performance_level(const int& card_index) -> QString;
  void set_power_profile(const int& id, const int& card_index);
  auto get_power_profile(const int& card_index) -> int;
  void set_power_cap(const int& value, const int& card_index);
  auto get_power_cap(const int& card_index) -> int;

 signals:
  void performanceLevel0Changed();
  void powerProfile0Changed();
  void powerCap0Changed();
  void maxPowerCap0Changed();
  void performanceLevel1Changed();
  void powerProfile1Changed();
  void powerCap1Changed();
  void maxPowerCap1Changed();

 private:
  int _performanceLevel0 = -1;
  int _powerProfile0;
  int _powerCap0;
  int _maxPowerCap0 = 0;
  int _performanceLevel1 = -1;
  int _powerProfile1;
  int _powerCap1;
  int _maxPowerCap1 = 0;

  ComboBoxModel performanceLevel0Model;
  ComboBoxModel performanceLevel1Model;
  ComboBoxModel powerProfile0Model;
  ComboBoxModel powerProfile1Model;

  std::vector<int> card_indices;

  void read_power_cap_max(const int& card_index);
  void read_power_cap(const int& card_index);
  void read_performance_level(const int& card_index);
};

}  // namespace amdgpu