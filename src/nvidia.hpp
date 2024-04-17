#pragma once

#include <qdebug.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include "combobox_model.hpp"

#ifdef USE_NVIDIA
#include "nvidia/nvidia.hpp"
#endif

namespace nvidia {

class Backend : public QObject {
  Q_OBJECT

  Q_PROPERTY(int powermizeMode0 MEMBER _powermizeMode0 NOTIFY powermizeMode0Changed)

  Q_PROPERTY(int powerLimit MEMBER _powerLimit NOTIFY powerLimitChanged)

  Q_PROPERTY(int gpuClockOffset0 MEMBER _gpuClockOffset0 NOTIFY gpuClockOffset0Changed)

  Q_PROPERTY(int gpuClockOffset0Max MEMBER _gpuClockOffset0Max NOTIFY gpuClockOffset0MaxChanged)

  Q_PROPERTY(int gpuClockOffset0Min MEMBER _gpuClockOffset0Min NOTIFY gpuClockOffset0MinChanged)

  Q_PROPERTY(int memoryClockOffset0 MEMBER _memoryClockOffset0 NOTIFY memoryClockOffset0Changed)

  Q_PROPERTY(int memoryClockOffset0Min MEMBER _memoryClockOffset0Min NOTIFY memoryClockOffset0MinChanged)

  Q_PROPERTY(int memoryClockOffset0Max MEMBER _memoryClockOffset0Max NOTIFY memoryClockOffset0MaxChanged)

 public:
  explicit Backend(QObject* parent = nullptr);

 signals:
  void powermizeMode0Changed();
  void powerLimitChanged();
  void gpuClockOffset0Changed();
  void gpuClockOffset0MaxChanged();
  void gpuClockOffset0MinChanged();
  void memoryClockOffset0Changed();
  void memoryClockOffset0MinChanged();
  void memoryClockOffset0MaxChanged();

 private:
  int _powermizeMode0 = -1;
  int _scheduler;
  int _powerLimit;
  int _gpuClockOffset0;
  int _gpuClockOffset0Min;
  int _gpuClockOffset0Max;
  int _memoryClockOffset0;
  int _memoryClockOffset0Min;
  int _memoryClockOffset0Max;

  ComboBoxModel powermizeMode0Model;

#ifdef USE_NVIDIA
  std::unique_ptr<nvidia_wrapper::Nvidia> nv_wrapper;
#endif

  auto has_gpu() -> bool;
};

}  // namespace nvidia