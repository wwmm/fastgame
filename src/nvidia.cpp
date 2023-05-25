#include "nvidia.hpp"

namespace ui::nvidia {

using namespace std::string_literals;

auto constexpr log_tag = "nvidia: ";

#ifdef USE_NVIDIA
std::unique_ptr<nvidia_wrapper::Nvidia> nv_wrapper;
#endif

struct _Nvidia {
  GtkPopover parent_instance;

  GtkDropDown* powermize_mode0;

  GtkSpinButton *gpu_clock_offset0, *memory_clock_offset0;
};

G_DEFINE_TYPE(Nvidia, nvidia, GTK_TYPE_BOX)

auto has_gpu() -> bool {
  bool found = false;

#ifdef USE_NVIDIA
  found = nv_wrapper->has_gpu();
#endif

  return found;
}

void set_powermize_mode(Nvidia* self, const int& mode_id, const int& card_index) {
  gtk_drop_down_set_selected(self->powermize_mode0, mode_id);
}

auto get_powermize_mode(Nvidia* self, const int& card_index) -> int {
  return gtk_drop_down_get_selected(self->powermize_mode0);
}

void set_gpu_clock_offset(Nvidia* self, const int& value, const int& card_index) {
  gtk_spin_button_set_value(self->gpu_clock_offset0, value);
}

auto get_gpu_clock_offset(Nvidia* self, const int& card_index) -> int {
  return static_cast<int>(gtk_spin_button_get_value(self->gpu_clock_offset0));
}

void set_memory_clock_offset(Nvidia* self, const int& value, const int& card_index) {
  gtk_spin_button_set_value(self->memory_clock_offset0, value);
}

auto get_memory_clock_offset(Nvidia* self, const int& card_index) -> int {
  return static_cast<int>(gtk_spin_button_get_value(self->memory_clock_offset0));
}

void dispose(GObject* object) {
  util::debug(log_tag + "disposed"s);

  G_OBJECT_CLASS(nvidia_parent_class)->dispose(object);
}

void finalize(GObject* object) {
  util::debug(log_tag + "finalized"s);

  G_OBJECT_CLASS(nvidia_parent_class)->finalize(object);
}

void nvidia_class_init(NvidiaClass* klass) {
  auto* object_class = G_OBJECT_CLASS(klass);
  auto* widget_class = GTK_WIDGET_CLASS(klass);

  object_class->dispose = dispose;
  object_class->finalize = finalize;

  gtk_widget_class_set_template_from_resource(widget_class, "/com/github/wwmm/fastgame/ui/nvidia.ui");

  gtk_widget_class_bind_template_child(widget_class, Nvidia, powermize_mode0);
  gtk_widget_class_bind_template_child(widget_class, Nvidia, gpu_clock_offset0);
  gtk_widget_class_bind_template_child(widget_class, Nvidia, memory_clock_offset0);
}

void nvidia_init(Nvidia* self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  ui::prepare_spinbuttons<"MHz">(self->gpu_clock_offset0, self->memory_clock_offset0);

#ifdef USE_NVIDIA
  nv_wrapper = std::make_unique<nvidia_wrapper::Nvidia>();

  {
    auto [min, max] = nv_wrapper->get_gpu_clock_offset_range();

    auto adj = gtk_spin_button_get_adjustment(self->gpu_clock_offset0);

    gtk_adjustment_set_lower(adj, min);
    gtk_adjustment_set_upper(adj, max);
  }

  {
    auto [min, max] = nv_wrapper->get_memory_clock_offset_range();

    auto adj = gtk_spin_button_get_adjustment(self->memory_clock_offset0);

    gtk_adjustment_set_lower(adj, min);
    gtk_adjustment_set_upper(adj, max);
  }

#endif
}

auto create() -> Nvidia* {
  return static_cast<Nvidia*>(g_object_new(FG_TYPE_NVIDIA, nullptr));
}

}  // namespace ui::nvidia
