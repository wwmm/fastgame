#ifndef DISK_HPP
#define DISK_HPP

#include <gtkmm/adjustment.h>
#include <gtkmm/builder.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/grid.h>
#include <gtkmm/stack.h>
#include <gtkmm/switch.h>
#include <udisks/udisks.h>

class Disk : public Gtk::Grid {
 public:
  Disk(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
  Disk(const Disk&) = delete;
  auto operator=(const Disk&) -> Disk& = delete;
  Disk(const Disk&&) = delete;
  auto operator=(const Disk&&) -> Disk& = delete;
  ~Disk() override;

  static auto add_to_stack(Gtk::Stack* stack) -> Disk*;

  auto get_device() -> std::string;

  void set_device(const std::string& value);

  auto get_scheduler() -> std::string;

  void set_scheduler(const std::string& value);

  auto get_readahead() -> int;

  void set_readahead(const int& value);

  auto get_nr_requests() -> int;

  void set_nr_requests(const int& value);

  auto get_enable_realtime_priority() -> bool;

  void set_enable_realtime_priority(const bool& value);

  auto get_enable_add_random() -> bool;

  void set_enable_add_random(const bool& value);

  auto get_drive_id() -> std::string;

  auto get_disable_apm() -> bool;

  void set_disable_apm(const bool& value);

  auto get_supports_apm() const -> bool;

  auto get_enable_write_cache() -> bool;

  void set_enable_write_cache(const bool& value);

  auto get_supports_write_cache() const -> bool;

 private:
  std::string log_tag = "disk: ";

  Gtk::ComboBoxText *device = nullptr, *scheduler = nullptr;

  Gtk::Switch *enable_realtime_priority = nullptr, *add_random = nullptr, *disable_apm = nullptr,
              *enable_write_cache = nullptr;

  Glib::RefPtr<Gtk::Adjustment> readahead, nr_requests;

  UDisksClient* udisks_client = nullptr;

  bool supports_apm = false, supports_write_cache = false;

  std::string drive_id;

  void init_scheduler();

  void init_udisks_object();
};

#endif
