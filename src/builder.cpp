#include <iostream>
#include <filesystem>
#include <thread>
#include <toml++/toml.hpp>

int main(int argc, char** argv) {
    if (argc == 0) return -1; 

    toml::table tbl; 
    try
    {
        tbl = toml::parse_file(argv[1]);

        auto project_settings = tbl["project_settings"]; 
        auto name = project_settings["name"].value<std::string>().value_or(""); 

        if (name == "") {
            std::cout << "No project name specified\n"; 
            return -1; 
        }; 

        auto main_file = project_settings["main_file"].value<std::string>().value_or("");
        if (main_file == "") {
            std::cout << "No main file specified\n"; 
            return -1; 
        } 

        auto build_dir = project_settings["build_dir"].value<std::string>().value_or("build"); 
        if (!std::filesystem::exists(build_dir)) 
            std::filesystem::create_directory(build_dir);

        std::string append = std::filesystem::exists(main_file) ? "" : "../" ; 

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


        auto libs = tbl["libraries"];
        bool fetch_libraries = libs["fetch_libraries"].value<bool>().value_or(false);

        std::ostringstream lib_flags;

        if (toml::array* dirs = libs["library_dirs"].as_array()) {
            dirs->for_each([&](auto&& el) {
                if constexpr (toml::is_string<decltype(el)>) {
                    lib_flags << "-L " << append << *el << " ";
                }
            });
        }

        if (toml::array* arr = libs["libraries"].as_array()) {
            arr->for_each([&](auto&& el) {
                if constexpr (toml::is_string<decltype(el)>) {
                    std::string lib = *el;
                    if (fetch_libraries) {
                        std::string cmd = "c3c vendor-fetch " + lib;
                        int result = std::system(cmd.c_str());
                        if (result != 0) {
                            std::cerr << "Failed to fetch library: " << lib << "\n";
                        }
                    }
                    lib_flags << "-l " << lib << " ";
                }
            });
        }

        std::string lib_flags_str = lib_flags.str();

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        auto build_settings = tbl["build_settings"];
        
        auto optimization_level = build_settings["optimization_level"].value<std::string>().value_or("O0"); 
        auto verbose = build_settings["verbose"].value<bool>().value_or(false) ? " -v": " ";

        auto run_after = build_settings["run_after_build"]; 
        auto run_after_build = run_after[0].value<bool>().value_or(false); 

        std::string run_after_args = ""; 

        if (run_after_build) {
            run_after_args = run_after[1].value<std::string>().value_or(""); 
        }

        std::string extra_files;
        auto included_files = build_settings["include_files"];


        if (toml::array* arr = included_files.as_array()) {
            arr->for_each([&extra_files, &append](auto&& el){
                if constexpr (toml::is_string<decltype(el)>) {
                    extra_files += append + std::string(*el) + " ";
                }
            });
        }

        std::string command = "c3c compile " + append
                               + main_file 
                               + " -o " + build_dir + "/" + name 
                               + " -" + optimization_level 
                               + verbose + " "
                               + extra_files + " " + lib_flags_str;

        std::cout << command + "\n";         
        int res = std::system(command.c_str()); 

        if (res != 0) {
            std::cout << "Error while running build command\n"; 
            return -1; 
        }

        if (run_after_build) {
            std::filesystem::current_path(build_dir);

            #ifdef _WIN32 
                std::system((name + " " + run_after_args).c_str());
            #else 
                std::system(("./" + name + " " + run_after_args).c_str());
            #endif 
        }


    }
    catch (const toml::parse_error& err)
    {
        std::cerr << "Parsing failed:\n" << err << "\n";
        return 1;
    }

    return 0;


}