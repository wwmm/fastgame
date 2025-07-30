#include "scx_sched.hpp"
#include <qabstractitemmodel.h>
#include <qbytearray.h>
#include <qhash.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqml.h>
#include <qtmetamacros.h>
#include <qvariant.h>
#include <array>
#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/readable_pipe.hpp>
#include <boost/process/v2/environment.hpp>
#include <boost/process/v2/process.hpp>
#include <boost/process/v2/stdio.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <cstddef>
#include <exception>
#include <regex>
#include <string>
#include <vector>
#include "combobox_model.hpp"
#include "config.h"
#include "util.hpp"

namespace scx_sched {

Backend::Backend(QObject* parent) : QObject(parent) {
  qmlRegisterSingletonInstance<Backend>("ScxSchedBackend", VERSION_MAJOR, VERSION_MINOR, "ScxSchedBackend", this);

  qmlRegisterSingletonInstance<ComboBoxModel>("FGModelScxSched", VERSION_MAJOR, VERSION_MINOR, "FGModelScxSched",
                                              &schedulerModel);

  init_scheduler();
}

auto Backend::enable() const -> bool {
  return _enable;
}

void Backend::setEnable(const bool& value) {
  _enable = value;

  Q_EMIT enableChanged();
}

auto Backend::scheduler() -> std::string {
  return !schedulerModel.getList().isEmpty() ? schedulerModel.getValue(_scheduler).toStdString() : "";
}

void Backend::setScheduler(const std::string& value) {
  auto id = schedulerModel.getId(QString::fromStdString(value));

  if (id == -1) {
    return;
  }
  _scheduler = id;

  Q_EMIT schedulerChanged();
}

void Backend::init_scheduler() {
  try {
    boost::asio::io_context ctx;
    boost::asio::readable_pipe rp{ctx};
    std::array<char, 4096> buffer;

    auto exe = boost::process::environment::find_executable("scxctl");

    if (exe.empty()) {
      util::warning("Could not find the command scxctl");

      return;
    }

    auto available_scheds =
        boost::process::process(ctx, exe, {"list"}, boost::process::process_stdio{.in = {}, .out = rp, .err = {}});

    std::string scxctl_output;

    while (true) {
      boost::system::error_code ec;

      std::size_t n = rp.read_some(boost::asio::buffer(buffer), ec);

      if (ec == boost::asio::error::eof) {
        break;
      } else if (ec) {
        throw boost::system::system_error(ec);
      }

      scxctl_output.append(buffer.data(), n);
    }

    available_scheds.wait();

    std::vector<std::string> schedulers;

    std::regex expr("\"([^\"]+)\"");
    std::smatch match;

    auto begin = scxctl_output.cbegin();
    auto end = scxctl_output.cend();

    while (std::regex_search(begin, end, match, expr)) {
      schedulers.push_back(match[1]);  // match[1] = inside quotes
      begin = match[0].second;         // move past the current match
    }

    for (const auto& s : schedulers) {
      schedulerModel.append(QString::fromStdString(s));
    }

    if (!schedulerModel.getList().empty()) {
      setScheduler(schedulers[0]);
    }

  } catch (std::exception& e) {
    util::warning(e.what());
  }

  // for some reason when scxctl is called for the first time it tries to load its default scheduler
  util::disable_scx_sched();
}

}  // namespace scx_sched