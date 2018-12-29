#include "generate_labels_from_kaggle_dataset.hpp"

#include <QDebug>

int main(int argc, char *argv[])try
{
    if(argc != 2){
        qDebug()<<"Must enter the location of json";
        return -1;
    }

    generate_labels_from_kaggle_dataset(argv[1]).apply();
}catch(std::exception const &ex){
    qDebug()<<__func__<<ex.what();
}
