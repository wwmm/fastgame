#include "application_ui.hpp"
#include "presets_menu.hpp"

namespace ui::application_window {

using namespace std::string_literals;

auto constexpr log_tag = "application_ui: ";

struct Data {
 public:
  ~Data() { util::debug(log_tag + "data struct destroyed"s); }

  int width, height;

  bool maximized, fullscreen;

  GApplication* gapp;

  GtkIconTheme* icon_theme;
};

struct _ApplicationWindow {
  AdwWindow parent_instance;

  AdwViewStack* stack;

  GtkListView* listview;

  GtkMenuButton* presets_menu_button;

  GtkEntry* game_executable;

  GtkSpinner* spinner;

  ui::presets_menu::PresetsMenu* presetsMenu;

  ui::environment_variables::EnvironmentVariables* environment_variables;

  ui::cpu::Cpu* cpu;

  ui::memory::Memory* memory;

  ui::amdgpu::Amdgpu* amdgpu;

  ui::disk::Disk* disk;

  GSettings* settings;

  Data* data;
};

G_DEFINE_TYPE(ApplicationWindow, application_window, ADW_TYPE_APPLICATION_WINDOW)

void save_preset(ApplicationWindow* self, const std::string& name, const std::filesystem::path& directory) {
  boost::property_tree::ptree root;
  boost::property_tree::ptree node;

  // game executable

  root.put("game-executable", gtk_editable_get_text(GTK_EDITABLE(self->game_executable)));

  // environment variables

  for (const auto& v : ui::environment_variables::get_list()) {
    boost::property_tree::ptree local_node;

    local_node.put("", v);

    node.push_back(std::make_pair("", local_node));
  }

  root.add_child("environment-variables", node);

  // cpu

  root.put("cpu.use-batch-scheduler", ui::cpu::get_enable_batch_scheduler(self->cpu));
  root.put("cpu.child-runs-first", ui::cpu::get_child_runs_first(self->cpu));
  root.put("cpu.frequency-governor", ui::cpu::get_frequency_governor(self->cpu));
  root.put("cpu.pcie-aspm-policy", ui::cpu::get_pcie_aspm_policy(self->cpu));
  root.put("cpu.use-cpu-dma-latency", ui::cpu::get_use_cpu_dma_latency(self->cpu));
  root.put("cpu.use-realtime-wineserver", ui::cpu::get_use_realtime_wineserver(self->cpu));
  root.put("cpu.niceness", ui::cpu::get_niceness(self->cpu));
  root.put("cpu.timer-slack", ui::cpu::get_timer_slack(self->cpu));

  node.clear();

  for (const auto& c : ui::cpu::get_game_cores(self->cpu)) {
    boost::property_tree::ptree local_node;

    local_node.put("", c);

    node.push_back(std::make_pair("", local_node));
  }

  root.add_child("cpu.game-cores", node);

  node.clear();

  for (const auto& c : ui::cpu::get_wineserver_cores(self->cpu)) {
    boost::property_tree::ptree local_node;

    local_node.put("", c);

    node.push_back(std::make_pair("", local_node));
  }

  root.add_child("cpu.wineserver-cores", node);

  // memory

  root.put("memory.virtual-memory.swappiness", ui::memory::get_swappiness(self->memory));
  root.put("memory.virtual-memory.cache-pressure", ui::memory::get_cache_pressure(self->memory));
  root.put("memory.virtual-memory.compaction-proactiveness", ui::memory::get_compaction_proactiveness(self->memory));
  root.put("memory.virtual-memory.page-lock-unfairness", ui::memory::get_page_lock_unfairness(self->memory));
  root.put("memory.virtual-memory.percpu-pagelist-high-fraction",
           ui::memory::get_percpu_pagelist_high_fraction(self->memory));

  root.put("memory.mglru.min_ttl_ms", ui::memory::get_mglru_min_ttl_ms(self->memory));

  root.put("memory.transparent-hugepages.enabled", ui::memory::get_thp_enabled(self->memory));
  root.put("memory.transparent-hugepages.defrag", ui::memory::get_thp_defrag(self->memory));
  root.put("memory.transparent-hugepages.shmem_enabled", ui::memory::get_thp_shmem_enabled(self->memory));

  // amdgpu

  if (self->amdgpu != nullptr) {
    root.put("amdgpu.performance-level", ui::amdgpu::get_performance_level(self->amdgpu));
    root.put("amdgpu.power-profile", ui::amdgpu::get_power_profile(self->amdgpu));
    root.put("amdgpu.power-cap", ui::amdgpu::get_power_cap(self->amdgpu));

    if (ui::amdgpu::get_n_cards() == 2) {
      root.put("amdgpu.card1.performance-level", ui::amdgpu::get_performance_level(self->amdgpu, 1));
      root.put("amdgpu.card1.power-profile", ui::amdgpu::get_power_profile(self->amdgpu, 1));
      root.put("amdgpu.card1.power-cap", ui::amdgpu::get_power_cap(self->amdgpu, 1));
    }
  }

  // disk

  root.put("disk.device", ui::disk::get_device(self->disk));
  root.put("disk.scheduler", ui::disk::get_scheduler(self->disk));
  root.put("disk.enable-realtime-priority", ui::disk::get_enable_realtime_priority(self->disk));
  root.put("disk.readahead", ui::disk::get_readahead(self->disk));
  root.put("disk.nr-requests", ui::disk::get_nr_requests(self->disk));
  root.put("disk.rq-affinity", ui::disk::get_rq_affinity(self->disk));
  root.put("disk.nomerges", ui::disk::get_nomerges(self->disk));
  root.put("disk.add_random", ui::disk::get_enable_add_random(self->disk));
  root.put("disk.udisks.drive-id", ui::disk::get_drive_id());
  root.put("disk.udisks.supports-apm", ui::disk::get_supports_apm());
  root.put("disk.udisks.disable-apm", ui::disk::get_disable_apm(self->disk));
  root.put("disk.udisks.supports-write-cache", ui::disk::get_supports_write_cache());
  root.put("disk.udisks.enable-write-cache", ui::disk::get_enable_write_cache(self->disk));

  auto output_file = directory / std::filesystem::path{name + ".json"};

  boost::property_tree::write_json(output_file, root);

  util::debug(log_tag + "saved preset: "s + output_file.string());
}

void load_preset(ApplicationWindow* self, const std::string& name) {
  auto input_file = ui::presets_menu::user_presets_dir / std::filesystem::path{name + ".json"};

  boost::property_tree::ptree root;

  boost::property_tree::read_json(input_file.string(), root);

  // game executable

  gtk_editable_set_text(GTK_EDITABLE(self->game_executable), root.get<std::string>("game-executable", "").c_str());

  // environmental variables

  try {
    std::vector<std::string> variables_list;

    for (const auto& c : root.get_child("environment-variables")) {
      variables_list.emplace_back(c.second.data());
    }

    ui::environment_variables::add_list(variables_list);
  } catch (const boost::property_tree::ptree_error& e) {
    util::warning(log_tag + "error when parsing the environmental variables list"s);
  }

  // cpu

  ui::cpu::set_enable_batch_scheduler(
      self->cpu, root.get<bool>("cpu.use-batch-scheduler", ui::cpu::get_enable_batch_scheduler(self->cpu)));

  ui::cpu::set_child_runs_first(self->cpu,
                                root.get<bool>("cpu.child-runs-first", ui::cpu::get_child_runs_first(self->cpu)));

  ui::cpu::set_frequency_governor(
      self->cpu, root.get<std::string>("cpu.frequency-governor", ui::cpu::get_frequency_governor(self->cpu)));

  ui::cpu::set_pcie_aspm_policy(
      self->cpu, root.get<std::string>("cpu.pcie-aspm-policy", ui::cpu::get_pcie_aspm_policy(self->cpu)));

  ui::cpu::set_use_cpu_dma_latency(
      self->cpu, root.get<bool>("cpu.use-cpu-dma-latency", ui::cpu::get_use_cpu_dma_latency(self->cpu)));

  ui::cpu::set_use_realtime_wineserver(
      self->cpu, root.get<bool>("cpu.use-realtime-wineserver", ui::cpu::get_use_realtime_wineserver(self->cpu)));

  ui::cpu::set_niceness(self->cpu, root.get<int>("cpu.niceness", ui::cpu::get_niceness(self->cpu)));

  ui::cpu::set_timer_slack(self->cpu, root.get<int>("cpu.timer-slack", ui::cpu::get_timer_slack(self->cpu)));

  try {
    std::vector<std::string> cores_list;

    for (const auto& c : root.get_child("cpu.game-cores")) {
      cores_list.emplace_back(c.second.data());
    }

    ui::cpu::set_game_cores(self->cpu, cores_list);

    cores_list.clear();

    for (const auto& c : root.get_child("cpu.wineserver-cores")) {
      cores_list.emplace_back(c.second.data());
    }

    ui::cpu::set_wineserver_cores(self->cpu, cores_list);
  } catch (const boost::property_tree::ptree_error& e) {
    util::warning(log_tag + "error when parsing the cpu core list"s);
  }

  // memory

  ui::memory::set_swappiness(
      self->memory, root.get<int>("memory.virtual-memory.swappiness", ui::memory::get_swappiness(self->memory)));

  ui::memory::set_cache_pressure(self->memory, root.get<int>("memory.virtual-memory.cache-pressure",
                                                             ui::memory::get_cache_pressure(self->memory)));

  ui::memory::set_compaction_proactiveness(self->memory,
                                           root.get<int>("memory.virtual-memory.compaction-proactiveness",
                                                         ui::memory::get_compaction_proactiveness(self->memory)));

  ui::memory::set_page_lock_unfairness(self->memory, root.get<int>("memory.virtual-memory.page-lock-unfairness",
                                                                   ui::memory::get_page_lock_unfairness(self->memory)));

  ui::memory::set_percpu_pagelist_high_fraction(
      self->memory, root.get<int>("memory.virtual-memory.percpu-pagelist-high-fraction",
                                  ui::memory::get_percpu_pagelist_high_fraction(self->memory)));

  ui::memory::set_mglru_min_ttl_ms(
      self->memory, root.get<int>("memory.mglru.min_ttl_ms", ui::memory::get_mglru_min_ttl_ms(self->memory)));

  ui::memory::set_thp_enabled(self->memory, root.get<std::string>("memory.transparent-hugepages.enabled",
                                                                  ui::memory::get_thp_enabled(self->memory)));

  ui::memory::set_thp_defrag(self->memory, root.get<std::string>("memory.transparent-hugepages.defrag",
                                                                 ui::memory::get_thp_defrag(self->memory)));

  ui::memory::set_thp_shmem_enabled(self->memory,
                                    root.get<std::string>("memory.transparent-hugepages.shmem_enabled",
                                                          ui::memory::get_thp_shmem_enabled(self->memory)));

  // amdgpu

  if (self->amdgpu != nullptr) {
    ui::amdgpu::set_performance_level(
        self->amdgpu,
        root.get<std::string>("amdgpu.performance-level", ui::amdgpu::get_performance_level(self->amdgpu)));

    ui::amdgpu::set_power_profile(
        self->amdgpu, root.get<std::string>("amdgpu.power-profile", ui::amdgpu::get_power_profile(self->amdgpu)));

    ui::amdgpu::set_power_cap(self->amdgpu, root.get<int>("amdgpu.power-cap", ui::amdgpu::get_power_cap(self->amdgpu)));

    if (ui::amdgpu::get_n_cards() == 2) {
      ui::amdgpu::set_performance_level(
          self->amdgpu,
          root.get<std::string>("amdgpu.card1.performance-level", ui::amdgpu::get_performance_level(self->amdgpu, 1)),
          1);

      ui::amdgpu::set_power_profile(
          self->amdgpu,
          root.get<std::string>("amdgpu.card1.power-profile", ui::amdgpu::get_power_profile(self->amdgpu, 1)), 1);

      ui::amdgpu::set_power_cap(self->amdgpu,
                                root.get<int>("amdgpu.card1.power-cap", ui::amdgpu::get_power_cap(self->amdgpu, 1)), 1);
    }
  }

  // disk

  ui::disk::set_device(self->disk, root.get<std::string>("disk.device", ui::disk::get_device(self->disk)));

  ui::disk::set_scheduler(self->disk, root.get<std::string>("disk.scheduler", ui::disk::get_scheduler(self->disk)));

  ui::disk::set_enable_realtime_priority(
      self->disk, root.get<bool>("disk.enable-realtime-priority", ui::disk::get_enable_realtime_priority(self->disk)));

  ui::disk::set_readahead(self->disk, root.get<int>("disk.readahead", ui::disk::get_readahead(self->disk)));

  ui::disk::set_nr_requests(self->disk, root.get<int>("disk.nr-requests", ui::disk::get_nr_requests(self->disk)));

  ui::disk::set_rq_affinity(self->disk, root.get<int>("disk.rq-affinity", ui::disk::get_rq_affinity(self->disk)));

  ui::disk::set_nomerges(self->disk, root.get<int>("disk.nomerges", ui::disk::get_nomerges(self->disk)));

  ui::disk::set_enable_add_random(self->disk,
                                  root.get<bool>("disk.add_random", ui::disk::get_enable_add_random(self->disk)));

  ui::disk::set_disable_apm(self->disk,
                            root.get<bool>("disk.udisks.disable-apm", ui::disk::get_disable_apm(self->disk)));

  ui::disk::set_enable_write_cache(
      self->disk, root.get<bool>("disk.udisks.enable-write-cache", ui::disk::get_enable_write_cache(self->disk)));
}

void on_apply_settings(ApplicationWindow* self, GtkButton* btn) {
  gtk_spinner_start(self->spinner);

  // First we remove the file in case a server instance is already running. This will make it exit.

  auto file_path = std::filesystem::temp_directory_path() / std::filesystem::path{"fastgame.json"};

  std::filesystem::remove(file_path);

  util::debug(log_tag + "removed the file: "s + file_path.string());

  /*
   After a timout of 3 seconds we launch the new server. In case a server instance is already running this should
   give it enough time to exit.
  */

  g_timeout_add_seconds(3, GSourceFunc(+[](ApplicationWindow* self) {
                          save_preset(self, "fastgame", std::filesystem::temp_directory_path());

                          try {
                            std::thread t([]() {
                              // std::system("pkexec fastgame_apply");
                              boost::process::child c(boost::process::search_path("pkexec"), "fastgame_apply");

                              c.wait();
                            });

                            t.detach();

                            gtk_spinner_stop(self->spinner);

                          } catch (std::exception& e) {
                            gtk_spinner_stop(self->spinner);

                            util::warning(log_tag + std::string(e.what()));
                          }
                        }),
                        self);
}

void init_theme_color(ApplicationWindow* self) {
  if (g_settings_get_boolean(self->settings, "use-dark-theme") == 0) {
    adw_style_manager_set_color_scheme(adw_style_manager_get_default(), ADW_COLOR_SCHEME_PREFER_LIGHT);
  } else {
    adw_style_manager_set_color_scheme(adw_style_manager_get_default(), ADW_COLOR_SCHEME_PREFER_DARK);
  }
}

auto setup_icon_theme() -> GtkIconTheme* {
  auto* icon_theme = gtk_icon_theme_get_for_display(gdk_display_get_default());

  if (icon_theme == nullptr) {
    util::warning(log_tag + "can't retrieve the icon theme in use on the system. App icons won't be shown."s);

    return nullptr;
  }

  auto* name = gtk_icon_theme_get_theme_name(icon_theme);

  if (name == nullptr) {
    util::debug(log_tag + "Icon Theme detected, but the name is empty"s);
  } else {
    util::debug(log_tag + "Icon Theme "s + name + " detected"s);

    g_free(name);
  }

  gtk_icon_theme_add_resource_path(icon_theme, "/com/github/wwmm/fastgame/icons");

  return icon_theme;
}

void constructed(GObject* object) {
  auto* self = FG_APP_WINDOW(object);

  self->data->maximized = g_settings_get_boolean(self->settings, "window-maximized") != 0;
  self->data->fullscreen = g_settings_get_boolean(self->settings, "window-fullscreen") != 0;
  self->data->width = g_settings_get_int(self->settings, "window-width");
  self->data->height = g_settings_get_int(self->settings, "window-height");

  gtk_window_set_default_size(GTK_WINDOW(self), self->data->width, self->data->height);

  if (self->data->maximized) {
    gtk_window_maximize(GTK_WINDOW(self));
  }

  if (self->data->fullscreen) {
    gtk_window_fullscreen(GTK_WINDOW(self));
  }

  G_OBJECT_CLASS(application_window_parent_class)->constructed(object);
}

void size_allocate(GtkWidget* widget, int width, int height, int baseline) {
  auto* self = FG_APP_WINDOW(widget);

  GTK_WIDGET_CLASS(application_window_parent_class)->size_allocate(widget, width, height, baseline);

  if (!self->data->maximized && !self->data->fullscreen) {
    gtk_window_get_default_size(GTK_WINDOW(self), &self->data->width, &self->data->height);
  }
}

void surface_state_changed(GtkWidget* widget) {
  auto* self = FG_APP_WINDOW(widget);

  GdkToplevelState new_state = GDK_TOPLEVEL_STATE_MAXIMIZED;

  new_state = gdk_toplevel_get_state(GDK_TOPLEVEL(gtk_native_get_surface(GTK_NATIVE(widget))));

  self->data->maximized = (new_state & GDK_TOPLEVEL_STATE_MAXIMIZED) != 0;
  self->data->fullscreen = (new_state & GDK_TOPLEVEL_STATE_FULLSCREEN) != 0;
}

void realize(GtkWidget* widget) {
  GTK_WIDGET_CLASS(application_window_parent_class)->realize(widget);

  g_signal_connect_swapped(gtk_native_get_surface(GTK_NATIVE(widget)), "notify::state",
                           G_CALLBACK(surface_state_changed), widget);

  auto* self = FG_APP_WINDOW(widget);

  /*
    Getting the gapp pointer here because it is not defined when "init" is called
  */

  self->data->gapp = G_APPLICATION(gtk_window_get_application(GTK_WINDOW(widget)));

  ui::presets_menu::setup(self->presetsMenu, app::FG_APP(self->data->gapp));
}

void unrealize(GtkWidget* widget) {
  g_signal_handlers_disconnect_by_func(gtk_native_get_surface(GTK_NATIVE((widget))),
                                       reinterpret_cast<gpointer>(surface_state_changed), widget);

  GTK_WIDGET_CLASS(application_window_parent_class)->unrealize(widget);
}

void dispose(GObject* object) {
  auto* self = FG_APP_WINDOW(object);

  g_settings_set_int(self->settings, "window-width", self->data->width);
  g_settings_set_int(self->settings, "window-height", self->data->height);
  g_settings_set_boolean(self->settings, "window-maximized", static_cast<gboolean>(self->data->maximized));
  g_settings_set_boolean(self->settings, "window-fullscreen", static_cast<gboolean>(self->data->fullscreen));

  g_object_unref(self->settings);

  util::debug(log_tag + "disposed"s);

  G_OBJECT_CLASS(application_window_parent_class)->dispose(object);
}

void finalize(GObject* object) {
  auto* self = FG_APP_WINDOW(object);

  delete self->data;

  // We remove this file so that the server knows it is time to exit

  auto file_path = std::filesystem::temp_directory_path() / std::filesystem::path{"fastgame.json"};

  std::filesystem::remove(file_path);

  util::debug(log_tag + "removed the file: "s + file_path.string());

  util::debug(log_tag + "finalized"s);

  G_OBJECT_CLASS(application_window_parent_class)->finalize(object);
}

void application_window_class_init(ApplicationWindowClass* klass) {
  auto* object_class = G_OBJECT_CLASS(klass);
  auto* widget_class = GTK_WIDGET_CLASS(klass);

  object_class->constructed = constructed;
  object_class->dispose = dispose;
  object_class->finalize = finalize;

  widget_class->size_allocate = size_allocate;
  widget_class->realize = realize;
  widget_class->unrealize = unrealize;

  gtk_widget_class_set_template_from_resource(widget_class, "/com/github/wwmm/fastgame/ui/application_window.ui");

  gtk_widget_class_bind_template_child(widget_class, ApplicationWindow, stack);
  gtk_widget_class_bind_template_child(widget_class, ApplicationWindow, presets_menu_button);
  gtk_widget_class_bind_template_child(widget_class, ApplicationWindow, game_executable);
  gtk_widget_class_bind_template_child(widget_class, ApplicationWindow, spinner);

  gtk_widget_class_bind_template_callback(widget_class, on_apply_settings);
}

void application_window_init(ApplicationWindow* self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  self->data = new Data();

  self->data->width = -1;
  self->data->height = -1;
  self->data->maximized = false;
  self->data->fullscreen = false;

  self->settings = g_settings_new("com.github.wwmm.fastgame");

  init_theme_color(self);

  self->data->icon_theme = setup_icon_theme();

  self->presetsMenu = ui::presets_menu::create();
  self->environment_variables = ui::environment_variables::create();
  self->cpu = ui::cpu::create();
  self->memory = ui::memory::create();
  self->amdgpu = ui::amdgpu::create();
  self->disk = ui::disk::create();

  auto* page_env = adw_view_stack_add_titled(self->stack, GTK_WIDGET(self->environment_variables),
                                             "environment_variables", _("Environment Variables"));

  auto* page_cpu = adw_view_stack_add_titled(self->stack, GTK_WIDGET(self->cpu), "cpu", _("CPU"));

  auto* page_memory = adw_view_stack_add_titled(self->stack, GTK_WIDGET(self->memory), "memory", _("Memory"));

  auto* page_disk = adw_view_stack_add_titled(self->stack, GTK_WIDGET(self->disk), "disk", _("Disk"));

  if (util::card_is_amdgpu(0)) {
    auto* page_amdgpu = adw_view_stack_add_titled(self->stack, GTK_WIDGET(self->amdgpu), "amdgpu", _("AMD GPU"));

    adw_view_stack_page_set_icon_name(page_amdgpu, "fg-gpu-symbolic");
  }

  adw_view_stack_page_set_icon_name(page_env, "text-x-generic-symbolic");
  adw_view_stack_page_set_icon_name(page_cpu, "fg-cpu-symbolic");
  adw_view_stack_page_set_icon_name(page_memory, "fg-memory-symbolic");
  adw_view_stack_page_set_icon_name(page_disk, "drive-harddisk-symbolic");

  gtk_menu_button_set_popover(self->presets_menu_button, GTK_WIDGET(self->presetsMenu));

  g_settings_bind(self->settings, "autohide-popovers", self->presetsMenu, "autohide", G_SETTINGS_BIND_DEFAULT);

  g_signal_connect(self->settings, "changed::use-dark-theme",
                   G_CALLBACK(+[](GSettings* settings, char* key, ApplicationWindow* self) { init_theme_color(self); }),
                   self);

  ui::presets_menu::save_preset.connect(
      [=](const std::string& name) { save_preset(self, name, ui::presets_menu::user_presets_dir); });

  ui::presets_menu::load_preset.connect([=](const std::string& name) { load_preset(self, name); });
}

auto create(GApplication* gapp) -> ApplicationWindow* {
  return static_cast<ApplicationWindow*>(g_object_new(FG_TYPE_APPLICATION_WINDOW, "application", gapp, nullptr));
}

}  // namespace ui::application_window
