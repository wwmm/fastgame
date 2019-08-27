install_data(['config.json'], install_dir: fastgame_config_dir)

systemd = dependency('systemd', required : false)

if systemd.found()
systemdsystemunitdir = join_paths(prefix, 'lib/systemd/system')

install_data(['fastgame.service'], install_dir: systemdsystemunitdir)
endif