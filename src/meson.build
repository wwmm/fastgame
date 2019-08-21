fastgame_sources = [
	'fastgame.cpp',
	'netlink.cpp',
	'cmdline_options.cpp',
	'config.cpp',
	'scheduler.cpp'
]

fastgame_deps = [
	dependency('boost', version: '>=1.65', modules:['program_options']),
	dependency('threads')
]

executable(
	meson.project_name(),
	fastgame_sources,
	include_directories : [include_dir,config_h_dir],
	dependencies : fastgame_deps,
	install: true
)

