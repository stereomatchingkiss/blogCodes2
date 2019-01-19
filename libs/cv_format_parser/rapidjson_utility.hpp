#ifndef RAPIDJSON_UTILITY_HPP
#define RAPIDJSON_UTILITY_HPP

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

namespace rapid_json{

rapidjson::Document json_file_to_doc(std::string const &file_location);

void json_file_prettify(std::string const &input_file, std::string const &output_file);

}


#endif // RAPIDJSON_UTILITY_HPP
