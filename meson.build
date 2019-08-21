project(
	'fastgame',
	'c',
	'cpp',
	default_options : ['cpp_std=c++17','buildtype=debugoptimized'],
	version: '0.0.1',
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
config_h_dir = include_directories('.')

# Configuration
conf = configuration_data()

conf.set_quoted('VERSION', meson.project_version())

configure_file(output: 'config.h', configuration: conf)

subdir('src')

