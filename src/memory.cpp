#include "memory.hpp"
#include <glibmm/i18n.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "util.hpp"

namespace fs = std::filesystem;

Memory::Memory(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::Grid(cobject) {
  // loading glade widgets

  builder->get_widget("thp_enabled", thp_enabled);
  builder->get_widget("thp_defrag", thp_defrag);
  builder->get_widget("thp_shmem_enabled", thp_shmem_enabled);

  cache_pressure = Glib::RefPtr<Gtk::Adjustment>::cast_dynamic(builder->get_object("cache_pressure"));

  // initializing widgets

  cache_pressure->set_value(std::stoi(util::read_system_setting("/proc/sys/vm/vfs_cache_pressure")[0]));

  read_transparent_huge_page_values();
}

Memory::~Memory() {
  util::debug(log_tag + "destroyed");
}

auto Memory::add_to_stack(Gtk::Stack* stack) -> Memory* {
  auto builder = Gtk::Builder::create_from_resource("/com/github/wwmm/fastgame/ui/memory.glade");

  Memory* ui = nullptr;

  builder->get_widget_derived("widgets_grid", ui);

  stack->add(*ui, "memory", _("Memory"));

  return ui;
}

void Memory::read_transparent_huge_page_values() {
  // parameter: enabled

  auto enabled_list = util::read_system_setting("/sys/kernel/mm/transparent_hugepage/enabled");

  auto enabled_value = util::get_selected_value(enabled_list);

  util::debug(log_tag + "transparent huge pages state: " + enabled_value);

  for (auto& value : enabled_list) {
    if (value.find('[') != std::string::npos) {
      value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
    }

    thp_enabled->append(value);
  }

  thp_enabled->set_active_text(enabled_value);

  // parameter: defrag

  auto defrag_list = util::read_system_setting("/sys/kernel/mm/transparent_hugepage/defrag");

  auto defrag_value = util::get_selected_value(defrag_list);

  util::debug(log_tag + "transparent huge pages defrag: " + defrag_value);

  for (auto& value : defrag_list) {
    if (value.find('[') != std::string::npos) {
      value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
    }

    thp_defrag->append(value);
  }

  thp_defrag->set_active_text(defrag_value);

  // parameter: shmem_enabled

  auto shmem_enabled_list = util::read_system_setting("/sys/kernel/mm/transparent_hugepage/shmem_enabled");

  auto shmem_enabled_value = util::get_selected_value(shmem_enabled_list);

  util::debug(log_tag + "transparent huge pages state: " + shmem_enabled_value);

  for (auto& value : shmem_enabled_list) {
    if (value.find('[') != std::string::npos) {
      value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
    }

    thp_shmem_enabled->append(value);
  }

  thp_shmem_enabled->set_active_text(shmem_enabled_value);
}

auto Memory::get_cache_pressure() -> int {
  return static_cast<int>(cache_pressure->get_value());
}

void Memory::set_cache_pressure(const int& value) {
  cache_pressure->set_value(value);
}

auto Memory::get_thp_enabled() -> std::string {
  return thp_enabled->get_active_text();
}

void Memory::set_thp_enabled(const std::string& value) {
  thp_enabled->set_active_text(value);
}

auto Memory::get_thp_defrag() -> std::string {
  return thp_defrag->get_active_text();
}

void Memory::set_thp_defrag(const std::string& value) {
  thp_defrag->set_active_text(value);
}

auto Memory::get_thp_shmem_enabled() -> std::string {
  return thp_shmem_enabled->get_active_text();
}

void Memory::set_thp_shmem_enabled(const std::string& value) {
  thp_shmem_enabled->set_active_text(value);
}