schemadir = join_paths(datadir, 'glib-2.0', 'schemas')

install_data([
  'schemas/com.github.wwmm.fastgame.gschema.xml',
], install_dir: schemadir)

desktop_file = i18n_mod.merge_file(
  input: 'com.github.wwmm.fastgame.desktop.in',
  output: 'com.github.wwmm.fastgame.desktop',
  type: 'desktop',
  po_dir: '../po',
  install: true,
  install_dir: join_paths(datadir, 'applications')
)

# Validate merged desktop entry file
desktop_utils = find_program('desktop-file-validate', required: false)
if desktop_utils.found()
  test('Validate desktop file', desktop_utils,
    args: [desktop_file]
  )
endif

conf_glade = configuration_data()
conf_glade.set('PROJECT_VERSION', meson.project_version())

glade_about = configure_file(input: 'ui/about.glade.in',
               output: 'about.glade',
               configuration: conf_glade)

gresources = gnome_mod.compile_resources(
	'fastgame-resources',
	'fastgame.gresource.xml',
	c_name: meson.project_name(),
  dependencies: [glade_about]
)

#icondir = join_paths(datadir, 'icons', 'hicolor')
#install_data('fastgame.svg',
#  install_dir: join_paths(icondir, 'scalable', 'apps')
#)

conf_polkit = configuration_data()

conf_polkit.set('BINDIR', bindir)

configure_file(input: 'com.github.wwmm.fastgame.apply.policy.in',
               output: 'com.github.wwmm.fastgame.apply.policy',
               configuration: conf_polkit,
               install: true,
               install_dir: join_paths(datadir, 'polkit-1', 'actions'))

# i18n_mod.merge_file(
#   input: policy_with_bindir,
#   output: 'com.github.wwmm.fastgame.apply.policy',
#   po_dir: join_paths(meson.source_root(), 'po'),
#   install: true,
#   install_dir: join_paths(datadir, 'polkit-1', 'actions')
# )
