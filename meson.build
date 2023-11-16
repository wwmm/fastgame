project(
	'fastgame',
	'c',
	'cpp',
	default_options : [
		'cpp_std=c++20',
		'buildtype=debugoptimized',
		# 1: -Wall
		# 2: -Wall -Wextra
		# 3: -Wall -Wextra -Wpedantic
		'warning_level=2'
	],
	license: 'GPL-3-or-later',
	version: '0.3.0',
	meson_version: '>= 0.51.0'
)

add_global_arguments('-DG_LOG_DOMAIN="fastgame"', language : 'c')
add_global_arguments('-DG_LOG_DOMAIN="fastgame"', language : 'cpp')

gnome_mod = import('gnome')
i18n_mod = import('i18n')

prefix = get_option('prefix')
bindir = join_paths(prefix, get_option('bindir'))
localedir = join_paths(prefix, get_option('localedir'))
datadir = join_paths(prefix, get_option('datadir'))
include_dir = include_directories('include')
sysconfdir = join_paths(prefix, get_option('sysconfdir'))
config_h_dir = include_directories('.')

fastgame_config_dir = join_paths(sysconfdir, 'fastgame')

# Configuration
conf = configuration_data()

conf.set_quoted('GETTEXT_PACKAGE', meson.project_name())
conf.set_quoted('LOCALE_DIR', localedir)
conf.set_quoted('VERSION', meson.project_version())
conf.set_quoted('CONFIGDIR', fastgame_config_dir)

subdir('data')
subdir('po')
subdir('src')

configure_file(output: 'config.h', configuration: conf)
