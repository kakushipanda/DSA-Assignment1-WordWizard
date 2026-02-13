cmake_minimum_required(VERSION 3.5)
project(WordWizard)

#find files with extension of .cpp and .h
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/*.[ch]pp
    ${CMAKE_CURRENT_LIST_DIR}/*.h
)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

#adding project
add_executable(WordWizard
    ${SOURCE_FILES}
    ${imgui_source_files}
)

if(MSVC)
    set_property(TARGET WordWizard PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

target_link_libraries(WordWizard
    PRIVATE
        imgui::imgui
        glfw
        glad::glad
        imm32
        winmm
        gdi32
        shell32
)


set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT WordWizard)

