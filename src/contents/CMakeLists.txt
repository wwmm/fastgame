configure_file(com.github.wwmm.fastgame.apply.policy.in com.github.wwmm.fastgame.apply.policy)

install(FILES 
    icons/fastgame.svg 
    DESTINATION ${KDE_INSTALL_FULL_DATADIR}/icons/hicolor/scalable/apps
)

install(FILES 
    ${CMAKE_CURRENT_BINARY_DIR}/com.github.wwmm.fastgame.apply.policy 
    DESTINATION ${KDE_INSTALL_FULL_DATADIR}/polkit-1/actions
)

install(PROGRAMS 
    com.github.wwmm.fastgame.desktop 
    DESTINATION ${KDE_INSTALL_APPDIR}
)