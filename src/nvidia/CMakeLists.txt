if(XNVCTRL)
    add_library(fastgame_nvidia STATIC nvidia.cpp nvml.cpp nvidia.hpp nvml.hpp)

    target_include_directories(fastgame_nvidia PRIVATE ../ ${LIBX11_INCLUDE_DIRS})
    
    target_link_libraries(fastgame_nvidia ${XNVCTRL} ${LIBX11_LIBRARIES})
endif()
