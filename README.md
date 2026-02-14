HashMap Application - WordWizard

_____________________________________________________________________________
Overview
This is a C++ application built using CMake with dependencies managed through 
vcpkg. The project includes batch scripts to simplify building, running, and 
cleaning the application.

_____________________________________________________________________________
Requirements

Windows operating system
CMake version 3.15 or newer
A C++ compiler such as Microsoft Visual C++
Git (only needed if vcpkg must be downloaded or updated)

_____________________________________________________________________________
Building the Application (Recommended Method)

Run the provided "run.bat" script. It will automatically:
- Create the build folder,
- Run the CMake configure command,
- Generate the Visual Studio 2022 solution.

Cleaning the Build
To remove all build files:

1. Open a command prompt in the project root.
2. Run:

clean.bat

This will delete the build directory and any compiled outputs.
_____________________________________________________________________________
Word Wizard – Quick User Guide

1. Launch the application from Executable\WordWizard.exe

2. In the “Word Wizard: Anagram Finder” window:

    Click the text box.

    Type any sequence of letters (for example: astr).

3. The app will automatically:

    Search the dictionary.

    Display all anagrams that match the typed letters.

4. Results appear instantly below the input:

    Green text → matches found.

    Red text → no anagrams exist.

5. Toggle "Show performance comparison with linear search" to see performance
   comparison with linear search.

