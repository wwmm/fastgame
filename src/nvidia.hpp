#pragma once

#include <qdebug.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <memory>
#include "combobox_model.hpp"

#ifdef USE_NVIDIA
#include "nvidia/nvidia.hpp"
#endif

namespace nvidia {

class Backend : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool available MEMBER _available NOTIFY availableChanged)

  Q_PROPERTY(int powermizeMode0 READ powermizeMode0 WRITE setPowermizeMode0 NOTIFY powermizeMode0Changed)

  Q_PROPERTY(int powerLimit0 READ powerLimit0 WRITE setPowerLimit0 NOTIFY powerLimit0Changed)

  Q_PROPERTY(int gpuClockOffset0 READ gpuClockOffset0 WRITE setGpuClockOffset0 NOTIFY gpuClockOffset0Changed)

  Q_PROPERTY(int gpuClockOffset0Max MEMBER _gpuClockOffset0Max NOTIFY gpuClockOffset0MaxChanged)

  Q_PROPERTY(int gpuClockOffset0Min MEMBER _gpuClockOffset0Min NOTIFY gpuClockOffset0MinChanged)

  Q_PROPERTY(
      int memoryClockOffset0 READ memoryClockOffset0 WRITE setMemoryClockOffset0 NOTIFY memoryClockOffset0Changed)

  Q_PROPERTY(int memoryClockOffset0Min MEMBER _memoryClockOffset0Min NOTIFY memoryClockOffset0MinChanged)

  Q_PROPERTY(int memoryClockOffset0Max MEMBER _memoryClockOffset0Max NOTIFY memoryClockOffset0MaxChanged)

 public:
  explicit Backend(QObject* parent = nullptr);

  [[nodiscard]] auto powermizeMode0() const -> int;
  [[nodiscard]] auto powerLimit0() const -> int;
  [[nodiscard]] auto gpuClockOffset0() const -> int;
  [[nodiscard]] auto memoryClockOffset0() const -> int;

  void setPowermizeMode0(const int& value);
  void setPowerLimit0(const int& value);
  void setGpuClockOffset0(const int& value);
  void setMemoryClockOffset0(const int& value);

  auto has_gpu() -> bool;

 signals:
  void availableChanged();
  void powermizeMode0Changed();
  void powerLimit0Changed();
  void gpuClockOffset0Changed();
  void gpuClockOffset0MaxChanged();
  void gpuClockOffset0MinChanged();
  void memoryClockOffset0Changed();
  void memoryClockOffset0MinChanged();
  void memoryClockOffset0MaxChanged();

 private:
  bool _available = true;

  int _powermizeMode0 = 0;
  int _scheduler;
  int _powerLimit0;
  int _gpuClockOffset0 = 0;
  int _gpuClockOffset0Min;
  int _gpuClockOffset0Max;
  int _memoryClockOffset0 = 0;
  int _memoryClockOffset0Min;
  int _memoryClockOffset0Max;

  ComboBoxModel powermizeMode0Model;

#ifdef USE_NVIDIA
  std::unique_ptr<nvidia_wrapper::Nvidia> nv_wrapper;
#endif
};

}  // namespace nvidia