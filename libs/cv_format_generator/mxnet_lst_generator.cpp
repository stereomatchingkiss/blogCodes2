#include "mxnet_lst_generator.hpp"


mxnet_lst_generator::mxnet_lst_generator(const QString &save_as) :
    file_(save_as)
{
    if(file_.open(QIODevice::WriteOnly)){
        stream_.setDevice(&file_);
    }else{
        throw std::runtime_error(QString("cannot open file:%1").arg(save_as).toStdString());
    }
}

void mxnet_lst_generator::append(mxnet_lst_generator::data_to_generate input)
{
    data_to_gen_.emplace_back(std::move(input));
}

void mxnet_lst_generator::apply()
{
    for(auto const &val : data_to_gen_){
        apply(val);
    }
}

void mxnet_lst_generator::apply(const mxnet_lst_generator::data_to_generate &input)
{
    stream_<<input.index_<<"\t"<<4<<"\t"<<5<<"\t"<<input.width_<<"\t"<<input.height_;
    for(auto const &val : input.labels_){
        stream_<<"\t"<<val.id_<<"\t"<<val.top_left_.x()<<"\t"<<val.top_left_.y()
              <<"\t"<<val.bottom_right_.x()<<"\t"<<val.bottom_right_.y();
    }
    stream_<<"\t"<<input.img_path_<<"\r\n";
}
