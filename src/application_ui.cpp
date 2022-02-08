#include "application_ui.hpp"
#include "environment_variables.hpp"

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

  GtkButton* apply_settings;

  GtkSpinner* spinner;

  ui::presets_menu::PresetsMenu* presetsMenu;

  ui::environmental_variables::EnvironmentVariables* environment_variables;

  GSettings* settings;

  Data* data;
};

G_DEFINE_TYPE(ApplicationWindow, application_window, ADW_TYPE_APPLICATION_WINDOW)

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
  gtk_widget_class_bind_template_child(widget_class, ApplicationWindow, apply_settings);
  gtk_widget_class_bind_template_child(widget_class, ApplicationWindow, spinner);
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
  self->environment_variables = ui::environmental_variables::create();

  auto* page_env = adw_view_stack_add_titled(self->stack, GTK_WIDGET(self->environment_variables),
                                             "environment_variables", _("Environment Variables"));

  adw_view_stack_page_set_icon_name(page_env, "text-x-generic-symbolic");

  gtk_menu_button_set_popover(self->presets_menu_button, GTK_WIDGET(self->presetsMenu));

  g_settings_bind(self->settings, "autohide-popovers", self->presetsMenu, "autohide", G_SETTINGS_BIND_DEFAULT);

  g_signal_connect(self->settings, "changed::use-dark-theme",
                   G_CALLBACK(+[](GSettings* settings, char* key, ApplicationWindow* self) { init_theme_color(self); }),
                   self);
}

auto create(GApplication* gapp) -> ApplicationWindow* {
  return static_cast<ApplicationWindow*>(g_object_new(FG_TYPE_APPLICATION_WINDOW, "application", gapp, nullptr));
}

}  // namespace ui::application_window

// ApplicationUi::ApplicationUi(BaseObjectType* cobject,
//                              const Glib::RefPtr<Gtk::Builder>& builder,
//                              Application* application)
//     : Gtk::ApplicationWindow(cobject),
//       app(application),

//   environment_variables = EnvironmentVariables::add_to_stack(stack);
//   cpu = Cpu::add_to_stack(stack);
//   disk = Disk::add_to_stack(stack);

//   if (util::card_is_amdgpu(0)) {
//     amdgpu = Amdgpu::add_to_stack(stack);
//   }

//   memory = Memory::add_to_stack(stack);
//   network = Network::add_to_stack(stack);

//   // signal connection

//   add_preset->signal_clicked().connect([=]() { create_preset(); });

//   import_preset->signal_clicked().connect([=]() { import_preset_file(); });

//   button_apply->signal_clicked().connect([=]() { apply_settings(); });

// }

// void ApplicationUi::save_preset(const std::string& name, const std::filesystem::path& directory) {
//   boost::property_tree::ptree root;
//   boost::property_tree::ptree node;

//   // game executable

//   root.put("game-executable", game_executable->get_text());

//   // environment variables

//   for (const auto& v : environment_variables->get_variables()) {
//     boost::property_tree::ptree local_node;

//     local_node.put("", v);

//     node.push_back(std::make_pair("", local_node));
//   }

//   root.add_child("environment-variables", node);

//   // cpu

//   root.put("cpu.use-batch-scheduler", cpu->get_enable_batch_scheduler());
//   root.put("cpu.child-runs-first", cpu->get_child_runs_first());
//   root.put("cpu.frequency-governor", cpu->get_frequency_governor());
//   root.put("cpu.use-cpu-dma-latency", cpu->get_use_cpu_dma_latency());
//   root.put("cpu.use-realtime-wineserver", cpu->get_use_realtime_wineserver());
//   root.put("cpu.niceness", cpu->get_niceness());

//   node.clear();

//   for (const auto& c : cpu->get_game_cores()) {
//     boost::property_tree::ptree local_node;

//     local_node.put("", c);

//     node.push_back(std::make_pair("", local_node));
//   }

//   root.add_child("cpu.game-cores", node);

//   node.clear();

//   for (const auto& c : cpu->get_workqueue_cores()) {
//     boost::property_tree::ptree local_node;

//     local_node.put("", c);

//     node.push_back(std::make_pair("", local_node));
//   }

//   root.add_child("cpu.workqueue-cores", node);

//   node.clear();

//   for (const auto& c : cpu->get_wineserver_cores()) {
//     boost::property_tree::ptree local_node;

//     local_node.put("", c);

//     node.push_back(std::make_pair("", local_node));
//   }

//   root.add_child("cpu.wineserver-cores", node);

//   // disk

//   root.put("disk.device", disk->get_device());
//   root.put("disk.scheduler", disk->get_scheduler());
//   root.put("disk.enable-realtime-priority", disk->get_enable_realtime_priority());
//   root.put("disk.readahead", disk->get_readahead());
//   root.put("disk.nr-requests", disk->get_nr_requests());
//   root.put("disk.add_random", disk->get_enable_add_random());
//   root.put("disk.udisks.drive-id", disk->get_drive_id());
//   root.put("disk.udisks.supports-apm", disk->get_supports_apm());
//   root.put("disk.udisks.disable-apm", disk->get_disable_apm());
//   root.put("disk.udisks.supports-write-cache", disk->get_supports_write_cache());
//   root.put("disk.udisks.enable-write-cache", disk->get_enable_write_cache());

