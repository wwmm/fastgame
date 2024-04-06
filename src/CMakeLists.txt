configure_file(config.h.in config.h)

add_executable(fastgame)

target_sources(fastgame PRIVATE
    combobox_model.cpp
    command_line_arguments.cpp
    command_line_arguments.hpp
    cpu.cpp
    main.cpp
    environment_variables.cpp
    environment_variables.hpp
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

install(TARGETS fastgame ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})