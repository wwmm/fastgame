#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <giomm/settings.h>
#include <gtkmm/application.h>

class Application : public Gtk::Application {
 public:
  Application();
  Application(const Application&) = delete;
  auto operator=(const Application&) -> Application& = delete;
  Application(const Application&&) = delete;
  auto operator=(const Application &&) -> Application& = delete;
  ~Application() override;

  static auto create() -> Glib::RefPtr<Application>;
  Glib::RefPtr<Gio::Settings> settings;

 protected:
  void on_startup() override;
  void on_activate() override;

 private:
  std::string log_tag = "application: ";

  void create_actions();
};

#endif
