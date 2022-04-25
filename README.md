# Camera_Calibration_and_Augmented_Reality

In this project, the user has to take multiple photos of the chessboard at different angles in order to extract the intrinsic features of the camera such as camera matrix and distortion matrix. The features are stored in a text file. The program can detect a target and then place a virtual object in the scene relative to the target that moves and orients itself correctly given motion of the camera or target. Harris corner features are also detected to use as a features in the real world and use these corners to project the virtual objests in the real world..

Operating System - Linux(Ubuntu)
IDE - Visual Studio Code

The following files are attached :

vidDisplay.cpp
draw3d.cpp
harris.cpp
function.cpp
function.h

Inorder to execute please type in the name of the files to be compiled in the "add_executable" line (line 24) in CMakeLists.txt file, by default it is set to the required file to be run.

Then type the following commands,

1. cmake .
2. make
3. ./CV_PROJECT_4 "label"
  
Wiki Khoury Report : https://wiki.khoury.northeastern.edu/x/KdZ5Bg

