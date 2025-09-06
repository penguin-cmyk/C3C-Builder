## C3C-Builder

C3C-Builder is a build utility for [C3C](https://c3-lang.org/) projects, similar in style and functionality to CMake for C/C++.
It uses TOML configuration files to manage project settings, build options, libraries, and more.

It utilizes the C3C cli so having the language installed is a requirement.

Features
- Build C3C projects using a single TOML configuration.
- Handle multiple source files, libraries, and include paths.
- Options for optimization, verbose builds, and post-build commands.

Cross-platform support (Linux, Windows, macOS).

Building the Builder

You can compile the builder itself using g++:
```
g++ src/builder.cpp -o c3c-builder
```

Once compiled, you can use it to build your C3C projects by running it with the specified path to your TOML configuration. 

## Example configuration
```toml
[project_settings]
name = "example_name"       ## this is the project name and your main file will be built to this 
main_file = "src/client.c3" ## main file of your project 
build_dir = "../build"      ## the output build directory 

[build_settings]
verbose = false             ## verbose when building    
optimization_level = "O0"   ## any of the optimizations levels that c3c provides
## run_after_build: [yes / no, arguments]
run_after_build = [true, "arg1 arg2 arg3 ..."] 

## all other files that will be included in the build command 
include_files = [ 
    "src/modules/http.c3",
    "src/modules/json.c3",
    "src/modules/websocket.c3"
]

[libraries]
fetch_libraries = false ## if this is true it will fetch the libraries provided in the libraries array
libraries = [] 
library_dirs = []       ## these are local library directories and not global 
```
