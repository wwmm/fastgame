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

void set_cache_pressure(Memory* self, const int& value) {
  gtk_spin_button_set_value(self->cache_pressure, value);
}

auto get_cache_pressure(Memory* self) -> int {
  return static_cast<int>(gtk_spin_button_get_value(self->cache_pressure));
}

void set_compaction_proactiveness(Memory* self, const int& value) {
  gtk_spin_button_set_value(self->compaction_proactiveness, value);
}

auto get_compaction_proactiveness(Memory* self) -> int {
  return static_cast<int>(gtk_spin_button_get_value(self->compaction_proactiveness));
}

void set_thp_enabled(Memory* self, const std::string& name) {
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(self->thp_enabled), name.c_str());
}

auto get_thp_enabled(Memory* self) -> std::string {
  return gtk_combo_box_text_get_active_text(self->thp_enabled);
}

void set_thp_defrag(Memory* self, const std::string& name) {
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(self->thp_defrag), name.c_str());
}

auto get_thp_defrag(Memory* self) -> std::string {
  return gtk_combo_box_text_get_active_text(self->thp_defrag);
}

void set_thp_shmem_enabled(Memory* self, const std::string& name) {
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(self->thp_shmem_enabled), name.c_str());
}

auto get_thp_shmem_enabled(Memory* self) -> std::string {
  return gtk_combo_box_text_get_active_text(self->thp_shmem_enabled);
}

void read_transparent_huge_page_values(Memory* self) {
  // parameter: enabled

  auto enabled_list = util::read_system_setting("/sys/kernel/mm/transparent_hugepage/enabled");

  auto enabled_value = util::get_selected_value(enabled_list);

  util::debug(log_tag + "transparent huge pages state: "s + enabled_value);

  for (auto& value : enabled_list) {
    if (value.find('[') != std::string::npos) {
      value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
    }

    gtk_combo_box_text_append(self->thp_enabled, value.c_str(), value.c_str());
  }

  gtk_combo_box_set_active_id(GTK_COMBO_BOX(self->thp_enabled), enabled_value.c_str());

  // parameter: defrag

  auto defrag_list = util::read_system_setting("/sys/kernel/mm/transparent_hugepage/defrag");

  auto defrag_value = util::get_selected_value(defrag_list);

  util::debug(log_tag + "transparent huge pages defrag: "s + defrag_value);

  for (auto& value : defrag_list) {
    if (value.find('[') != std::string::npos) {
      value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
    }

    gtk_combo_box_text_append(self->thp_defrag, value.c_str(), value.c_str());
  }

  gtk_combo_box_set_active_id(GTK_COMBO_BOX(self->thp_defrag), defrag_value.c_str());

  // parameter: shmem_enabled

  auto shmem_enabled_list = util::read_system_setting("/sys/kernel/mm/transparent_hugepage/shmem_enabled");

  auto shmem_enabled_value = util::get_selected_value(shmem_enabled_list);

  util::debug(log_tag + "transparent huge pages state: "s + shmem_enabled_value);

  for (auto& value : shmem_enabled_list) {
    if (value.find('[') != std::string::npos) {
      value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
    }

    gtk_combo_box_text_append(self->thp_shmem_enabled, value.c_str(), value.c_str());
  }

  gtk_combo_box_set_active_id(GTK_COMBO_BOX(self->thp_shmem_enabled), shmem_enabled_value.c_str());
}

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
}

void memory_init(Memory* self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  gtk_spin_button_set_value(self->cache_pressure,
                            std::stoi(util::read_system_setting("/proc/sys/vm/vfs_cache_pressure")[0]));

  gtk_spin_button_set_value(self->compaction_proactiveness,
                            std::stoi(util::read_system_setting("/proc/sys/vm/compaction_proactiveness")[0]));

  read_transparent_huge_page_values(self);
}

auto create() -> Memory* {
  return static_cast<Memory*>(g_object_new(FG_TYPE_MEMORY, nullptr));
}

}  // namespace ui::memory
