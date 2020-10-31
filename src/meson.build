subdir('nvidia')

fastgame_server_sources = [
	'fastgame_server.cpp',
	'netlink.cpp',
	'cmdline_options.cpp',
	'config.cpp',
	'scheduler.cpp',
	'tweaks.cpp',
	'radeon.cpp'
]

fastgame_server_deps = [
	dependency('boost', version: '>=1.65', modules:['program_options'])
]

fastgame_sources = [
	'fastgame.cpp',
	'cmdline_options.cpp',
	'config.cpp',
	'scheduler.cpp'
]

fastgame_deps = [
	dependency('boost', version: '>=1.65', modules:['program_options'])
]

fastgame_gui_sources = [
	'fastgame_gui.cpp',
	'application.cpp',
	'application_ui.cpp',
	'environment_variables.cpp',
	'amdgpu.cpp',
	'memory.cpp',
	'cpu.cpp',
	'util.cpp',
	gresources
]

fastgame_gui_deps = [
	dependency('glib-2.0', version: '>=2.56'),
	dependency('glibmm-2.4', version: '>=2.56'),
	dependency('gtk+-3.0', version: '>=3.20'),
	dependency('gtkmm-3.0', version: '>=3.24'),
	dependency('sigc++-2.0', version: ['>=2.10', '<3'])
]

linkwith = []

if libxnvctrl.found()
	linkwith += fastgame_nvidia
endif

executable(
	'fastgame_server',
	fastgame_server_sources,
	include_directories : [include_dir,config_h_dir],
	dependencies : fastgame_server_deps,
	install: true,
	link_with: linkwith
)

executable(
	'fastgame',
	fastgame_sources,
	include_directories : [include_dir,config_h_dir],
	dependencies : fastgame_deps,
	install: true
)

executable(
	'fastgame_gui',
	fastgame_gui_sources,
	include_directories : [include_dir,config_h_dir],
	dependencies : fastgame_gui_deps,
	install: true
)
