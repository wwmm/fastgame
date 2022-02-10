#pragma once

#include <adwaita.h>
#include <glib/gi18n.h>
#include <sigc++/sigc++.h>
#include <filesystem>
#include <string>
#include "config.h"
#include "util.hpp"

namespace app {

G_BEGIN_DECLS

#define FG_TYPE_APPLICATION (application_get_type())

G_DECLARE_FINAL_TYPE(Application, application, FG, APP, AdwApplication)

G_END_DECLS

struct Data {
 public:
  std::vector<sigc::connection> connections;

  std::vector<gulong> gconnections, gconnections_sie, gconnections_soe;
};

struct _Application {
  AdwApplication parent_instance;

  GSettings* settings;

  Data* data;
};

auto application_new() -> GApplication*;

void hide_all_windows(GApplication* app);

}  // namespace app
