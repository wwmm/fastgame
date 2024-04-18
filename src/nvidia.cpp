#include "nvidia.hpp"
#include <linux/prctl.h>
#include <qobject.h>
#include <qqml.h>
#include <qstring.h>
#include <qtmetamacros.h>
#include <sys/prctl.h>
#include <memory>
#include "combobox_model.hpp"
#include "config.h"

namespace nvidia {

Backend::Backend(QObject* parent) : QObject(parent) {
  qmlRegisterSingletonInstance<Backend>("FGNvidiaBackend", PROJECT_VERSION_MAJOR, 0, "FGNvidiaBackend", this);

  qmlRegisterSingletonInstance<ComboBoxModel>("FGModelPowermizeMode0", PROJECT_VERSION_MAJOR, 0,
                                              "FGModelPowermizeMode0", &powermizeMode0Model);

#ifdef USE_NVIDIA
  nv_wrapper = std::make_unique<nvidia_wrapper::Nvidia>();

  {
    auto [min, max] = nv_wrapper->get_gpu_clock_offset_range();

    _gpuClockOffset0Min = min;
    _gpuClockOffset0Max = max;

    Q_EMIT gpuClockOffset0MinChanged();
    Q_EMIT gpuClockOffset0MaxChanged();
  }

  {
    auto [min, max] = nv_wrapper->get_memory_clock_offset_range();

    _memoryClockOffset0Min = min;
    _memoryClockOffset0Max = max;

    Q_EMIT memoryClockOffset0MinChanged();
    Q_EMIT memoryClockOffset0MaxChanged();
  }

#endif

  _available = has_gpu();
  Q_EMIT availableChanged();
}

// NOLINTNEXTLINE
auto Backend::has_gpu() -> bool {
  bool found = false;

#ifdef USE_NVIDIA
  found = nv_wrapper->has_gpu();
#endif

  return found;
}

}  // namespace nvidia
