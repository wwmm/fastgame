#include "application_ui.hpp"
#include <glibmm/i18n.h>
#include <glibmm/miscutils.h>
#include <gtkmm/dialog.h>
#include <gtkmm/filechoosernative.h>
#include <gtkmm/icontheme.h>
#include <gtkmm/settings.h>
#include <boost/process.hpp>
#include <boost/process/search_path.hpp>
#include <boost/process/spawn.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <filesystem>
#include "glibmm/main.h"
#include "util.hpp"

ApplicationUi::ApplicationUi(BaseObjectType* cobject,
                             const Glib::RefPtr<Gtk::Builder>& builder,
                             Application* application)
    : Gtk::ApplicationWindow(cobject),
      app(application),
      settings(app->settings),
      user_presets_dir(Glib::get_user_config_dir() + "/fastgame") {
  Gtk::IconTheme::get_default()->add_resource_path("/com/github/wwmm/fastgame/icons");

  // loading glade widgets

  builder->get_widget("stack", stack);
  builder->get_widget("add_preset", add_preset);
  builder->get_widget("import_preset", import_preset);
  builder->get_widget("preset_name", preset_name);
  builder->get_widget("presets_listbox", presets_listbox);
  builder->get_widget("presets_menu_button", presets_menu_button);
  builder->get_widget("presets_menu_scrolled_window", presets_menu_scrolled_window);
  builder->get_widget("button_apply", button_apply);
  builder->get_widget("headerbar_spinner", headerbar_spinner);
  builder->get_widget("game_executable", game_executable);

  create_user_directory();

  environment_variables = EnvironmentVariables::add_to_stack(stack);
  cpu = Cpu::add_to_stack(stack);
  disk = Disk::add_to_stack(stack);
  amdgpu = Amdgpu::add_to_stack(stack);
  memory = Memory::add_to_stack(stack);

  // binding glade widgets to gsettings keys

  auto flag = Gio::SettingsBindFlags::SETTINGS_BIND_DEFAULT;

  settings->bind("use-dark-theme", Gtk::Settings::get_default().get(), "gtk_application_prefer_dark_theme", flag);

  // signal connection

  add_preset->signal_clicked().connect([=]() { create_preset(); });

  import_preset->signal_clicked().connect([=]() { import_preset_file(); });

  presets_listbox->set_sort_func(sigc::ptr_fun(&ApplicationUi::on_listbox_sort));

  presets_menu_button->signal_clicked().connect(sigc::mem_fun(*this, &ApplicationUi::on_presets_menu_button_clicked));

  button_apply->signal_clicked().connect([=]() { apply_settings(); });

  // restore the window size

  auto window_width = settings->get_int("window-width");
  auto window_height = settings->get_int("window-height");

  if (window_width > 0 && window_height > 0) {
    set_default_size(window_width, window_height);
  }
}

ApplicationUi::~ApplicationUi() {
  for (auto& c : connections) {
    c.disconnect();
  }

  // We remove this file so that the server knows it is time to exit

  auto file_path = std::filesystem::temp_directory_path() / std::filesystem::path{"fastgame.json"};

  std::filesystem::remove(file_path);

  util::debug(log_tag + "removed the file: " + file_path.string());

  util::debug(log_tag + "destroyed");
}

auto ApplicationUi::create(Application* app_this) -> ApplicationUi* {
  auto builder = Gtk::Builder::create_from_resource("/com/github/wwmm/fastgame/ui/application.glade");

  ApplicationUi* window = nullptr;

  builder->get_widget_derived("ApplicationUi", window, app_this);

  return window;
}

void ApplicationUi::create_user_directory() {
  if (!std::filesystem::is_directory(user_presets_dir)) {
    if (std::filesystem::create_directories(user_presets_dir)) {
      util::debug(log_tag + "user presets directory created: " + user_presets_dir.string());
    } else {
      util::warning(log_tag + "failed to create user presets directory: " + user_presets_dir.string());
    }
  } else {
    util::debug(log_tag + "user presets directory already exists: " + user_presets_dir.string());
  }
}

auto ApplicationUi::get_presets_names() -> std::vector<std::string> {
  std::vector<std::string> names;

  for (const auto& entry : std::filesystem::directory_iterator(user_presets_dir)) {
    names.emplace_back(entry.path().stem().string());
  }

  return names;
}