//   // amdgpu

//   if (amdgpu != nullptr) {
//     root.put("amdgpu.performance-level", amdgpu->get_performance_level());
//     root.put("amdgpu.power-cap", amdgpu->get_power_cap());
//     root.put("amdgpu.irq-affinity", amdgpu->get_irq_affinity());
//   }

//   // memory

//   root.put("memory.virtual-memory.cache-pressure", memory->get_cache_pressure());
//   root.put("memory.virtual-memory.compaction-proactiveness", memory->get_compaction_proactiveness());
//   root.put("memory.transparent-hugepages.enabled", memory->get_thp_enabled());
//   root.put("memory.transparent-hugepages.defrag", memory->get_thp_defrag());
//   root.put("memory.transparent-hugepages.shmem_enabled", memory->get_thp_shmem_enabled());

//   // network

//   root.put("network.ipv4.use_tcp_mtu_probing", network->get_use_tcp_mtu_probing());
//   root.put("network.ipv4.tcp_congestion_control", network->get_tcp_congestion_control());
//   root.put("network.ipv4.tcp_max_reordering", network->get_tcp_max_reordering());
//   root.put("network.ipv4.tcp_probe_interval", network->get_tcp_probe_interval());

//   auto output_file = directory / std::filesystem::path{name + ".json"};

//   boost::property_tree::write_json(output_file, root);

//   util::debug(log_tag + "saved preset: " + output_file.string());
// }

// void ApplicationUi::load_preset(const std::string& name) {
//   auto input_file = user_presets_dir / std::filesystem::path{name + ".json"};

//   boost::property_tree::ptree root;

//   boost::property_tree::read_json(input_file.string(), root);

//   // game executable

//   game_executable->set_text(root.get<std::string>("game-executable", game_executable->get_text()));

//   // environmental variables

//   try {
//     std::vector<std::string> variables_list;

//     for (const auto& c : root.get_child("environment-variables")) {
//       variables_list.emplace_back(c.second.data());
//     }

//     environment_variables->set_variables(variables_list);
//   } catch (const boost::property_tree::ptree_error& e) {
//     util::warning(log_tag + "error when parsing the environmental variables list");
//   }

//   // cpu

//   cpu->set_enable_batch_scheduler(root.get<bool>("cpu.use-batch-scheduler", cpu->get_enable_batch_scheduler()));
//   cpu->set_child_runs_first(root.get<bool>("cpu.child-runs-first", cpu->get_child_runs_first()));
//   cpu->set_frequency_governor(root.get<std::string>("cpu.frequency-governor", cpu->get_frequency_governor()));
//   cpu->set_use_cpu_dma_latency(root.get<bool>("cpu.use-cpu-dma-latency", cpu->get_use_cpu_dma_latency()));
//   cpu->set_use_realtime_wineserver(root.get<bool>("cpu.use-realtime-wineserver",
//   cpu->get_use_realtime_wineserver())); cpu->set_niceness(root.get<int>("cpu.niceness", cpu->get_niceness()));

//   try {
//     std::vector<std::string> cores_list;

//     for (const auto& c : root.get_child("cpu.game-cores")) {
//       cores_list.emplace_back(c.second.data());
//     }

//     cpu->set_game_cores(cores_list);

//     cores_list.clear();

//     for (const auto& c : root.get_child("cpu.workqueue-cores")) {
//       cores_list.emplace_back(c.second.data());
//     }

//     cpu->set_workqueue_cores(cores_list);

//     cores_list.clear();

//     for (const auto& c : root.get_child("cpu.wineserver-cores")) {
//       cores_list.emplace_back(c.second.data());
//     }

//     cpu->set_wineserver_cores(cores_list);
//   } catch (const boost::property_tree::ptree_error& e) {
//     util::warning(log_tag + "error when parsing the cpu core list");
//   }

//   // disk

//   disk->set_device(root.get<std::string>("disk.device", disk->get_device()));
//   disk->set_scheduler(root.get<std::string>("disk.scheduler", disk->get_scheduler()));
//   disk->set_enable_realtime_priority(
//       root.get<bool>("disk.enable-realtime-priority", disk->get_enable_realtime_priority()));
//   disk->set_readahead(root.get<int>("disk.readahead", disk->get_readahead()));
//   disk->set_nr_requests(root.get<int>("disk.nr-requests", disk->get_nr_requests()));
//   disk->set_enable_add_random(root.get<bool>("disk.add_random", disk->get_enable_add_random()));
//   disk->set_disable_apm(root.get<bool>("disk.udisks.disable-apm", disk->get_disable_apm()));
//   disk->set_enable_write_cache(root.get<bool>("disk.udisks.enable-write-cache", disk->get_enable_write_cache()));

//   // amdgpu

