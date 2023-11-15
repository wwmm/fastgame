#include "cpu.hpp"

namespace ui::cpu {

using namespace std::string_literals;

auto constexpr log_tag = "cpu: ";

struct _Cpu {
  GtkPopover parent_instance;

  GtkDropDown *frequency_governor, *pcie_aspm_policy;

  GtkSpinButton *niceness, *autogroup_niceness, *timer_slack;

  GtkSwitch *use_sched_batch, *realtime_wineserver, *use_cpu_dma_latency, *child_runs_first;

  GtkFlowBox *flowbox_game_affinity, *flowbox_wineserver_affinity;
};

G_DEFINE_TYPE(Cpu, cpu, GTK_TYPE_BOX)

void set_enable_batch_scheduler(Cpu* self, const bool& state) {
  gtk_switch_set_active(self->use_sched_batch, state != 0);
}

auto get_enable_batch_scheduler(Cpu* self) -> bool {
  return gtk_switch_get_active(self->use_sched_batch);
}

void set_child_runs_first(Cpu* self, const bool& state) {
  gtk_switch_set_active(self->child_runs_first, state != 0);
}

auto get_child_runs_first(Cpu* self) -> bool {
  return gtk_switch_get_active(self->child_runs_first);
}

void set_use_cpu_dma_latency(Cpu* self, const bool& state) {
  gtk_switch_set_active(self->use_cpu_dma_latency, state != 0);
}

auto get_use_cpu_dma_latency(Cpu* self) -> bool {
  return gtk_switch_get_active(self->use_cpu_dma_latency);
}

void set_use_realtime_wineserver(Cpu* self, const bool& state) {
  gtk_switch_set_active(self->realtime_wineserver, state != 0);
}

auto get_use_realtime_wineserver(Cpu* self) -> bool {
  return gtk_switch_get_active(self->realtime_wineserver);
}

void set_niceness(Cpu* self, const int& value) {
  gtk_spin_button_set_value(self->niceness, value);
}

auto get_niceness(Cpu* self) -> int {
  return static_cast<int>(gtk_spin_button_get_value(self->niceness));
}

void set_autogroup_niceness(Cpu* self, const int& value) {
  gtk_spin_button_set_value(self->autogroup_niceness, value);
}

auto get_autogroup_niceness(Cpu* self) -> int {
  return static_cast<int>(gtk_spin_button_get_value(self->autogroup_niceness));
}

void set_timer_slack(Cpu* self, const int& value) {
  gtk_spin_button_set_value(self->timer_slack, value);
}

auto get_timer_slack(Cpu* self) -> int {
  return static_cast<int>(gtk_spin_button_get_value(self->timer_slack));
}

void set_frequency_governor(Cpu* self, const std::string& name) {
  auto* model = reinterpret_cast<GtkStringList*>(gtk_drop_down_get_model(self->frequency_governor));

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

  gtk_drop_down_set_selected(self->frequency_governor, id);
}

auto get_frequency_governor(Cpu* self) -> std::string {
  auto* selected_item = gtk_drop_down_get_selected_item(self->frequency_governor);

  if (selected_item == nullptr) {
    return "";
  }

  return gtk_string_object_get_string(GTK_STRING_OBJECT(selected_item));
}

void set_pcie_aspm_policy(Cpu* self, const std::string& name) {
  auto* model = reinterpret_cast<GtkStringList*>(gtk_drop_down_get_model(self->pcie_aspm_policy));

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

  gtk_drop_down_set_selected(self->pcie_aspm_policy, id);
}

auto get_pcie_aspm_policy(Cpu* self) -> std::string {
  auto* selected_item = gtk_drop_down_get_selected_item(self->pcie_aspm_policy);

  if (selected_item == nullptr) {
    return "";
  }

  return gtk_string_object_get_string(GTK_STRING_OBJECT(selected_item));
}

void set_cores(Cpu* self, GtkFlowBox* flowbox, const std::vector<std::string>& list) {
  for (auto child = gtk_widget_get_first_child(GTK_WIDGET(flowbox)); child != nullptr;) {
    auto* next_child = gtk_widget_get_next_sibling(child);

    auto* checkbutton = GTK_CHECK_BUTTON(gtk_widget_get_first_child(child));

    if (std::find(list.begin(), list.end(), gtk_check_button_get_label(checkbutton)) != list.end()) {
      gtk_check_button_set_active(checkbutton, 1);
    } else {
      gtk_check_button_set_active(checkbutton, 0);
    }

    child = next_child;
  }
}

auto get_cores(Cpu* self, GtkFlowBox* flowbox) -> std::vector<std::string> {
  std::vector<std::string> list;

  for (auto child = gtk_widget_get_first_child(GTK_WIDGET(flowbox)); child != nullptr;) {
    auto* next_child = gtk_widget_get_next_sibling(child);

    auto* checkbutton = GTK_CHECK_BUTTON(gtk_widget_get_first_child(child));

    if (gtk_check_button_get_active(checkbutton) != 0) {
      list.emplace_back(gtk_check_button_get_label(checkbutton));
    }

    child = next_child;
  }

  return list;
}

void set_game_cores(Cpu* self, const std::vector<std::string>& list) {
  set_cores(self, self->flowbox_game_affinity, list);
}

auto get_game_cores(Cpu* self) -> std::vector<std::string> {
  return get_cores(self, self->flowbox_game_affinity);
}

void set_wineserver_cores(Cpu* self, const std::vector<std::string>& list) {
  set_cores(self, self->flowbox_wineserver_affinity, list);
}

auto get_wineserver_cores(Cpu* self) -> std::vector<std::string> {
  return get_cores(self, self->flowbox_wineserver_affinity);
}

void init_pcie_aspm(Cpu* self) {
  auto list_policies = util::read_system_setting("/sys/module/pcie_aspm/parameters/policy");

  auto selected_policy = util::get_selected_value(list_policies);

  util::debug(log_tag + "The current pcie_aspm policy is: "s + selected_policy);

  auto* model = reinterpret_cast<GtkStringList*>(gtk_drop_down_get_model(self->pcie_aspm_policy));

  guint selected_id = 0;

  for (size_t n = 0; n < list_policies.size(); n++) {
    auto value = list_policies[n];

    if (value.empty()) {
      continue;
    }

    if (value.find('[') != std::string::npos) {
      value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
    }

    gtk_string_list_append(model, value.c_str());

    if (value == selected_policy) {
      selected_id = n;
    }
  }

  gtk_drop_down_set_selected(self->pcie_aspm_policy, selected_id);
}

void dispose(GObject* object) {
  util::debug(log_tag + "disposed"s);

  G_OBJECT_CLASS(cpu_parent_class)->dispose(object);
}

void finalize(GObject* object) {
  util::debug(log_tag + "finalized"s);

  G_OBJECT_CLASS(cpu_parent_class)->finalize(object);
}

void cpu_class_init(CpuClass* klass) {
  auto* object_class = G_OBJECT_CLASS(klass);
  auto* widget_class = GTK_WIDGET_CLASS(klass);

  object_class->dispose = dispose;
  object_class->finalize = finalize;

  gtk_widget_class_set_template_from_resource(widget_class, "/com/github/wwmm/fastgame/ui/cpu.ui");

  gtk_widget_class_bind_template_child(widget_class, Cpu, frequency_governor);
  gtk_widget_class_bind_template_child(widget_class, Cpu, pcie_aspm_policy);
  gtk_widget_class_bind_template_child(widget_class, Cpu, niceness);
  gtk_widget_class_bind_template_child(widget_class, Cpu, autogroup_niceness);
  gtk_widget_class_bind_template_child(widget_class, Cpu, timer_slack);
  gtk_widget_class_bind_template_child(widget_class, Cpu, use_sched_batch);
  gtk_widget_class_bind_template_child(widget_class, Cpu, realtime_wineserver);
  gtk_widget_class_bind_template_child(widget_class, Cpu, use_cpu_dma_latency);
  gtk_widget_class_bind_template_child(widget_class, Cpu, child_runs_first);
  gtk_widget_class_bind_template_child(widget_class, Cpu, flowbox_game_affinity);
  gtk_widget_class_bind_template_child(widget_class, Cpu, flowbox_wineserver_affinity);
}

void cpu_init(Cpu* self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  ui::prepare_spinbutton<"ns">(self->timer_slack);

  int timer_slack_ns = prctl(PR_GET_TIMERSLACK, &timer_slack_ns, 0, 0, 0);

  gtk_spin_button_set_value(self->timer_slack, timer_slack_ns);

  if (const auto list = util::read_system_setting("/proc/sys/kernel/sched_child_runs_first"); !list.empty()) {
    gtk_switch_set_active(self->child_runs_first, std::stoi(list[0]) != 0);
  }

  /* We assume that all cores are set to the same frequency governor and that the system has at least one core. In
     this case reading the core 0 property should be enough
  */

  auto list_governors = util::read_system_setting("/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors");

  auto selected_governor = util::read_system_setting("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor")[0];

  auto* model = reinterpret_cast<GtkStringList*>(gtk_drop_down_get_model(self->frequency_governor));

  guint selected_id = 0;

  for (size_t n = 0; n < list_governors.size(); n++) {
    auto value = list_governors[n];

    if (value.empty()) {
      continue;
    }

    gtk_string_list_append(model, value.c_str());

    if (value == selected_governor) {
      selected_id = n;
    }
  }

  gtk_drop_down_set_selected(self->frequency_governor, selected_id);

  auto n_cores = std::thread::hardware_concurrency();

  util::debug(log_tag + "number of cores: "s + std::to_string(n_cores));

  for (uint n = 0; n < n_cores; n++) {
    auto* checkbutton1 = gtk_check_button_new_with_label(std::to_string(n).c_str());
    auto* checkbutton2 = gtk_check_button_new_with_label(std::to_string(n).c_str());

    gtk_check_button_set_active(GTK_CHECK_BUTTON(checkbutton1), 1);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(checkbutton2), 1);

    gtk_flow_box_append(self->flowbox_game_affinity, checkbutton1);
    gtk_flow_box_append(self->flowbox_wineserver_affinity, checkbutton2);
  }

  init_pcie_aspm(self);
}

auto create() -> Cpu* {
  return static_cast<Cpu*>(g_object_new(FG_TYPE_CPU, nullptr));
}

}  // namespace ui::cpu
