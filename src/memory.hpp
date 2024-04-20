#pragma once

#include <qdebug.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <string>
#include "combobox_model.hpp"

namespace memory {

class Backend : public QObject {
  Q_OBJECT

  Q_PROPERTY(int thpEnabled MEMBER _thpEnabled NOTIFY thpEnabledChanged)

  Q_PROPERTY(int thpDefrag MEMBER _thpDefrag NOTIFY thpDefragChanged)

  Q_PROPERTY(int thpShmemEnabled MEMBER _thpShmemEnabled NOTIFY thpShmemEnabledChanged)

  Q_PROPERTY(int scanSleep READ scanSleep WRITE setScanSleep NOTIFY scanSleepChanged)

  Q_PROPERTY(int allocSleep READ allocSleep WRITE setAllocSleep NOTIFY allocSleepChanged)

  Q_PROPERTY(int mglruMinTtlMs READ mglruMinTtlMs WRITE setMglruMinTtlMs NOTIFY mglruMinTtlMsChanged)

  Q_PROPERTY(int swappiness READ swappiness WRITE setSwappiness NOTIFY swappinessChanged)

  Q_PROPERTY(int cachePressure READ cachePressure WRITE setCachePressure NOTIFY cachePressureChanged)

  Q_PROPERTY(int compactionProactiveness READ compactionProactiveness WRITE setCompactionProactiveness NOTIFY
                 compactionProactivenessChanged)

  Q_PROPERTY(int minFreeKbytes READ minFreeKbytes WRITE setMinFreeKbytes NOTIFY minFreeKbytesChanged)

  Q_PROPERTY(
      int pageLockUnfairness READ pageLockUnfairness WRITE setPageLockUnfairness NOTIFY pageLockUnfairnessChanged)

  Q_PROPERTY(int perCpuPagelistHighFraction READ perCpuPagelistHighFraction WRITE setPerCpuPagelistHighFraction NOTIFY
                 perCpuPagelistHighFractionChanged)

 public:
  explicit Backend(QObject* parent = nullptr);

  [[nodiscard]] auto thpEnabled() -> std::string;
  [[nodiscard]] auto thpDefrag() -> std::string;
  [[nodiscard]] auto thpShmemEnabled() -> std::string;
  [[nodiscard]] auto scanSleep() const -> int;
  [[nodiscard]] auto allocSleep() const -> int;
  [[nodiscard]] auto mglruMinTtlMs() const -> int;
  [[nodiscard]] auto swappiness() const -> int;
  [[nodiscard]] auto cachePressure() const -> int;
  [[nodiscard]] auto compactionProactiveness() const -> int;
  [[nodiscard]] auto minFreeKbytes() const -> int;
  [[nodiscard]] auto pageLockUnfairness() const -> int;
  [[nodiscard]] auto perCpuPagelistHighFraction() const -> int;

  void setThpEnabled(const std::string& value);
  void setThpDefrag(const std::string& value);
  void setThpShmemEnabled(const std::string& value);
  void setScanSleep(const int& value);
  void setAllocSleep(const int& value);
  void setMglruMinTtlMs(const int& value);
  void setSwappiness(const int& value);
  void setCachePressure(const int& value);
  void setCompactionProactiveness(const int& value);
  void setMinFreeKbytes(const int& value);
  void setPageLockUnfairness(const int& value);
  void setPerCpuPagelistHighFraction(const int& value);

 signals:
  void swappinessChanged();
  void thpEnabledChanged();
  void thpDefragChanged();
  void thpShmemEnabledChanged();
  void scanSleepChanged();
  void allocSleepChanged();
  void mglruMinTtlMsChanged();
  void cachePressureChanged();
  void compactionProactivenessChanged();
  void minFreeKbytesChanged();
  void pageLockUnfairnessChanged();
  void perCpuPagelistHighFractionChanged();

 private:
  int _thpEnabled;
  int _thpDefrag;
  int _thpShmemEnabled;
  int _scanSleep;
  int _allocSleep;
  int _mglruMinTtlMs;
  int _swappiness;
  int _cachePressure;
  int _compactionProactiveness;
  int _minFreeKbytes;
  int _pageLockUnfairness;
  int _perCpuPagelistHighFraction;

  ComboBoxModel thpEnabledModel;
  ComboBoxModel thpDefragModel;
  ComboBoxModel thpShmemEnabledModel;

  void initThp();
};

}  // namespace memory