#pragma once

#include <adwaita.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "util.hpp"

namespace ui::memory {

G_BEGIN_DECLS

#define FG_TYPE_MEMORY (memory_get_type())

G_DECLARE_FINAL_TYPE(Memory, memory, FG, MEMORY, GtkBox)

G_END_DECLS

auto create() -> Memory*;

}  // namespace ui::memory

// class Memory : public Gtk::Grid {
//  public:
//   Memory(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
//   Memory(const Memory&) = delete;
//   auto operator=(const Memory&) -> Memory& = delete;
//   Memory(const Memory&&) = delete;
//   auto operator=(const Memory &&) -> Memory& = delete;
//   ~Memory() override;

//   static auto add_to_stack(Gtk::Stack* stack) -> Memory*;

//   auto get_cache_pressure() -> int;

//   void set_cache_pressure(const int& value);

//   auto get_thp_enabled() -> std::string;

//   void set_thp_enabled(const std::string& value);

//   auto get_thp_defrag() -> std::string;

//   void set_thp_defrag(const std::string& value);

//   auto get_thp_shmem_enabled() -> std::string;

//   void set_thp_shmem_enabled(const std::string& value);

//   auto get_compaction_proactiveness() -> int;

//   void set_compaction_proactiveness(const int& value);

//  private:
//   std::string log_tag = "memory: ";

//   Gtk::ComboBoxText *thp_enabled = nullptr, *thp_defrag = nullptr, *thp_shmem_enabled = nullptr;

//   Glib::RefPtr<Gtk::Adjustment> cache_pressure, compaction_proactiveness;

//   void read_transparent_huge_page_values();
// };

// #endif
