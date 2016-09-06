# Minesweeper
Minesweeper game for Windows

##Overview
The main aim of this project is recreating the popular game in traditional and Crash Bandicoot way.

##Description
The foundation of this project is game_board class. It provides e.g. rand_mines() method, which fills the proper matrix with radomly placed mines, neighbours_mines(), which changes neighbouring mines with proper value in fields matrix, where one field has flags: value, flagged, discovered.

One of the most interesting points is check_neighbours(...) procedure, which check the value of neighbouring fields using recursion. What is more, subclassing is used.

##Tools
- Microsoft Visual Studio 2013
- WinAPI

##How to run

##How to compile
You can use Microsoft Visual Studio to open the project and compiled it.

##Attributions
- http://stackoverflow.com/questions/8690619/how-to-get-screen-resolution-in-c
- https://code.msdn.microsoft.com/windowsapps/CppWindowsSubclassing-2ef7ee53

##License
MIT

##Credits
* Monika Grądzka
* Robert Kazimierczak


