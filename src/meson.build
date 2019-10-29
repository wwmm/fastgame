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
