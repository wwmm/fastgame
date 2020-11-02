#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <gtkmm/adjustment.h>
#include <gtkmm/builder.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/grid.h>
#include <gtkmm/stack.h>

class Memory : public Gtk::Grid {
 public:
  Memory(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
  Memory(const Memory&) = delete;
  auto operator=(const Memory&) -> Memory& = delete;
  Memory(const Memory&&) = delete;
  auto operator=(const Memory &&) -> Memory& = delete;
  ~Memory() override;

  static auto add_to_stack(Gtk::Stack* stack) -> Memory*;

  auto get_cache_pressure() -> int;

  void set_cache_pressure(const int& value);

  auto get_thp_enabled() -> std::string;

  void set_thp_enabled(const std::string& value);

  auto get_thp_defrag() -> std::string;

  void set_thp_defrag(const std::string& value);

  auto get_thp_shmem_enabled() -> std::string;

  void set_thp_shmem_enabled(const std::string& value);

 private:
  std::string log_tag = "memory: ";

  Gtk::ComboBoxText *thp_enabled = nullptr, *thp_defrag = nullptr, *thp_shmem_enabled = nullptr;

  Glib::RefPtr<Gtk::Adjustment> cache_pressure;

  void read_transparent_huge_page_values();
};

#endif
