if exist build (
  cmake --build build --target clean
  rmdir /s /q build
)