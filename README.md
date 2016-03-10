# Draughts

English version of rules. Impemented in C++ and C#.

####C++_Draughts
* `/CmdDraughts`: console version, without dynamic programming (doesn't save the tree)
* `/DraughtsLib`: same version, to be compiled as a dll library
* `/WinDraughts`: GUI written with MFC, TODO: engine should be replaced by the one from `DraughtsLib`

####CS_Checkers
* `/ConsoleCheckers`: console version for testing of engine
* `/WinCheckers`: GUI version of the same engine, GUI written using WPF. Works fine.

TODO: incorporate C++-engine from `DraughtsLib` using PInvoke

The GUI-version written in C# looks like this:

![WinCheckers_screenshot](https://github.com/DanglingPointer/Draughts/blob/master/CS_version_gui.png)
