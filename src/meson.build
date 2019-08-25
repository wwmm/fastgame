subdir('nvidia')

fastgame_sources = [
	'fastgame.cpp',
	'netlink.cpp',
	'cmdline_options.cpp',
	'config.cpp',
	'scheduler.cpp',
	'tweaks.cpp'
]

fastgame_deps = [
	dependency('boost', version: '>=1.65', modules:['program_options'])
]

linkwith = []

if libxnvctrl.found()
	linkwith += fastgame_nvidia
endif

executable(
	meson.project_name(),
	fastgame_sources,
	include_directories : [include_dir,config_h_dir],
	dependencies : fastgame_deps,
	install: true,
	link_with: linkwith
)
