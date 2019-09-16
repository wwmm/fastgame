project(
	'fastgame',
	'c',
	'cpp',
	default_options : ['cpp_std=c++17','buildtype=debugoptimized'],
	version: '0.0.6',
	meson_version: '>= 0.51.0'
)

add_global_arguments('-DG_LOG_DOMAIN="fastgame"', language : 'c')
add_global_arguments('-DG_LOG_DOMAIN="fastgame"', language : 'cpp')

gnome_mod = import('gnome')
i18n_mod = import('i18n')

prefix = get_option('prefix')
bindir = join_paths(prefix, get_option('bindir'))
datadir = join_paths(prefix, get_option('datadir'))
include_dir = include_directories('include')
sysconfdir = join_paths(prefix, get_option('sysconfdir'))
config_h_dir = include_directories('.')

fastgame_config_dir = join_paths(sysconfdir, 'fastgame')

# Configuration
conf = configuration_data()

conf.set_quoted('VERSION', meson.project_version())
conf.set_quoted('CONFIGDIR', fastgame_config_dir)

subdir('src')
subdir('data')

configure_file(output: 'config.h', configuration: conf)