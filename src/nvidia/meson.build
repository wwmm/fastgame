

nvidia_sources = [
	'nvidia.cpp',
	'nvml.cpp'
]

nvidia_deps = [
	libxnvctrl,
	libdl,
	dependency('x11')
]

if libxnvctrl.found() and libdl.found()
    conf.set('USE_NVIDIA', 1)

    fastgame_nvidia = library('fastgame_nvidia', nvidia_sources, dependencies : nvidia_deps, install: true)
endif
