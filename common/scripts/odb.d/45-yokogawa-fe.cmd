mkdir "/Equipment/PS Feedback/Settings"
mkdir "/Equipment/PS Feedback/Monitors"

cd "/Equipment/PS Feedback/Settings"

create STRING "IP address[1][256]"
set "IP address" "192.168.5.160"
create BOOL "Root Output"
set "Root Output" false
create STRING "Root Dir[1][256]"
set "Root Dir" "/home/newg2/Applications/field-daq/resources/Root/"
create BOOL "Simulation Mode"
set "Simulation Mode" false
create BOOL "Feedback Active"
set "Feedback Active" false
create DOUBLE "Current Setpoint (mA)"
set "Current Setpoint (mA)" 0.000
create DOUBLE "Field Setpoint"
set "Field Setpoint" 0.000
create DOUBLE "Field Readout Value"
set "Field Readout Value" 0.000
create DOUBLE "P Coefficient"
set "P Coefficient" 0.000
create DOUBLE "I Coefficient"
set "I Coefficient" 0.000
create DOUBLE "D Coefficient"
set "D Coefficient" 0.000
create DOUBLE "Scale Factor (Amps/Hz)"
set "Scale Factor (Amps/Hz)" 1.000

cd "/Equipment/PS Feedback/Monitors"

create INT "Buffer Load"
set "Buffer Load" 0
create BOOL "Read Thread Active"
set "Read Thread Active" false
create DOUBLE "Average Field"
set "Average Field" 0.00
create DOUBLE "Current Value (mA)"
set "Current Value (mA)" 0.00
