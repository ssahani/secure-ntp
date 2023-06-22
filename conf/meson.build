systemd_nts_timesyncd_conf = configure_file(
                       input : 'nts-timesyncd.conf.in',
                       output : 'nts-timesyncd.conf',
                       configuration : substs)
                       install_data(systemd_nts_timesyncd_conf,
                       install_dir : '/etc/nts-timesync')
