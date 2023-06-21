systemd_nts_timesyncd_conf = configure_file(
                       input : 'nts-timesyncd.service.in',
                       output : 'nts-timesyncd.service',
                       configuration : substs)
                       install_data(systemd_nts_timesyncd_conf,
                       install_dir : '/lib/systemd/system')
