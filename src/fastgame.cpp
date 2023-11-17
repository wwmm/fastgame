#include <glib-unix.h>
#include "application.hpp"
#include "application_ui.hpp"
#include "config.h"

auto sigterm(void* data) -> int {
  auto* app = G_APPLICATION(data);

  app::hide_all_windows(app);

  g_application_quit(app);

  return G_SOURCE_REMOVE;
}

auto main(int argc, char* argv[]) -> int {
  util::debug("fastgame version: " + std::string(VERSION));

  try {
    // Init internationalization support before anything else

    auto* bindtext_output = bindtextdomain(GETTEXT_PACKAGE, LOCALE_DIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);

    if (bindtext_output != nullptr) {
      util::debug("main: locale directory: " + std::string(bindtext_output));
    } else if (errno == ENOMEM) {
      util::warning("main: bindtextdomain: Not enough memory available!");

      return errno;
    }

    auto* app = app::application_new();

    g_unix_signal_add(2, (GSourceFunc)sigterm, app);

    auto status = g_application_run(app, argc, argv);

    g_object_unref(app);

    util::debug("Exitting the main function with status: " + std::to_string(status));

    return status;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;

    return EXIT_FAILURE;
  }
}
