#include "update_image_label.hpp"

update_image_label::update_image_label(std::string const &input_file,
                                       std::string const &output_file)
    :output_file_{output_file}
{          
    if(!doc_.load_file(input_file.c_str())){
        throw std::runtime_error(("cannot open file " + input_file).c_str());
    }


}

void update_image_label::update_box_ratio_of_label(double ratio)
{
    using namespace pugi;

    xml_node images_node = doc_.child("dataset").child("images");
    for(xml_node img_node : images_node.children()){
        xml_node box_node = img_node.child("box");
        set_box_value(ratio, box_node.attribute("top"));
        set_box_value(ratio, box_node.attribute("left"));
        set_box_value(ratio, box_node.attribute("width"));
        set_box_value(ratio, box_node.attribute("height"));
    }
    doc_.save_file(output_file_.c_str());
}

void update_image_label::set_box_value(double ratio,
                                       pugi::xml_attribute &attr) const
{
    if(attr){
        attr.set_value(static_cast<int>((attr.as_double() * ratio)));
    }
}
