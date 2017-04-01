mkdir "/Equipment/GalilFermi/Settings"
mkdir "/Equipment/GalilFermi/Monitors"
mkdir "/Equipment/GalilFermi/Settings/Auto Control"
mkdir "/Equipment/GalilFermi/Settings/Manual Control"
mkdir "/Equipment/GalilFermi/Settings/Emergency"

cd "/Equipment/GalilFermi/Settings/Emergency"
create INT Abort
set Abort 0

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
create BOOL "Motor Status[6]"
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
set "Cmd" 0
mkdir "Trolley"
mkdir "Plunging Probe"
cd "/Equipment/GalilFermi/Settings/Manual Control/Trolley"
create INT "Garage Abs Pos"
create INT "Garage Rel Pos"
create INT "Garage Velocity"
create BOOL "Garage Switch"
create INT "Trolley Abs Pos"
create INT "Trolley Rel Pos"
create INT "Trolley Velocity"
create INT "Tension Range Low"
create INT "Tension Range High"
create INT "Tension Offset 1"
create INT "Tension Offset 2"
create BOOL "Trolley Switch"
set "Garage Switch" false
set "Trolley Switch" false
set "Trolley Velocity" 100
set "Tension Range Low" 400
set "Tension Range High" 800
set "Tension Offset 1" 0
set "Tension Offset 2" 0
cd "/Equipment/GalilFermi/Settings/Manual Control/Plunging Probe"
create INT "Plunging Probe Abs Pos[3]"
create INT "Plunging Probe Rel Pos[3]"
create BOOL "Plunging Probe Switch"
set "Plunging Probe Switch" false


cd "/Equipment/GalilFermi/Settings/Auto Control"
create INT "Trigger"
create STRING "Mode[1][256]"
set "Mode" "Continuous"
set "Trigger" 0

mkdir "Trolley"
mkdir "Plunging Probe"
cd "/Equipment/GalilFermi/Settings/Auto Control/Trolley"
create INT "Garage Rel Pos"
create INT "Garage Step Number"
create INT "Trolley Rel Pos"
create INT "Trolley Steop Number"
cd "/Equipment/GalilFermi/Settings/Auto Control/Plunging Probe"
create INT "Plunging Probe Rel Pos[3]"
create INT "Plunging Probe Step Number[3]"

