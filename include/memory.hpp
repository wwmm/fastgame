#pragma once

#include <adwaita.h>
#include <iostream>

namespace ui::memory {

G_BEGIN_DECLS

#define FG_TYPE_MEMORY (memory_get_type())

G_DECLARE_FINAL_TYPE(Memory, memory, FG, MEMORY, GtkBox)

G_END_DECLS

auto create() -> Memory*;

void set_swappiness(Memory* self, const int& value);

auto get_swappiness(Memory* self) -> int;

void set_cache_pressure(Memory* self, const int& value);

auto get_cache_pressure(Memory* self) -> int;

void set_compaction_proactiveness(Memory* self, const int& value);

auto get_compaction_proactiveness(Memory* self) -> int;

void set_min_free_kbytes(Memory* self, const int& value);

auto get_min_free_kbytes(Memory* self) -> int;

void set_page_lock_unfairness(Memory* self, const int& value);

auto get_page_lock_unfairness(Memory* self) -> int;

void set_percpu_pagelist_high_fraction(Memory* self, const int& value);

auto get_percpu_pagelist_high_fraction(Memory* self) -> int;

void set_mglru_min_ttl_ms(Memory* self, const int& value);

auto get_mglru_min_ttl_ms(Memory* self) -> int;

void set_thp_enabled(Memory* self, const std::string& name);

auto get_thp_enabled(Memory* self) -> std::string;

void set_thp_defrag(Memory* self, const std::string& name);

auto get_thp_defrag(Memory* self) -> std::string;

void set_thp_shmem_enabled(Memory* self, const std::string& name);

auto get_thp_shmem_enabled(Memory* self) -> std::string;

void set_scan_sleep(Memory* self, const int& value);

auto get_scan_sleep(Memory* self) -> int;

void set_alloc_sleep(Memory* self, const int& value);

auto get_alloc_sleep(Memory* self) -> int;

}  // namespace ui::memory
