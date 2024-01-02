#include "memory.hpp"

namespace ui::memory {

using namespace std::string_literals;

auto constexpr log_tag = "memory: ";

struct _Memory {
  GtkPopover parent_instance;

  GtkDropDown *thp_enabled, *thp_defrag, *thp_shmem_enabled;

  GtkSpinButton *swappiness, *cache_pressure, *compaction_proactiveness, *page_lock_unfairness,
      *percpu_pagelist_high_fraction, *mglru_min_ttl_ms, *min_free_kbytes, *scan_sleep, *alloc_sleep;
};

G_DEFINE_TYPE(Memory, memory, GTK_TYPE_BOX)

void set_swappiness(Memory* self, const int& value) {
  gtk_spin_button_set_value(self->swappiness, value);
}

auto get_swappiness(Memory* self) -> int {
  return static_cast<int>(gtk_spin_button_get_value(self->swappiness));
}

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

void set_min_free_kbytes(Memory* self, const int& value) {
  gtk_spin_button_set_value(self->min_free_kbytes, value);
}

auto get_min_free_kbytes(Memory* self) -> int {
  return static_cast<int>(gtk_spin_button_get_value(self->min_free_kbytes));
}

void set_page_lock_unfairness(Memory* self, const int& value) {
  gtk_spin_button_set_value(self->page_lock_unfairness, value);
}

auto get_page_lock_unfairness(Memory* self) -> int {
  return static_cast<int>(gtk_spin_button_get_value(self->page_lock_unfairness));
}

void set_percpu_pagelist_high_fraction(Memory* self, const int& value) {
  gtk_spin_button_set_value(self->percpu_pagelist_high_fraction, value);
}

auto get_percpu_pagelist_high_fraction(Memory* self) -> int {
  return static_cast<int>(gtk_spin_button_get_value(self->percpu_pagelist_high_fraction));
}

void set_mglru_min_ttl_ms(Memory* self, const int& value) {
  gtk_spin_button_set_value(self->mglru_min_ttl_ms, value);
}

auto get_mglru_min_ttl_ms(Memory* self) -> int {
  return static_cast<int>(gtk_spin_button_get_value(self->mglru_min_ttl_ms));
}

void set_thp_enabled(Memory* self, const std::string& name) {
  auto* model = reinterpret_cast<GtkStringList*>(gtk_drop_down_get_model(self->thp_enabled));

  guint id = 0;

  for (guint n = 0U; n < g_list_model_get_n_items(G_LIST_MODEL(model)); n++) {
    auto label = gtk_string_list_get_string(model, n);

    if (label == nullptr) {
      continue;
    }

    if (label == name) {
      id = n;
    }
  }

  gtk_drop_down_set_selected(self->thp_enabled, id);
}

auto get_thp_enabled(Memory* self) -> std::string {
  auto* selected_item = gtk_drop_down_get_selected_item(self->thp_enabled);

  if (selected_item == nullptr) {
    return "";
  }

  return gtk_string_object_get_string(GTK_STRING_OBJECT(selected_item));
}

void set_thp_defrag(Memory* self, const std::string& name) {
  auto* model = reinterpret_cast<GtkStringList*>(gtk_drop_down_get_model(self->thp_defrag));

  guint id = 0;

  for (guint n = 0U; n < g_list_model_get_n_items(G_LIST_MODEL(model)); n++) {
    auto label = gtk_string_list_get_string(model, n);

    if (label == nullptr) {
      continue;
    }

    if (label == name) {
      id = n;
    }
  }

  gtk_drop_down_set_selected(self->thp_defrag, id);
}

auto get_thp_defrag(Memory* self) -> std::string {
  auto* selected_item = gtk_drop_down_get_selected_item(self->thp_defrag);

  if (selected_item == nullptr) {
    return "";
  }

  return gtk_string_object_get_string(GTK_STRING_OBJECT(selected_item));
}

void set_thp_shmem_enabled(Memory* self, const std::string& name) {
  auto* model = reinterpret_cast<GtkStringList*>(gtk_drop_down_get_model(self->thp_shmem_enabled));

  guint id = 0;

  for (guint n = 0U; n < g_list_model_get_n_items(G_LIST_MODEL(model)); n++) {
    auto label = gtk_string_list_get_string(model, n);

    if (label == nullptr) {
      continue;
    }

    if (label == name) {
      id = n;
    }
  }

  gtk_drop_down_set_selected(self->thp_shmem_enabled, id);
}

auto get_thp_shmem_enabled(Memory* self) -> std::string {
  auto* selected_item = gtk_drop_down_get_selected_item(self->thp_shmem_enabled);

  if (selected_item == nullptr) {
    return "";
  }

  return gtk_string_object_get_string(GTK_STRING_OBJECT(selected_item));
}

void set_scan_sleep(Memory* self, const int& value) {
  gtk_spin_button_set_value(self->scan_sleep, value);
}

auto get_scan_sleep(Memory* self) -> int {
  return static_cast<int>(gtk_spin_button_get_value(self->scan_sleep));
}

void set_alloc_sleep(Memory* self, const int& value) {
  gtk_spin_button_set_value(self->alloc_sleep, value);
}

auto get_alloc_sleep(Memory* self) -> int {
  return static_cast<int>(gtk_spin_button_get_value(self->alloc_sleep));
}

void read_transparent_huge_page_values(Memory* self) {
  // parameter: enabled

  {
    auto enabled_list = util::read_system_setting("/sys/kernel/mm/transparent_hugepage/enabled");

    auto enabled_value = util::get_selected_value(enabled_list);

    util::debug(log_tag + "transparent huge pages state: "s + enabled_value);

    auto* model = reinterpret_cast<GtkStringList*>(gtk_drop_down_get_model(self->thp_enabled));

    guint selected_id = 0;

    for (size_t n = 0; n < enabled_list.size(); n++) {
      auto value = enabled_list[n];

      if (value.empty()) {
        continue;
      }

      if (value.find('[') != std::string::npos) {
        value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
      }

      gtk_string_list_append(model, value.c_str());

      if (value == enabled_value) {
        selected_id = n;
      }
    }

    gtk_drop_down_set_selected(self->thp_enabled, selected_id);
  }

  // parameter: defrag

  {
    auto defrag_list = util::read_system_setting("/sys/kernel/mm/transparent_hugepage/defrag");

    auto defrag_value = util::get_selected_value(defrag_list);

    util::debug(log_tag + "transparent huge pages defrag: "s + defrag_value);

    auto* model = reinterpret_cast<GtkStringList*>(gtk_drop_down_get_model(self->thp_defrag));

    guint selected_id = 0;

    for (size_t n = 0; n < defrag_list.size(); n++) {
      auto value = defrag_list[n];

      if (value.empty()) {
        continue;
      }

      if (value.find('[') != std::string::npos) {
        value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
      }

      gtk_string_list_append(model, value.c_str());

      if (value == defrag_value) {
        selected_id = n;
      }
    }

    gtk_drop_down_set_selected(self->thp_defrag, selected_id);
  }

  // parameter: shmem_enabled

  {
    auto shmem_enabled_list = util::read_system_setting("/sys/kernel/mm/transparent_hugepage/shmem_enabled");

    auto shmem_enabled_value = util::get_selected_value(shmem_enabled_list);

    util::debug(log_tag + "transparent huge pages state: "s + shmem_enabled_value);

    auto* model = reinterpret_cast<GtkStringList*>(gtk_drop_down_get_model(self->thp_shmem_enabled));

    guint selected_id = 0;

    for (size_t n = 0; n < shmem_enabled_list.size(); n++) {
      auto value = shmem_enabled_list[n];

      if (value.empty()) {
        continue;
      }

      if (value.find('[') != std::string::npos) {
        value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
      }

      gtk_string_list_append(model, value.c_str());

      if (value == shmem_enabled_value) {
        selected_id = n;
      }
    }

    gtk_drop_down_set_selected(self->thp_shmem_enabled, selected_id);
  }

  if (const auto list =
          util::read_system_setting("/sys/kernel/mm/transparent_hugepage/khugepaged/scan_sleep_millisecs");
      !list.empty()) {
    gtk_spin_button_set_value(self->scan_sleep, std::stoi(list[0]));
  }

  if (const auto list =
          util::read_system_setting("/sys/kernel/mm/transparent_hugepage/khugepaged/alloc_sleep_millisecs");
      !list.empty()) {
    gtk_spin_button_set_value(self->alloc_sleep, std::stoi(list[0]));
  }
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
  gtk_widget_class_bind_template_child(widget_class, Memory, swappiness);
  gtk_widget_class_bind_template_child(widget_class, Memory, cache_pressure);
  gtk_widget_class_bind_template_child(widget_class, Memory, compaction_proactiveness);
  gtk_widget_class_bind_template_child(widget_class, Memory, page_lock_unfairness);
  gtk_widget_class_bind_template_child(widget_class, Memory, percpu_pagelist_high_fraction);
  gtk_widget_class_bind_template_child(widget_class, Memory, mglru_min_ttl_ms);
  gtk_widget_class_bind_template_child(widget_class, Memory, min_free_kbytes);
  gtk_widget_class_bind_template_child(widget_class, Memory, scan_sleep);
  gtk_widget_class_bind_template_child(widget_class, Memory, alloc_sleep);
}

void memory_init(Memory* self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  ui::prepare_spinbuttons<"ms">(self->mglru_min_ttl_ms, self->scan_sleep, self->alloc_sleep);
  ui::prepare_spinbutton<"KB">(self->min_free_kbytes);

  if (const auto list = util::read_system_setting("/proc/sys/vm/swappiness"); !list.empty()) {
    gtk_spin_button_set_value(self->swappiness, std::stoi(list[0]));
  }

  if (const auto list = util::read_system_setting("/proc/sys/vm/vfs_cache_pressure"); !list.empty()) {
    gtk_spin_button_set_value(self->cache_pressure, std::stoi(list[0]));
  }

  if (const auto list = util::read_system_setting("/proc/sys/vm/compaction_proactiveness"); !list.empty()) {
    gtk_spin_button_set_value(self->compaction_proactiveness, std::stoi(list[0]));
  }

  if (const auto list = util::read_system_setting("/proc/sys/vm/min_free_kbytes"); !list.empty()) {
    gtk_spin_button_set_value(self->min_free_kbytes, std::stoi(list[0]));
  }

  if (const auto list = util::read_system_setting("/proc/sys/vm/page_lock_unfairness"); !list.empty()) {
    gtk_spin_button_set_value(self->page_lock_unfairness, std::stoi(list[0]));
  }

  if (const auto list = util::read_system_setting("/proc/sys/vm/percpu_pagelist_high_fraction"); !list.empty()) {
    gtk_spin_button_set_value(self->percpu_pagelist_high_fraction, std::stoi(list[0]));
  }

  if (const auto list = util::read_system_setting("/sys/kernel/mm/lru_gen/min_ttl_ms"); !list.empty()) {
    gtk_spin_button_set_value(self->mglru_min_ttl_ms, std::stoi(list[0]));
  }

  read_transparent_huge_page_values(self);
}

auto create() -> Memory* {
  return static_cast<Memory*>(g_object_new(FG_TYPE_MEMORY, nullptr));
}

}  // namespace ui::memory
