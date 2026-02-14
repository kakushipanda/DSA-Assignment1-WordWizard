if exist build (
  cmake --build build --target clean
  rmdir /s /q build
)

if exist Executable (
  rmdir /s /q Executable
)

pause