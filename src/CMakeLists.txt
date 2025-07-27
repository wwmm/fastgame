configure_file(config.h.in config.h)

add_subdirectory(contents)
add_subdirectory(nvidia)

add_executable(fastgame)
add_executable(fastgame_launcher)
add_executable(fastgame_apply)

kde_target_enable_exceptions(fastgame PRIVATE)
kde_target_enable_exceptions(fastgame_launcher PRIVATE)
kde_target_enable_exceptions(fastgame_apply PRIVATE)

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
    scx_sched.cpp
    util.cpp
    resources.qrc
)

target_sources(fastgame_launcher PRIVATE
    fastgame_launcher.cpp
    util.cpp
)

target_sources(fastgame_apply PRIVATE
    fastgame_apply.cpp
    netlink.cpp
    netlink_c.c
    util.cpp
)

target_link_libraries(fastgame PRIVATE
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Quick
    Qt${QT_MAJOR_VERSION}::Qml
    Qt${QT_MAJOR_VERSION}::Gui
    Qt${QT_MAJOR_VERSION}::QuickControls2
    Qt${QT_MAJOR_VERSION}::Widgets
    KF${QT_MAJOR_VERSION}::I18n
    KF${QT_MAJOR_VERSION}::CoreAddons
    KF${QT_MAJOR_VERSION}::ConfigCore
    KF${QT_MAJOR_VERSION}::ConfigGui
    Boost::filesystem
    Boost::process
)

target_link_libraries(fastgame_launcher PRIVATE
    Qt${QT_MAJOR_VERSION}::Core
    Boost::program_options
)

target_link_libraries(fastgame_apply PRIVATE
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Qml
    Boost::process
)

if(XNVCTRL)
    target_link_libraries(fastgame PRIVATE fastgame_nvidia)
    target_link_libraries(fastgame_apply PRIVATE fastgame_nvidia)
endif()

kconfig_add_kcfg_files(fastgame GENERATE_MOC ${KCFGC_FILES})

# install(FILES ${KCFG_FILES} DESTINATION ${KDE_INSTALL_KCFGDIR})

install(TARGETS fastgame fastgame_launcher fastgame_apply ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})