project('vish', 'cpp')

add_global_arguments('-std=c++11', language : 'cpp')
add_global_arguments('-Wall', language : 'cpp')
add_global_arguments('-Wno-error=unused-local-typedefs', language : 'cpp')
add_global_arguments('-Wno-unused-local-typedefs', language : 'cpp')
add_global_arguments('-Wno-error=pedantic', language : 'cpp')

executable('vish', [
    'src/main.cpp',
    'src/prompt.cpp',
    'src/executor.cpp'
    ]
)
