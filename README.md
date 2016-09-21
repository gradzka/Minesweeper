# Minesweeper
Minesweeper game for Windows

##Overview
The main aim of this project is recreating the popular game. Player can choose between skins: Classic, Grayed, Crash Bandicoot and Super Mario.

##Description
The foundation of this project is game_board class. It provides e.g. rand_mines() method, which fills the proper matrix with radomly placed mines, neighbours_mines(), which changes neighbouring mines with proper value in fields matrix, where one field has flags: value, flagged, discovered, last_clicked.

One of the most interesting points is check_neighbours(...) procedure, which check the value of neighbouring fields using recursion. What is more, subclassing is used.

To speed up game for large boards, omp was used.

##Tools
- Microsoft Visual Studio 2013
- WinAPI

##Attributions
- http://stackoverflow.com/questions/8690619/how-to-get-screen-resolution-in-c
- https://code.msdn.microsoft.com/windowsapps/CppWindowsSubclassing-2ef7ee53
- http://cpp0x.pl/kursy/Kurs-WinAPI-C++/167
- http://www.functionx.com/win32/Lesson04.htm
- http://stackoverflow.com/questions/5690333/get-text-from-an-edit-control

##License
MIT

##Credits
* Monika Grądzka
* Robert Kazimierczak

Special thanks for resources used to make skins:
- Classic Theme & Classic Theme (Grayed) - Black Squirrel
  - http://www.spriters-resource.com/pc_computer/minesweeper/sheet/19849/
- Crash Bandicoot - Mr. C, Friedslick6, slawter
  - http://www.spriters-resource.com/playstation/crashbandicoot3warped/sheet/57008/
  - http://www.textures-resource.com/playstation/ps1crash1/sheet/2960/
  - http://www.spriters-resource.com/playstation/ctr/sheet/40646/
  - http://www.spriters-resource.com/playstation/ctr/sheet/40647/
- Super Mario - fsuarez913, mario, Random Talking Bush, Sonikku, A.J. Nitro
  - http://www.dafont.com/super-mario-256.font
  - http://www.spriters-resource.com/nes/supermariobros3/sheet/4237/
  - http://www.spriters-resource.com/wii_u/mariokart8/sheet/68237/
  - http://www.spriters-resource.com/snes/supermariobros3/sheet/4307/
  - http://www.spriters-resource.com/game_boy_advance/mlss/sheet/7545/
  - http://www.spriters-resource.com/wii_u/supermario3dworld/sheet/69874/
