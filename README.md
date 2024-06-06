# Log Reader
a qt based log reader to read spd log more efficiently

## Dependencies
- Qt6.2 (linux) / Qt6.5(MSVC2022)
- CMake (3.22)
- spdlog (1.14.1)
- boost (1.77.0)

## Features
- Open log file
- Filter log
- Highlight log
- Write user action to log and status bar
- Save user filter and highlight settings

## TODO
- [x] ui design
- [x] menu function design
- [x] read a log and display
- [x] regex find log level and class
- [x] add regex result to side bar
- [x] show all levels in different colors
- [x] update sidebar level given text
- [x] handle check all logic in level udpating
- [ ] handle check all logic in class updating
- [ ] filter log level given sidebar choices
- [ ] filter log class given sidebar choices
- [ ] show user action to status bar
- [ ] save filter settings for next time
- [ ] provide highlight mode
- [ ] click on empty logview to open file
- [ ] scroll area has gap to the left side
- [ ] show time in green color



