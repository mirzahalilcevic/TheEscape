----------------------------------------
LEVEL CONFIGURATION FORMAT
----------------------------------------

[PlayerStartPositionX] [PlayerStartPositionY] [PlayerStartRotation (in radians)]

[NumberOfEnemies]
[Enemy1StartPositionX] [Enemy1StartPositionY] [Enemy1StartRotation (in radians)]
...

[NumberOfLives]
[Life1PositionX] [Life1PositionY]
...

[WidthOfMap] [HeightOfMap] (Width and height have to be the same, a seg fault occurs if they aren't. It's not a feature, it's a bug)
[MAP] (map matrix)

Possible block values:

0         - free space
1 | 2 | 3 - walls with different textures
6         - door
7         - exit
8         - entrance (same as exit but doesn't lead to next level)

*** Note: If another level is added, static data member levelNum in Engine.hpp has to be changed accordingly.