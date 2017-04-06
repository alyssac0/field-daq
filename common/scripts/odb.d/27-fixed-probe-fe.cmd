mkdir "/Equipment/Fixed Probes/Settings/devices/sis_3316"
cd "/Equipment/Fixed Probes/Settings/devices"

ln "/Settings/Hardware/sis_3316/0" "sis_3316/sis_3316_0"
ln "/Settings/Hardware/nmr_pulser/0" "nmr_pulser"

cd ..
ln "/Settings/NMR Sequence/Fixed Probes" trg_seq_file
ln "/Settings/Hardware/mux_connections" mux_conf_file
ln "/Settings/Analysis/online_fid_params" fid_conf_file

create INT max_event_time
set max_event_time 8000

create INT mux_switch_time
set mux_switch_time 15000

create BOOL analyze_fids_online
set analyze_fids_online y

create BOOL use_fast_fids_class
set use_fast_fids_class n

create INT event_rate_limit
set event_rate_limit 10

create BOOL simulation_mode
set simulation_mode false

mkdir output
cd output

create BOOL write_midas
set write_midas y

create BOOL write_root
set write_root y

create STRING root_path
set root_path "/home/newg2/Applications/field-daq/resources/root"

create STRING root_file
set root_file "fixed_probe_run_%05i.root"

create BOOL write_full_waveforms
set write_full_waveforms false

create INT full_waveform_subsampling
set full_waveform_subsampling 100


