# Multiplayer hangman game
Desktop application in which players guess the letters of the sacred word shared by all participants in a given room. Every player has their own hangman and players in the same room can see other participants hangmen. Player gets a point for a correct guessed letter. There can be multiple games in different rooms at the same time, but in one room can be only up to six players.

## Technologies
* C++
* WPF
* CMake

## Setup
You can change port, time of the game and waiting time (in seconds) in the config file placed in ``Files`` directory (first value corresponds to the port, second value to the game time and the last value to the waiting time).

To run the server application run following commands:
```
cmake CMakeLists.txt
make
./server
```

To run the client application use following commands:
```
cd .\HangmanClient
dotnet publish -c Release --output .\MyTargetFolder HangmanClient.sln
cd .\MyTargerFolder
start HangmanClient (or double click on HangmanClient.exe)
```

You can change settings of the client in ``HangmanClient.dll.config``.
