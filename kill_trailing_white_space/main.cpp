#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <fstream>
#include <iostream>

std::vector<boost::filesystem::path>
get_files(const std::string &dir);

boost::program_options::variables_map
parse_command_line(int argc, char *argv[]);

int kill_trailing_whitespace(boost::filesystem::path const &path);

int main(int argc, char *argv[])
{
    try{
        auto const vm = parse_command_line(argc, argv);
        if(vm.count("help")){
            return 1;
        }

        if(!vm.empty()){
            if(vm.count("input_file") != 0 && vm.count("input_folder") != 0){
                std::cerr<<"can not specify input_file and "
                           "input_folder at the same time\n";
                return -1;
            }

            if(vm.count("input_file")){
                auto const file = vm["input_file"].as<std::string>();
                boost::filesystem::path path(file);
                return kill_trailing_whitespace(path);
            }

            if(vm.count("input_folder")){
                auto const folder = vm["input_folder"].as<std::string>();
                boost::filesystem::path path(folder);
                if(boost::filesystem::is_directory(path)){
                    auto const files = get_files(path.generic_string());
                    for(auto const &file : files){
                        kill_trailing_whitespace(file);
                    }
                }else{
                    std::cerr<<"this is not a folder\n";
                    return -1;
                }
            }
        }else{
            std::cerr<<"must specify input_file or input_folder\n";
            return -1;
        }
    }catch(std::exception const &ex){
        std::cerr<<ex.what()<<std::endl;
    }
}

std::vector<boost::filesystem::path>
get_files(const std::string &dir)
{
    using namespace boost::filesystem;

    path info(dir);
    std::vector<path> result;
    if(is_directory(info)){
        recursive_directory_iterator beg(dir), end;
        for(; beg != end; ++beg){
            result.emplace_back(beg->path());
        }
    }

    return result;
}

int kill_trailing_whitespace(boost::filesystem::path const &path)
{
    if(!boost::filesystem::is_directory(path)){
        auto const extension = path.extension().string();
        if(extension == ".hpp" || extension == ".cpp"){
            std::ifstream in(path.generic_string());
            if(!in.is_open()){
                std::cerr<<"cannot open file : "<<path.generic_string()<<" \n";
                return -1;
            }
            boost::uuids::random_generator gen;
            auto const id_str = boost::uuids::to_string(gen());
            auto const parent_path = path.parent_path().generic_string().empty() ?
                        "" : path.parent_path().generic_string() + "/";
            auto const temp_file_name = parent_path +
                    path.stem().generic_string() + "_" +
                    id_str + path.extension().generic_string();
            std::ofstream temp_out(temp_file_name);
            if(!temp_out.is_open()){
                std::cerr<<"cannot open : "<<temp_file_name<<"\n";
                return -1;
            }
            std::string line;
            while(std::getline(in, line)){
                boost::trim_right(line);
                boost::trim_if(line, boost::is_any_of("\t"));
                temp_out<<line<<"\n";
            }
            temp_out.close();
            in.close();
            boost::filesystem::rename(temp_file_name,
                                      path.generic_string());

            return 1;
        }
    }

    return -1;
}

boost::program_options::variables_map
parse_command_line(int argc, char *argv[])
{
    using namespace boost::program_options;

    options_description desc{"Options"};
    desc.add_options()
            ("help,h", "Help screen")
            ("input_file,i", value<std::string>(), "Name of input file")
            ("input_folder,I", value<std::string>(), "Name of input folder");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);

    notify(vm);

    if (vm.count("help")){
        std::cout << desc << std::endl;
        return {};
    }

    return vm;
}
