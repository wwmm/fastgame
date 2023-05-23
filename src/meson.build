fastgame_launcher_sources = [
	'fastgame_launcher.cpp',
	'util.cpp'
]

fastgame_launcher_deps = [
	dependency('glib-2.0', version: '>=2.56'),
	dependency('boost', version: '>=1.65', modules:['program_options'])
]

fastgame_sources = [
	'fastgame.cpp',
	'amdgpu.cpp',
	'application.cpp',
	'application_ui.cpp',
	'cpu.cpp',
	'disk.cpp',
	'environment_variable_holder.cpp',
	'environment_variables.cpp',
	'memory.cpp',
	'nvidia.cpp',
	'preferences_general.cpp',
	'preferences_window.cpp',
	'presets_menu.cpp',
	'ui_helpers.cpp',
	'util.cpp',
	gresources
]

fastgame_deps = [
	dependency('glib-2.0', version: '>=2.56'),
	dependency('gtk4', version: '>=4.2.1'),
	dependency('libadwaita-1', version: '>=1.0.0'),
	dependency('sigc++-3.0', version: '>=3.0.6'),
	dependency('boost', version: '>=1.65', modules:['filesystem']),
	dependency('fmt'),
	dependency('threads'),
	dependency('udisks2')
]

fastgame_apply_sources = [
	'fastgame_apply.cpp',
	'netlink.cpp',
	'util.cpp'
]

fastgame_apply_deps = [
	dependency('glib-2.0', version: '>=2.56'),
	dependency('boost', version: '>=1.65'),
	dependency('threads'),
	dependency('udisks2')
]

subdir('nvidia')

linkwith = []

if libxnvctrl.found()
	linkwith += fastgame_nvidia
endif

executable(
	'fastgame_launcher',
	fastgame_launcher_sources,
	include_directories : [include_dir,config_h_dir],
	dependencies : fastgame_launcher_deps,
	install: true
)

executable(
	'fastgame',
	fastgame_sources,
	include_directories : [include_dir,config_h_dir],
	dependencies : fastgame_deps,
	install: true,
	link_with: linkwith
)

executable(
	'fastgame_apply',
	fastgame_apply_sources,
	include_directories : [include_dir,config_h_dir],
	dependencies : fastgame_apply_deps,
	install: true,
	link_with: linkwith
)
