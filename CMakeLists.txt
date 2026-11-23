cmake_minimum_required(VERSION 3.19)
project(QtPet LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

set(CMAKE_AUTORCC ON)

qt_add_executable(QtPet
    WIN32 MACOSX_BUNDLE
    main.cpp
    petwindow.cpp
    petwindow.h
    resource.qrc
)

target_link_libraries(QtPet
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS QtPet
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET QtPet
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