//   if (amdgpu != nullptr) {
//     amdgpu->set_performance_level(root.get<std::string>("amdgpu.performance-level",
//     amdgpu->get_performance_level())); amdgpu->set_power_cap(root.get<int>("amdgpu.power-cap",
//     amdgpu->get_power_cap())); amdgpu->set_irq_affinity(root.get<int>("amdgpu.irq-affinity",
//     amdgpu->get_irq_affinity()));
//   }

//   // memory

//   memory->set_cache_pressure(root.get<int>("memory.virtual-memory.cache-pressure", memory->get_cache_pressure()));
//   memory->set_compaction_proactiveness(
//       root.get<int>("memory.virtual-memory.compaction-proactiveness", memory->get_compaction_proactiveness()));
//   memory->set_thp_enabled(root.get<std::string>("memory.transparent-hugepages.enabled", memory->get_thp_enabled()));
//   memory->set_thp_defrag(root.get<std::string>("memory.transparent-hugepages.defrag", memory->get_thp_defrag()));
//   memory->set_thp_shmem_enabled(
//       root.get<std::string>("memory.transparent-hugepages.shmem_enabled", memory->get_thp_shmem_enabled()));

//   // network

//   network->set_use_tcp_mtu_probing(
//       root.get<bool>("network.ipv4.use_tcp_mtu_probing", network->get_use_tcp_mtu_probing()));
//   network->set_tcp_congestion_control(
//       root.get<std::string>("network.ipv4.tcp_congestion_control", network->get_tcp_congestion_control()));
//   network->set_tcp_max_reordering(root.get<int>("network.ipv4.tcp_max_reordering",
//   network->get_tcp_max_reordering()));
//   network->set_tcp_probe_interval(root.get<int>("network.ipv4.tcp_probe_interval",
//   network->get_tcp_probe_interval()));

//   util::debug(log_tag + "loaded preset: " + input_file.string());
// }

// void ApplicationUi::create_preset() {
//   std::string name = preset_name->get_text();

//   if (!name.empty()) {
//     std::string illegalChars = "\\/";

//     for (auto it = name.begin(); it < name.end(); ++it) {
//       bool found = illegalChars.find(*it) != std::string::npos;

//       if (found) {
//         preset_name->set_text("");

//         return;
//       }
//     }

//     preset_name->set_text("");

//     for (auto& used_name : get_presets_names()) {
//       if (used_name == name) {
//         return;
//       }
//     }

//     save_preset(name, user_presets_dir);

//     populate_listbox();
//   }
// }

// void ApplicationUi::populate_listbox() {
//   auto children = presets_listbox->get_children();

//   for (const auto& c : children) {
//     presets_listbox->remove(*c);
//   }

//   auto names = get_presets_names();

//   for (const auto& name : names) {
//     auto b = Gtk::Builder::create_from_resource("/com/github/wwmm/fastgame/ui/preset_row.glade");

//     Gtk::ListBoxRow* row = nullptr;
//     Gtk::Button* apply_btn = nullptr;
//     Gtk::Button* save_btn = nullptr;
//     Gtk::Button* remove_btn = nullptr;
//     Gtk::Label* label = nullptr;

//     b->get_widget("preset_row", row);
//     b->get_widget("apply", apply_btn);
//     b->get_widget("save", save_btn);
//     b->get_widget("remove", remove_btn);
//     b->get_widget("name", label);

//     row->set_name(name);

//     label->set_text(name);

//     connections.emplace_back(apply_btn->signal_clicked().connect([=]() { load_preset(name); }));

//     connections.emplace_back(save_btn->signal_clicked().connect([=]() { save_preset(name, user_presets_dir); }));

//     connections.emplace_back(remove_btn->signal_clicked().connect([=]() {
//       auto file_path = user_presets_dir / std::filesystem::path{name + ".json"};

//       std::filesystem::remove(file_path);

//       util::debug(log_tag + "removed preset file: " + file_path.string());

//       populate_listbox();
//     }));

//     presets_listbox->add(*row);
//     presets_listbox->show_all();
//   }
// }

// void ApplicationUi::apply_settings() {
//   headerbar_spinner->start();

//   // First we remove the file in case a server instance is already running. This will make it exit.

//   auto file_path = std::filesystem::temp_directory_path() / std::filesystem::path{"fastgame.json"};

//   std::filesystem::remove(file_path);

//   util::debug(log_tag + "removed the file: " + file_path.string());

//   /*
//      After a timout of 3 seconds we launch the new server. In case a server instance is already running this should
//      give it enough time to exit.
//   */

//   Glib::signal_timeout().connect_seconds_once(
//       [=]() {
//         save_preset("fastgame", std::filesystem::temp_directory_path());

//         try {
//           std::thread t([]() {
//             // std::system("pkexec fastgame_apply");
//             boost::process::child c(boost::process::search_path("pkexec"), "fastgame_apply");

//             c.wait();
//           });

//           t.detach();

//           headerbar_spinner->stop();
//         } catch (std::exception& e) {
//           headerbar_spinner->stop();

//           util::warning(log_tag + e.what());
//         }
//       },
//       3);
// }