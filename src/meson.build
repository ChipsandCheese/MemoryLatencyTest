sources = ['memorylatency.c']

if (host_machine.cpu_family() == 'x86_64')
    sources += 'memorylatency-amd64.s'
endif

executable('memorylatency',
           sources,
           c_args: default_cflags,
           install : true)
