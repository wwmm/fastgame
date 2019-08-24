fastgame_sources = [
	'fastgame.cpp',
	'netlink.cpp',
	'cmdline_options.cpp',
	'config.cpp',
	'scheduler.cpp',
	'tweaks.cpp',
	'nvidia.cpp'
]

cxx = meson.get_compiler('cpp')

libxnvctrl = cxx.find_library('libXNVCtrl', required: true)

fastgame_deps = [
	dependency('boost', version: '>=1.65', modules:['program_options']),
	libxnvctrl,
	dependency('x11')
]

executable(
	meson.project_name(),
	fastgame_sources,
	include_directories : [include_dir,config_h_dir],
	dependencies : fastgame_deps,
	install: true
)

