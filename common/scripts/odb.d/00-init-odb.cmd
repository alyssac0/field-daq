create STRING "Experiment/Menu Buttons"
set "Experiment/Menu Buttons" "Status, ODB, Messages, Alarms, Programs, History, MSCB, Help"
create BOOL "/Experiment/Run Parameters/Root Output"
set "/Experiment/Run Parameters/Root Output" n

create INT "/Experiment/MAX_EVENT_SIZE"
set "/Experiment/MAX_EVENT_SIZE" 0x1000000

mkdir "Logger"
create BOOL "/Logger/ODB Dump"
set "Logger/ODB Dump" y

mkdir "Settings"
