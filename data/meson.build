install_data(['config.json'], install_dir: fastgame_config_dir)

systemd = dependency('systemd', required : false)

if systemd.found()
systemduserunitdir = join_paths(prefix, 'lib/systemd/user')

install_data(['fastgame.service'], install_dir: systemduserunitdir)
endif