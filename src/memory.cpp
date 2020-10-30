#include "memory.hpp"
#include <glibmm/i18n.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "util.hpp"

namespace fs = std::filesystem;

Memory::Memory(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, Application* application)
    : Gtk::Grid(cobject), app(application) {
  // loading glade widgets

  builder->get_widget("enabled", enabled);
  builder->get_widget("defrag", defrag);
  builder->get_widget("shmem_enabled", shmem_enabled);

  power_cap = Glib::RefPtr<Gtk::Adjustment>::cast_dynamic(builder->get_object("power_cap"));

  // initializing widgets

  read_transparent_huge_page_values();

  // signals connection

  // button_add->signal_clicked().connect([=]() { liststore->append(); });
}

Memory::~Memory() {
  util::debug(log_tag + "destroyed");
}

void Memory::add_to_stack(Gtk::Stack* stack, Application* app) {
  auto builder = Gtk::Builder::create_from_resource("/com/github/wwmm/fastgame/ui/memory.glade");

  Memory* ui = nullptr;

  builder->get_widget_derived("widgets_grid", ui, app);

  stack->add(*ui, "memory", _("Memory"));
}

void Memory::read_transparent_huge_page_values() {
  // parameter: enabled

  auto [enabled_list, enabled_value] = util::read_system_file_options("/sys/kernel/mm/transparent_hugepage/enabled");

  util::debug(log_tag + "transparent huge pages state: " + enabled_value);

  for (auto& value : enabled_list) {
    enabled->append(value);
  }

  enabled->set_active_text(enabled_value);

  // parameter: defrag

  auto [defrag_list, defrag_value] = util::read_system_file_options("/sys/kernel/mm/transparent_hugepage/defrag");

  util::debug(log_tag + "transparent huge pages defrag: " + defrag_value);

  for (auto& value : defrag_list) {
    defrag->append(value);
  }

  defrag->set_active_text(defrag_value);

  // parameter: shmem_enabled

  auto [shmem_enabled_list, shmem_enabled_value] =
      util::read_system_file_options("/sys/kernel/mm/transparent_hugepage/shmem_enabled");

  util::debug(log_tag + "transparent huge pages state: " + shmem_enabled_value);

  for (auto& value : shmem_enabled_list) {
    shmem_enabled->append(value);
  }

  shmem_enabled->set_active_text(shmem_enabled_value);
}
