cmake_minimum_required(VERSION 3.28)

project(fastgame VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(QT_DEFAULT_MAJOR_VERSION 6)
set(QT_MAJOR_VERSION ${QT_DEFAULT_MAJOR_VERSION})

set(APPLICATION_DOMAIN "fastgame")
set(APPLICATION_NAME "FastGame")
set(COMPONENT_NAME "fastgame")
set(ORGANIZATION_NAME "WWMM")
set(ORGANIZATION_DOMAIN "com.github.wwmm")

find_package(ECM REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)


find_package(Qt${QT_MAJOR_VERSION} REQUIRED NO_MODULE COMPONENTS
    Core
    Quick
    Qml
    Gui
    QuickControls2
    Widgets
)

find_package(KF${QT_MAJOR_VERSION} REQUIRED COMPONENTS
    Kirigami
    I18n
    CoreAddons
)

find_package(Threads REQUIRED)

add_subdirectory(src)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
