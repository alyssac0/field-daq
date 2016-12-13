mkdir "/Equipment/Galil/Settings"
mkdir "/Equipment/Galil/Monitor"

cd "/Equipment/Galil/Settings"
create STRING "CmdScript[1][256]"
create STRING "Script Directory[1][256]"
set "Script Directory" "/home/newg2/Applications/field-daq/online/GalilMotionScripts/"

cd "/Equipment/Galil/Monitor"

create BOOL "Read Thread Active"
create INT "Read Loop Index"
create INT "Buffer Load"
create DOUBLE "Motor Positions[3]"
create DOUBLE "Motor Velocities[3]"
create DOUBLE "Tensions[2]"

cd /
cp "/Equipment/Galil" "/Equipment/SimGalil"
