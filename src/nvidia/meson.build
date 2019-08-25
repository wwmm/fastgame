cxx = meson.get_compiler('cpp')

libxnvctrl = cxx.find_library('libXNVCtrl', required: false)

nvidia_sources = [
	'nvidia.cpp'
]

nvidia_deps = [
	libxnvctrl,
	dependency('x11')
]

if libxnvctrl.found()
    conf.set('USE_NVIDIA', 1)

    fastgame_nvidia = library('fastgame_nvidia', nvidia_sources, dependencies : nvidia_deps, install: true)
endif
