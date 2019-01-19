#include "rapidjson_utility.hpp"

#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>

#include <fstream>
#include <sstream>

rapidjson::Document rapid_json::json_file_to_doc(const std::string &file_location)
{
    using namespace rapidjson;

    std::ifstream istream(file_location);
    if(istream.is_open()){
        IStreamWrapper isw(istream);
        Document d;
        d.ParseStream(isw);
        if(d.HasParseError()){
            std::stringstream sstream;
            sstream<<d.GetErrorOffset();
            throw std::runtime_error(GetParseError_En(d.GetParseError()) + std::string(" offset:") + sstream.str());
        }

        return d;
    }else{
        throw std::runtime_error("Cannot open file:" + file_location);
    }
}

void rapid_json::json_file_prettify(const std::string &input_file, std::string const &output_file)
{
    using namespace rapidjson;
    using namespace std;

    Document const d = rapid_json::json_file_to_doc(input_file);
    ofstream ofs(output_file);
    OStreamWrapper osw(ofs);
    PrettyWriter<OStreamWrapper> writer(osw);
    d.Accept(writer);
}
