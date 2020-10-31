#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <gtkmm/adjustment.h>
#include <gtkmm/builder.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/grid.h>
#include <gtkmm/stack.h>
#include "application.hpp"

class Memory : public Gtk::Grid {
 public:
  Memory(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, Application* application);
  Memory(const Memory&) = delete;
  auto operator=(const Memory&) -> Memory& = delete;
  Memory(const Memory&&) = delete;
  auto operator=(const Memory &&) -> Memory& = delete;
  ~Memory() override;

  static void add_to_stack(Gtk::Stack* stack, Application* app);

 private:
  std::string log_tag = "memory: ";

  Application* app = nullptr;

  Gtk::ComboBoxText *thp_enabled = nullptr, *thp_defrag = nullptr, *thp_shmem_enabled = nullptr;

  Glib::RefPtr<Gtk::Adjustment> cache_pressure;

  void read_transparent_huge_page_values();
};

#endif