void ApplicationUi::save_preset(const std::string& name, const std::filesystem::path& directory) {
  boost::property_tree::ptree root;
  boost::property_tree::ptree node;

  // game executable

  root.put("game-executable", game_executable->get_text());

  // environment variables

  for (const auto& v : environment_variables->get_variables()) {
    boost::property_tree::ptree local_node;

    local_node.put("", v);

    node.push_back(std::make_pair("", local_node));
  }

  root.add_child("environment-variables", node);

  // cpu

  root.put("cpu.use-batch-scheduler", cpu->get_enable_batch_scheduler());
  root.put("cpu.child-runs-first", cpu->get_child_runs_first());
  root.put("cpu.frequency-governor", cpu->get_frequency_governor());
  root.put("cpu.use-cpu-dma-latency", cpu->get_use_cpu_dma_latency());
  root.put("cpu.use-realtime-wineserver", cpu->get_use_realtime_wineserver());
  root.put("cpu.niceness", cpu->get_niceness());

  node.clear();

  for (const auto& c : cpu->get_cores()) {
    boost::property_tree::ptree local_node;

    local_node.put("", c);

    node.push_back(std::make_pair("", local_node));
  }

  root.add_child("cpu.cores", node);

  // disk

  root.put("disk.device", disk->get_device());
  root.put("disk.scheduler", disk->get_scheduler());
  root.put("disk.enable-realtime-priority", disk->get_enable_realtime_priority());
  root.put("disk.readahead", disk->get_readahead());
  root.put("disk.nr-requests", disk->get_nr_requests());

  // amdgpu

  root.put("amdgpu.performance-level", amdgpu->get_performance_level());
  root.put("amdgpu.power-cap", amdgpu->get_power_cap());

  // memory

  root.put("memory.virtual-memory.cache-pressure", memory->get_cache_pressure());
  root.put("memory.virtual-memory.compaction-proactiveness", memory->get_compaction_proactiveness());
  root.put("memory.transparent-hugepages.enabled", memory->get_thp_enabled());
  root.put("memory.transparent-hugepages.defrag", memory->get_thp_defrag());
  root.put("memory.transparent-hugepages.shmem_enabled", memory->get_thp_shmem_enabled());

  auto output_file = directory / std::filesystem::path{name + ".json"};

  boost::property_tree::write_json(output_file, root);

  util::debug(log_tag + "saved preset: " + output_file.string());
}

void ApplicationUi::load_preset(const std::string& name) {
  auto input_file = user_presets_dir / std::filesystem::path{name + ".json"};

  boost::property_tree::ptree root;

  boost::property_tree::read_json(input_file.string(), root);

  // game executable

  game_executable->set_text(root.get<std::string>("game-executable", game_executable->get_text()));

  // environmental variables

  try {
    std::vector<std::string> variables_list;

    for (const auto& c : root.get_child("environment-variables")) {
      variables_list.emplace_back(c.second.data());
    }

    environment_variables->set_variables(variables_list);
  } catch (const boost::property_tree::ptree_error& e) {
    util::warning(log_tag + "error when parsing the environmental variables list");
  }

  // cpu

  cpu->set_enable_batch_scheduler(root.get<bool>("cpu.use-batch-scheduler", cpu->get_enable_batch_scheduler()));
  cpu->set_child_runs_first(root.get<bool>("cpu.child-runs-first", cpu->get_child_runs_first()));
  cpu->set_frequency_governor(root.get<std::string>("cpu.frequency-governor", cpu->get_frequency_governor()));
  cpu->set_use_cpu_dma_latency(root.get<bool>("cpu.use-cpu-dma-latency", cpu->get_use_cpu_dma_latency()));
  cpu->set_use_realtime_wineserver(root.get<bool>("cpu.use-realtime-wineserver", cpu->get_use_realtime_wineserver()));
  cpu->set_niceness(root.get<int>("cpu.niceness", cpu->get_niceness()));

  try {
    std::vector<std::string> cores_list;

    for (const auto& c : root.get_child("cpu.cores")) {
      cores_list.emplace_back(c.second.data());
    }

    cpu->set_cores(cores_list);
  } catch (const boost::property_tree::ptree_error& e) {
    util::warning(log_tag + "error when parsing the cpu core list");
  }

  // disk

  disk->set_device(root.get<std::string>("disk.device", disk->get_device()));
  disk->set_scheduler(root.get<std::string>("disk.scheduler", disk->get_scheduler()));
  disk->set_enable_realtime_priority(
      root.get<bool>("disk.enable-realtime-priority", disk->get_enable_realtime_priority()));
  disk->set_readahead(root.get<int>("disk.readahead", disk->get_readahead()));
  disk->set_nr_requests(root.get<int>("disk.nr-requests", disk->get_nr_requests()));

  // amdgpu

  amdgpu->set_performance_level(root.get<std::string>("amdgpu.performance-level", amdgpu->get_performance_level()));
  amdgpu->set_power_cap(root.get<int>("amdgpu.power-cap", amdgpu->get_power_cap()));

  // memory

  memory->set_cache_pressure(root.get<int>("memory.virtual-memory.cache-pressure", memory->get_cache_pressure()));
  memory->set_compaction_proactiveness(
      root.get<int>("memory.virtual-memory.compaction-proactiveness", memory->get_compaction_proactiveness()));
  memory->set_thp_enabled(root.get<std::string>("memory.transparent-hugepages.enabled", memory->get_thp_enabled()));
  memory->set_thp_defrag(root.get<std::string>("memory.transparent-hugepages.defrag", memory->get_thp_defrag()));
  memory->set_thp_shmem_enabled(
      root.get<std::string>("memory.transparent-hugepages.shmem_enabled", memory->get_thp_shmem_enabled()));

  util::debug(log_tag + "loaded preset: " + input_file.string());
}

