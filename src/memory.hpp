#pragma once

#include <qdebug.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <string>
#include "combobox_model.hpp"

namespace memory {

class Backend : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool optimizeHugeTLB READ optimizeHugeTLB WRITE setOptimizeHugeTLB NOTIFY optimizeHugeTLBChanged)

  Q_PROPERTY(int thpEnabled MEMBER _thpEnabled NOTIFY thpEnabledChanged)

  Q_PROPERTY(int thpDefrag MEMBER _thpDefrag NOTIFY thpDefragChanged)

  Q_PROPERTY(int thpShmemEnabled MEMBER _thpShmemEnabled NOTIFY thpShmemEnabledChanged)

  Q_PROPERTY(int scanSleep READ scanSleep WRITE setScanSleep NOTIFY scanSleepChanged)

  Q_PROPERTY(int allocSleep READ allocSleep WRITE setAllocSleep NOTIFY allocSleepChanged)

  Q_PROPERTY(int mglruMinTtlMs READ mglruMinTtlMs WRITE setMglruMinTtlMs NOTIFY mglruMinTtlMsChanged)

  Q_PROPERTY(int swappiness READ swappiness WRITE setSwappiness NOTIFY swappinessChanged)

  Q_PROPERTY(int pageCluster READ pageCluster WRITE setPageCluster NOTIFY pageClusterChanged)

  Q_PROPERTY(int cachePressure READ cachePressure WRITE setCachePressure NOTIFY cachePressureChanged)

  Q_PROPERTY(int compactionProactiveness READ compactionProactiveness WRITE setCompactionProactiveness NOTIFY
                 compactionProactivenessChanged)

  Q_PROPERTY(int extfragThreshold READ extfragThreshold WRITE setExtfragThreshold NOTIFY extfragThresholdChanged)

  Q_PROPERTY(int minFreeKbytes READ minFreeKbytes WRITE setMinFreeKbytes NOTIFY minFreeKbytesChanged)

  Q_PROPERTY(int minUnmappedRatio READ minUnmappedRatio WRITE setMinUnmappedRatio NOTIFY minUnmappedRatioChanged)

  Q_PROPERTY(int minSlabRatio READ minSlabRatio WRITE setMinSlabRatio NOTIFY minSlabRatioChanged)

  Q_PROPERTY(
      int pageLockUnfairness READ pageLockUnfairness WRITE setPageLockUnfairness NOTIFY pageLockUnfairnessChanged)

  Q_PROPERTY(int perCpuPagelistHighFraction READ perCpuPagelistHighFraction WRITE setPerCpuPagelistHighFraction NOTIFY
                 perCpuPagelistHighFractionChanged)

  Q_PROPERTY(int zoneReclaimMode MEMBER _zoneReclaimMode NOTIFY zoneReclaimModeChanged)

 public:
  explicit Backend(QObject* parent = nullptr);

  [[nodiscard]] auto optimizeHugeTLB() const -> bool;
  [[nodiscard]] auto thpEnabled() -> std::string;
  [[nodiscard]] auto thpDefrag() -> std::string;
  [[nodiscard]] auto thpShmemEnabled() -> std::string;
  [[nodiscard]] auto scanSleep() const -> int;
  [[nodiscard]] auto allocSleep() const -> int;
  [[nodiscard]] auto mglruMinTtlMs() const -> int;
  [[nodiscard]] auto swappiness() const -> int;
  [[nodiscard]] auto pageCluster() const -> int;
  [[nodiscard]] auto cachePressure() const -> int;
  [[nodiscard]] auto compactionProactiveness() const -> int;
  [[nodiscard]] auto extfragThreshold() const -> int;
  [[nodiscard]] auto minFreeKbytes() const -> int;
  [[nodiscard]] auto minUnmappedRatio() const -> int;
  [[nodiscard]] auto minSlabRatio() const -> int;
  [[nodiscard]] auto pageLockUnfairness() const -> int;
  [[nodiscard]] auto perCpuPagelistHighFraction() const -> int;
  [[nodiscard]] auto zoneReclaimMode() const -> int;

  void setOptimizeHugeTLB(const bool& value);
  void setThpEnabled(const std::string& value);
  void setThpDefrag(const std::string& value);
  void setThpShmemEnabled(const std::string& value);
  void setScanSleep(const int& value);
  void setAllocSleep(const int& value);
  void setMglruMinTtlMs(const int& value);
  void setSwappiness(const int& value);
  void setPageCluster(const int& value);
  void setCachePressure(const int& value);
  void setCompactionProactiveness(const int& value);
  void setExtfragThreshold(const int& value);
  void setMinFreeKbytes(const int& value);
  void setMinUnmappedRatio(const int& value);
  void setMinSlabRatio(const int& value);
  void setPageLockUnfairness(const int& value);
  void setPerCpuPagelistHighFraction(const int& value);
  void setZoneReclaimMode(const int& value);

 signals:
  void optimizeHugeTLBChanged();
  void swappinessChanged();
  void pageClusterChanged();
  void thpEnabledChanged();
  void thpDefragChanged();
  void thpShmemEnabledChanged();
  void scanSleepChanged();
  void allocSleepChanged();
  void mglruMinTtlMsChanged();
  void cachePressureChanged();
  void compactionProactivenessChanged();
  void extfragThresholdChanged();
  void minFreeKbytesChanged();
  void minUnmappedRatioChanged();
  void minSlabRatioChanged();
  void pageLockUnfairnessChanged();
  void perCpuPagelistHighFractionChanged();
  void zoneReclaimModeChanged();

 private:
  bool _optimizeHugeTLB;
  int _thpEnabled;
  int _thpDefrag;
  int _thpShmemEnabled;
  int _scanSleep;
  int _allocSleep;
  int _mglruMinTtlMs;
  int _swappiness;
  int _pageCluster;
  int _cachePressure;
  int _compactionProactiveness;
  int _extfragThreshold;
  int _minFreeKbytes;
  int _minUnmappedRatio;
  int _minSlabRatio;
  int _pageLockUnfairness;
  int _perCpuPagelistHighFraction;
  int _zoneReclaimMode;

  ComboBoxModel thpEnabledModel;
  ComboBoxModel thpDefragModel;
  ComboBoxModel thpShmemEnabledModel;
  ComboBoxModel zoneReclaimModeModel;

  void initThp();
  void init_vm();
};

}  // namespace memory