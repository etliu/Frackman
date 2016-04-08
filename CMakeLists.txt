cmake_minimum_required(VERSION 3.3)
project(FrackMan)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    DerivedData/FrackMan/Build/Products/Debug/Assets/barrel.tga
    DerivedData/FrackMan/Build/Products/Debug/Assets/bribed.wav
    DerivedData/FrackMan/Build/Products/Debug/Assets/die.wav
    DerivedData/FrackMan/Build/Products/Debug/Assets/digging.wav
    DerivedData/FrackMan/Build/Products/Debug/Assets/dirt.tga
    DerivedData/FrackMan/Build/Products/Debug/Assets/finished.wav
    DerivedData/FrackMan/Build/Products/Debug/Assets/foundoil.wav
    DerivedData/FrackMan/Build/Products/Debug/Assets/frack1.tga
    DerivedData/FrackMan/Build/Products/Debug/Assets/frack2.tga
    DerivedData/FrackMan/Build/Products/Debug/Assets/frack3.tga
    DerivedData/FrackMan/Build/Products/Debug/Assets/frack4.tga
    DerivedData/FrackMan/Build/Products/Debug/Assets/giveup.wav
    DerivedData/FrackMan/Build/Products/Debug/Assets/goaway.wav
    DerivedData/FrackMan/Build/Products/Debug/Assets/gold.tga
    DerivedData/FrackMan/Build/Products/Debug/Assets/hardcore1.tga
    DerivedData/FrackMan/Build/Products/Debug/Assets/hardcore2.tga
    DerivedData/FrackMan/Build/Products/Debug/Assets/hardcore3.tga
    DerivedData/FrackMan/Build/Products/Debug/Assets/hardcore4.tga
    DerivedData/FrackMan/Build/Products/Debug/Assets/ouch.wav
    DerivedData/FrackMan/Build/Products/Debug/Assets/protester1.tga
    DerivedData/FrackMan/Build/Products/Debug/Assets/protester2.tga
    DerivedData/FrackMan/Build/Products/Debug/Assets/protester3.tga
    DerivedData/FrackMan/Build/Products/Debug/Assets/rock1.tga
    DerivedData/FrackMan/Build/Products/Debug/Assets/rock2.tga
    DerivedData/FrackMan/Build/Products/Debug/Assets/rock3.tga
    DerivedData/FrackMan/Build/Products/Debug/Assets/rock4.tga
    DerivedData/FrackMan/Build/Products/Debug/Assets/rockslide.wav
    DerivedData/FrackMan/Build/Products/Debug/Assets/sonar.tga
    DerivedData/FrackMan/Build/Products/Debug/Assets/sonar.wav
    DerivedData/FrackMan/Build/Products/Debug/Assets/squirt.wav
    DerivedData/FrackMan/Build/Products/Debug/Assets/theme.wav
    DerivedData/FrackMan/Build/Products/Debug/Assets/water1.tga
    DerivedData/FrackMan/Build/Products/Debug/Assets/water2.tga
    DerivedData/FrackMan/Build/Products/Debug/Assets/water3.tga
    DerivedData/FrackMan/Build/Products/Debug/Assets/waterpool.tga
    DerivedData/FrackMan/Build/Products/Debug/Assets/woohoo.wav
    FrackMan/Actor.cpp
    FrackMan/Actor.h
    FrackMan/freeglut.h
    FrackMan/freeglut_ext.h
    FrackMan/freeglut_std.h
    FrackMan/GameConstants.h
    FrackMan/GameController.cpp
    FrackMan/GameController.h
    FrackMan/GameWorld.cpp
    FrackMan/GameWorld.h
    FrackMan/GraphObject.h
    FrackMan/main.cpp
    FrackMan/SoundFX.h
    FrackMan/SpriteManager.h
    FrackMan/StudentWorld.cpp
    FrackMan/StudentWorld.h
    FrackMan.xcodeproj/project.xcworkspace/contents.xcworkspacedata
    FrackMan.xcodeproj/project.pbxproj )

add_executable(FrackMan ${SOURCE_FILES})