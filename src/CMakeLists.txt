configure_file(config.h.in config.h)

add_subdirectory(nvidia)

add_executable(fastgame)

target_sources(fastgame PRIVATE
    amdgpu.cpp
    combobox_model.cpp
    command_line_arguments.cpp
    cpu.cpp
    disk.cpp
    environment_variables.cpp
    main.cpp
    memory.cpp
    nvidia.cpp
    presets_manager.cpp
    util.cpp
    resources.qrc
)

target_link_libraries(fastgame
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Quick
    Qt${QT_MAJOR_VERSION}::Qml
    Qt${QT_MAJOR_VERSION}::Gui
    Qt${QT_MAJOR_VERSION}::QuickControls2
    Qt${QT_MAJOR_VERSION}::Widgets
    KF${QT_MAJOR_VERSION}::I18n
    KF${QT_MAJOR_VERSION}::CoreAddons
)

if(XNVCTRL)
    target_link_libraries(fastgame fastgame_nvidia)
endif()

install(TARGETS fastgame ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})