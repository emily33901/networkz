project('networkz', 'cpp')

common_args = ['--std=c++17', '--bind', '-s', 'USE_SDL=2', '-s', 'USE_WEBGL2=1', '-s', 'FULL_ES3=1', '-s', 'ASSERTIONS=1']

dep_compile_args = ['-w'] + common_args

# Declare dependencies first

imgui_core = static_library(
    'imgui',
    files(
        'external/imgui/imgui.cpp', 
        'external/imgui/imgui_demo.cpp', 
        'external/imgui/imgui_draw.cpp', 
        'external/imgui/imgui_demo.cpp',
        'external/imgui/imgui_widgets.cpp',
    ),
    cpp_args: dep_compile_args,
    link_args: common_args,
    install: false,
    pic: true,
)

imgui_node_editor = static_library(
    'imgui_node_editor',
    files(
        'external/imgui-node-editor/NodeEditor/Source/crude_json.cpp',
        'external/imgui-node-editor/NodeEditor/Source/imgui_canvas.cpp',
        'external/imgui-node-editor/NodeEditor/Source/imgui_node_editor_api.cpp',
        'external/imgui-node-editor/NodeEditor/Source/imgui_node_editor.cpp',
    ),
    include_directories: include_directories(
        'external/imgui-node-editor/NodeEditor/Include',
        'external/imgui'
    ),
    cpp_args: dep_compile_args,
    link_args: common_args,
    install: false,
    pic: true,
)

deps = [
    # ImGui
    declare_dependency(
        link_with: imgui_core,
        include_directories: include_directories('external/imgui')
    ),

    # Node editor
    declare_dependency(
        link_with: imgui_node_editor,
        include_directories: include_directories('external/imgui-node-editor/NodeEditor/Include')
    ),

    declare_dependency(
        include_directories: include_directories('external/cpptoml/include')
    )
]

# Networkz files

src = [
    'src/common/platform.hh',
    'src/common/defer.hh',
    'src/common/precompiled.hh', 'src/common/precompiled.cc',

    'src/render/imgui/opengl.hh', 'src/render/imgui/opengl.cc',
    'src/render/imgui/platform.hh','src/render/imgui/platform.cc',


    'src/render/render.hh',
    'src/render/init.cc',
    'src/render/frame.cc',
    'src/render/js.cc',

    'src/filesystem/filesystem.hh',
    'src/filesystem/filesystem.cc',

    
    'src/test/test.cc',
    'src/test/test.cc',

    'src/main.cc',
]

includes = include_directories([
    'src/', 
])

args = common_args + []

executable('networkz', src, 
           include_directories : includes,
           cpp_pch: 'src/common/precompiled.hh',
           dependencies: deps,
           name_suffix: 'js',
           cpp_args: args,
           link_args: args)

time = run_command('python', 'tools/build/time.py').stdout().strip()

# Configure index.html
conf_data = configuration_data()
conf_data.set('exe_name', 'networkz')
conf_data.set('compileDate', time)
configure_file(input: 'dist/index.html',
               output: 'index.html',
               configuration : conf_data)

configure_file(input: 'dist/application.toml',
               output: 'application.toml',
               configuration : conf_data)