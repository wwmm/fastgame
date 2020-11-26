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
	'application.cpp',
	'application_ui.cpp',
	'environment_variables.cpp',
	'amdgpu.cpp',
	'memory.cpp',
	'cpu.cpp',
	'disk.cpp',
	'network.cpp',
	'util.cpp',
	gresources
]

fastgame_deps = [
	dependency('glib-2.0', version: '>=2.56'),
	dependency('glibmm-2.4', version: '>=2.56'),
	dependency('gtk+-3.0', version: '>=3.20'),
	dependency('gtkmm-3.0', version: '>=3.24'),
	dependency('sigc++-2.0', version: ['>=2.10', '<3']),
	dependency('boost', version: '>=1.65', modules:['filesystem']),
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
	dependency('threads')
]

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
	install: true
)

executable(
	'fastgame_apply',
	fastgame_apply_sources,
	include_directories : [include_dir,config_h_dir],
	dependencies : fastgame_apply_deps,
	install: true
)
