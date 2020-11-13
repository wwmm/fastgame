#ifndef DISK_HPP
#define DISK_HPP

#include <gtkmm/adjustment.h>
#include <gtkmm/builder.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/grid.h>
#include <gtkmm/stack.h>
#include <gtkmm/switch.h>

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

 private:
  std::string log_tag = "disk: ";

  Gtk::ComboBoxText *device = nullptr, *scheduler = nullptr;

  Gtk::Switch *enable_realtime_priority = nullptr, *add_random = nullptr;

  Glib::RefPtr<Gtk::Adjustment> readahead, nr_requests;

  void init_scheduler();
};

#endif