void ApplicationUi::create_preset() {
  std::string name = preset_name->get_text();

  if (!name.empty()) {
    std::string illegalChars = "\\/";

    for (auto it = name.begin(); it < name.end(); ++it) {
      bool found = illegalChars.find(*it) != std::string::npos;

      if (found) {
        preset_name->set_text("");

        return;
      }
    }

    preset_name->set_text("");

    for (auto& used_name : get_presets_names()) {
      if (used_name == name) {
        return;
      }
    }

    save_preset(name, user_presets_dir);

    populate_listbox();
  }
}

void ApplicationUi::import_preset_file() {
  auto* main_window = dynamic_cast<Gtk::Window*>(this->get_toplevel());

  auto dialog = Gtk::FileChooserNative::create(
      _("Import Presets"), *main_window, Gtk::FileChooserAction::FILE_CHOOSER_ACTION_OPEN, _("Open"), _("Cancel"));

  auto dialog_filter = Gtk::FileFilter::create();

  dialog_filter->set_name(_("Presets"));
  dialog_filter->add_pattern("*.json");

  dialog->add_filter(dialog_filter);

  dialog->signal_response().connect([=](auto response_id) {
    switch (response_id) {
      case Gtk::ResponseType::RESPONSE_ACCEPT: {
        for (const auto& file_name : dialog->get_filenames()) {
          auto file_path = std::filesystem::path{file_name};

          if (std::filesystem::is_regular_file(file_path)) {
            auto output_path = user_presets_dir / std::filesystem::path{file_path.filename()};

            std::filesystem::copy_file(file_path, output_path, std::filesystem::copy_options::overwrite_existing);

            util::debug(log_tag + "imported preset to: " + output_path.string());
          }
        }

        populate_listbox();

        break;
      }
      default:
        break;
    }
  });

  dialog->set_modal(true);
  dialog->set_select_multiple(true);
  dialog->show();
}

auto ApplicationUi::on_listbox_sort(Gtk::ListBoxRow* row1, Gtk::ListBoxRow* row2) -> int {
  auto name1 = row1->get_name();
  auto name2 = row2->get_name();

  std::vector<std::string> names = {name1, name2};

  std::sort(names.begin(), names.end());

  if (name1 == names[0]) {
    return -1;
  }
  if (name2 == names[0]) {
    return 1;
  }

  return 0;
}

void ApplicationUi::populate_listbox() {
  auto children = presets_listbox->get_children();

  for (const auto& c : children) {
    presets_listbox->remove(*c);
  }

  auto names = get_presets_names();

  for (const auto& name : names) {
    auto b = Gtk::Builder::create_from_resource("/com/github/wwmm/fastgame/ui/preset_row.glade");

    Gtk::ListBoxRow* row = nullptr;
    Gtk::Button* apply_btn = nullptr;
    Gtk::Button* save_btn = nullptr;
    Gtk::Button* remove_btn = nullptr;
    Gtk::Label* label = nullptr;

    b->get_widget("preset_row", row);
    b->get_widget("apply", apply_btn);
    b->get_widget("save", save_btn);
    b->get_widget("remove", remove_btn);
    b->get_widget("name", label);

    row->set_name(name);

    label->set_text(name);

    connections.emplace_back(apply_btn->signal_clicked().connect([=]() { load_preset(name); }));

    connections.emplace_back(save_btn->signal_clicked().connect([=]() { save_preset(name, user_presets_dir); }));

    connections.emplace_back(remove_btn->signal_clicked().connect([=]() {
      auto file_path = user_presets_dir / std::filesystem::path{name + ".json"};

      std::filesystem::remove(file_path);

      util::debug(log_tag + "removed preset file: " + file_path.string());

      populate_listbox();
    }));

    presets_listbox->add(*row);
    presets_listbox->show_all();
  }
}

void ApplicationUi::on_presets_menu_button_clicked() {
  auto* parent = dynamic_cast<Gtk::ApplicationWindow*>(this->get_toplevel());
  const float scaling_factor = 0.7F;

  int height = static_cast<int>(scaling_factor * static_cast<float>(parent->get_allocated_height()));

  presets_menu_scrolled_window->set_max_content_height(height);

  populate_listbox();
}

void ApplicationUi::apply_settings() {
  headerbar_spinner->start();

  // First we remove the file in case a server instance is already running. This will make it exit.

  auto file_path = std::filesystem::temp_directory_path() / std::filesystem::path{"fastgame.json"};

  std::filesystem::remove(file_path);

  util::debug(log_tag + "removed the file: " + file_path.string());

  /*
     After a timout of 3 seconds we launch the new server. In case a server instance is already running this should
     give it enough time to exit.
  */

  Glib::signal_timeout().connect_seconds_once(
      [=]() {
        save_preset("fastgame", std::filesystem::temp_directory_path());

        try {
          std::thread t([]() {
            // std::system("pkexec fastgame_apply");
            boost::process::child c(boost::process::search_path("pkexec"), "fastgame_apply");

            c.wait();
          });

          t.detach();

          headerbar_spinner->stop();
        } catch (std::exception& e) {
          headerbar_spinner->stop();

          util::warning(log_tag + e.what());
        }
      },
      3);
}