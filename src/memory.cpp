#include "memory.hpp"

namespace ui::memory {

using namespace std::string_literals;

auto constexpr log_tag = "memory: ";

struct _Memory {
  GtkPopover parent_instance;

  GtkComboBoxText *thp_enabled, *thp_defrag, *thp_shmem_enabled;

  GtkSpinButton *cache_pressure, *compaction_proactiveness;
};

G_DEFINE_TYPE(Memory, memory, GTK_TYPE_BOX)

void dispose(GObject* object) {
  util::debug(log_tag + "disposed"s);

  G_OBJECT_CLASS(memory_parent_class)->dispose(object);
}

void finalize(GObject* object) {
  util::debug(log_tag + "finalized"s);

  G_OBJECT_CLASS(memory_parent_class)->finalize(object);
}

void memory_class_init(MemoryClass* klass) {
  auto* object_class = G_OBJECT_CLASS(klass);
  auto* widget_class = GTK_WIDGET_CLASS(klass);

  object_class->dispose = dispose;
  object_class->finalize = finalize;

  gtk_widget_class_set_template_from_resource(widget_class, "/com/github/wwmm/fastgame/ui/memory.ui");

  gtk_widget_class_bind_template_child(widget_class, Memory, thp_enabled);
  gtk_widget_class_bind_template_child(widget_class, Memory, thp_defrag);
  gtk_widget_class_bind_template_child(widget_class, Memory, thp_shmem_enabled);
  gtk_widget_class_bind_template_child(widget_class, Memory, cache_pressure);
  gtk_widget_class_bind_template_child(widget_class, Memory, compaction_proactiveness);

  // gtk_widget_class_bind_template_callback(widget_class, on_add_line);
}

void memory_init(Memory* self) {
  gtk_widget_init_template(GTK_WIDGET(self));
}

auto create() -> Memory* {
  return static_cast<Memory*>(g_object_new(FG_TYPE_MEMORY, nullptr));
}

}  // namespace ui::memory

// namespace fs = std::filesystem;

// Memory::Memory(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::Grid(cobject) {
//   // loading glade widgets

//   builder->get_widget("thp_enabled", thp_enabled);
//   builder->get_widget("thp_defrag", thp_defrag);
//   builder->get_widget("thp_shmem_enabled", thp_shmem_enabled);

//   cache_pressure = Glib::RefPtr<Gtk::Adjustment>::cast_dynamic(builder->get_object("cache_pressure"));
//   compaction_proactiveness =
//       Glib::RefPtr<Gtk::Adjustment>::cast_dynamic(builder->get_object("compaction_proactiveness"));

//   // initializing widgets

//   cache_pressure->set_value(std::stoi(util::read_system_setting("/proc/sys/vm/vfs_cache_pressure")[0]));

//   compaction_proactiveness->set_value(std::stoi(util::read_system_setting("/proc/sys/vm/compaction_proactiveness")[0]));

//   read_transparent_huge_page_values();
// }

// auto Memory::add_to_stack(Gtk::Stack* stack) -> Memory* {
//   auto builder = Gtk::Builder::create_from_resource("/com/github/wwmm/fastgame/ui/memory.glade");

//   Memory* ui = nullptr;

//   builder->get_widget_derived("widgets_grid", ui);

//   stack->add(*ui, "memory", _("Memory"));

//   return ui;
// }

// void Memory::read_transparent_huge_page_values() {
//   // parameter: enabled

//   auto enabled_list = util::read_system_setting("/sys/kernel/mm/transparent_hugepage/enabled");

//   auto enabled_value = util::get_selected_value(enabled_list);

//   util::debug(log_tag + "transparent huge pages state: " + enabled_value);

//   for (auto& value : enabled_list) {
//     if (value.find('[') != std::string::npos) {
//       value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
//     }

//     thp_enabled->append(value);
//   }

//   thp_enabled->set_active_text(enabled_value);

//   // parameter: defrag

//   auto defrag_list = util::read_system_setting("/sys/kernel/mm/transparent_hugepage/defrag");

//   auto defrag_value = util::get_selected_value(defrag_list);

//   util::debug(log_tag + "transparent huge pages defrag: " + defrag_value);

//   for (auto& value : defrag_list) {
//     if (value.find('[') != std::string::npos) {
//       value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
//     }

//     thp_defrag->append(value);
//   }

//   thp_defrag->set_active_text(defrag_value);

//   // parameter: shmem_enabled

//   auto shmem_enabled_list = util::read_system_setting("/sys/kernel/mm/transparent_hugepage/shmem_enabled");

//   auto shmem_enabled_value = util::get_selected_value(shmem_enabled_list);

//   util::debug(log_tag + "transparent huge pages state: " + shmem_enabled_value);

//   for (auto& value : shmem_enabled_list) {
//     if (value.find('[') != std::string::npos) {
//       value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
//     }

//     thp_shmem_enabled->append(value);
//   }

//   thp_shmem_enabled->set_active_text(shmem_enabled_value);
// }

// auto Memory::get_cache_pressure() -> int {
//   return static_cast<int>(cache_pressure->get_value());
// }

// void Memory::set_cache_pressure(const int& value) {
//   cache_pressure->set_value(value);
// }

// auto Memory::get_thp_enabled() -> std::string {
//   return thp_enabled->get_active_text();
// }

// void Memory::set_thp_enabled(const std::string& value) {
//   thp_enabled->set_active_text(value);
// }

// auto Memory::get_thp_defrag() -> std::string {
//   return thp_defrag->get_active_text();
// }

// void Memory::set_thp_defrag(const std::string& value) {
//   thp_defrag->set_active_text(value);
// }

// auto Memory::get_thp_shmem_enabled() -> std::string {
//   return thp_shmem_enabled->get_active_text();
// }

// void Memory::set_thp_shmem_enabled(const std::string& value) {
//   thp_shmem_enabled->set_active_text(value);
// }

// auto Memory::get_compaction_proactiveness() -> int {
//   return static_cast<int>(compaction_proactiveness->get_value());
// }

// void Memory::set_compaction_proactiveness(const int& value) {
//   compaction_proactiveness->set_value(value);
// }