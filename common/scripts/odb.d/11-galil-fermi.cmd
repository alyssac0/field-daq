mkdir "/Equipment/GalilFermi/Settings"
mkdir "/Equipment/GalilFermi/Monitors"
mkdir "/Equipment/GalilFermi/Settings/Auto Control"
mkdir "/Equipment/GalilFermi/Settings/Manual Control"
mkdir "/Equipment/GalilFermi/Settings/Emergency"

cd "/Equipment/GalilFermi/Settings/Emergency"
create INT Abort
create INT Reset
set Abort 0
set Reset 0

cd "/Equipment/GalilFermi/Settings"
create STRING "Cmd Script[1][256]"
set "Cmd Script" "FermiScript"
create STRING "Script Directory[1][256]"
set "Script Directory" "/home/newg2/Applications/field-daq/online/GalilMotionScripts/"
create BOOL "Root Output"
set "Root Output" false
create STRING "Root Dir[1][256]"
set "Root Dir" "/home/newg2/Applications/field-daq/resources/Root/"
create BOOL "Simulation Mode"
set "Simulation Mode" false

cd "/Equipment/GalilFermi/Monitors"
create BOOL "Monitor Thread Active"
create BOOL "Control Thread Active"
create INT "Positions[6]"
create INT "Velocities[6]"
create INT "Control Voltages[6]"
create INT "Analogs[6]"
create INT "Limit Switches Forward[6]"
create INT "Limit Switches Reverse[6]"
create INT "Auto Motion Finished"
create INT "Buffer Load"
set "Auto Motion Finished" 1

mkdir "/Equipment/GalilFermi/Monitors/Trolley"
cd "/Equipment/GalilFermi/Monitors/Trolley"
create DOUBLE "Position"
create DOUBLE "Velocity"
create DOUBLE "Tensions[2]"


cd "/Equipment/GalilFermi/Settings/Manual Control"
create INT "Cmd"
create INT "Garage Abs Pos"
create INT "Garage Rel Pos"
create INT "Trolley Abs Pos"
create INT "Trolley Rel Pos"
create INT "Plunging Probe Abs Pos[3]"
create INT "Plunging Probe Rel Pos[3]"

cd "/Equipment/GalilFermi/Settings/Auto Control"
create INT "Trigger"
create STRING "Mode[1][256]"
create INT "Garage Rel Pos"
create INT "Garage Step Number"
create INT "Trolley Rel Pos"
create INT "Trolley Steop Number"
create INT "Plunging Probe Rel Pos[3]"
create INT "Plunging Probe Step Number[3]"

set "Mode" "Continuous"
