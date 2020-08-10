#include "convert_yolo_detect_results.hpp"
#include "ui_convert_yolo_detect_results.h"

#include <QDebug>
#include <QDomDocument>

#include <QFile>
#include <QFileInfo>
#include <QImageReader>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <QMessageBox>
#include <QSettings>

namespace{

QString const state_alexey_ab_yolo_result("convert_yolo_det_result/state_alexey_ab_yolo_result");
QString const state_save_convert_result_at("convert_yolo_det_result/state_save_convert_result_at");

void add_folder_to_xml(QDomDocument &dom, QDomElement &root, QString const &fname)
{
    auto folder_node = dom.createElement("folder");
    auto const fname_split = fname.split("/");
    if(fname_split.size() >= 2){
        auto folder_node_text = dom.createTextNode(fname_split[fname_split.size() - 2]);
        folder_node.appendChild(folder_node_text);
    }
    root.appendChild(folder_node);
}

void add_child_to_xml(QDomDocument &dom, QDomElement &root, QString const &child_name, QString const &text)
{
    auto file_name_node = dom.createElement(child_name);
    file_name_node.appendChild(dom.createTextNode(text));
    root.appendChild(file_name_node);
}

void add_object_to_xml(QDomDocument &dom, QDomElement &root, QJsonObject const &obj, QSize const &size)
{
    auto obj_node = dom.createElement("object");
    add_child_to_xml(dom, obj_node, "name", obj["name"].toString());
    add_child_to_xml(dom, obj_node, "pose", "Unspecified");
    add_child_to_xml(dom, obj_node, "truncated", "0");
    add_child_to_xml(dom, obj_node, "Unspecified", "0");

    auto bndbox_node = dom.createElement("bndbox");
    auto const coor = obj["relative_coordinates"].toObject();
    int const width = static_cast<int>(size.width() * coor["width"].toDouble());
    int const height = static_cast<int>(size.height() * coor["height"].toDouble());
    int const center_x = static_cast<int>(coor["center_x"].toDouble() * size.width());
    int const center_y = static_cast<int>(coor["center_y"].toDouble() * size.height());
    int const xmin = std::max(0, center_x - (width/2));
    int const ymin = std::max(0, center_y - (height/2));
    int const xmax = std::min(size.width() - 1, center_x + (width/2));
    int const ymax = std::min(size.height() - 1, center_y + (height/2));

    add_child_to_xml(dom, bndbox_node, "xmin", QString::number(xmin));
    add_child_to_xml(dom, bndbox_node, "ymin", QString::number(ymin));
    add_child_to_xml(dom, bndbox_node, "xmax", QString::number(xmax));
    add_child_to_xml(dom, bndbox_node, "ymax", QString::number(ymax));

    obj_node.appendChild(bndbox_node);
    root.appendChild(obj_node);
}

QSize add_size_to_xml(QDomDocument &dom, QDomElement &root, QString const &im_path)
{
    QImageReader im_reader(im_path);
    im_reader.setDecideFormatFromContent(true);
    if(im_reader.canRead()){
        auto const size = im_reader.size();
        auto size_node = dom.createElement("size");
        auto width_node = dom.createElement("width");
        width_node.appendChild(dom.createTextNode(QString::number(size.width())));
        auto height_node = dom.createElement("height");
        height_node.appendChild(dom.createTextNode(QString::number(size.height())));
        auto depth_node = dom.createElement("depth");
        depth_node.appendChild(dom.createTextNode(QString::number(3)));
        size_node.appendChild(width_node);
        size_node.appendChild(height_node);
        size_node.appendChild(depth_node);
        root.appendChild(size_node);

        return size;
    }else{
        qDebug()<<__func__<<"cannot read image info = "<<im_path;
    }

    return {};
}

void add_source_to_xml(QDomDocument &dom, QDomElement &root)
{
    auto source_node = dom.createElement("source");
    auto db_node = dom.createElement("database");
    db_node.appendChild(dom.createTextNode("unknown"));
    source_node.appendChild(db_node);
    root.appendChild(source_node);
}

}

convert_yolo_detect_results::convert_yolo_detect_results(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::convert_yolo_detect_results)
{
    ui->setupUi(this);

    QSettings settings("image_clean_up_tool", "convert_yolo_detect_results");
    if(settings.contains(state_alexey_ab_yolo_result)){
        ui->lineEditYoloDetectResult->setText(settings.value(state_alexey_ab_yolo_result).toString());
    }
    if(settings.contains(state_save_convert_result_at)){
        ui->lineEditSaveResultsAt->setText(settings.value(state_save_convert_result_at).toString());
    }
}

convert_yolo_detect_results::~convert_yolo_detect_results()
{
    QSettings settings("image_clean_up_tool", "convert_yolo_detect_results");
    settings.setValue(state_alexey_ab_yolo_result, ui->lineEditYoloDetectResult->text());
    settings.setValue(state_save_convert_result_at, ui->lineEditSaveResultsAt->text());

    delete ui;
}

void convert_yolo_detect_results::on_pushButtonConvert_clicked()
{    
    if(QFile file(ui->lineEditYoloDetectResult->text()); file.open(QIODevice::ReadOnly)){
        auto doc = QJsonDocument::fromJson(file.readAll());
        auto const img_arrays = doc.array();
        qDebug()<<__func__<<": array size = "<<img_arrays.size();
        for(auto const &arr : img_arrays){
            qDebug()<<arr;
            QDomDocument dom;
            auto root = dom.createElement("annotation");
            dom.appendChild(root);

            auto const fname = arr["filename"].toString();
            QFileInfo finfo(fname) ;
            add_folder_to_xml(dom, root, fname);
            add_child_to_xml(dom, root, "filename", finfo.fileName());
            add_child_to_xml(dom, root, "path", fname);
            add_source_to_xml(dom, root);
            auto const img_size = add_size_to_xml(dom, root, fname);
            add_child_to_xml(dom, root, "segmented", "0");

            auto const objs = arr["objects"].toArray();
            qDebug()<<__func__<<": objs size = "<<objs.size();
            for(auto const &obj : objs){
                add_object_to_xml(dom, root, obj.toObject(), img_size);
            }

            if(QFile out_file(ui->lineEditSaveResultsAt->text() + "/" + finfo.baseName() + ".xml");
                    out_file.open(QIODevice::WriteOnly)){
                QTextStream stream(&out_file);
                stream<<dom.toString();
            }
        }
        QMessageBox::information(this, tr("image_clean_up_tool"), tr("Yolo detect result conversion done"));
    }
}
