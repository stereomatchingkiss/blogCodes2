#include "download_model.hpp"
#include "download_manager.hpp"
#include "global_variable.hpp"

namespace dm{

namespace model{

namespace{

using tag_enum = download_item::tag;

}

download_model::download_model(QObject *parent) :
    QAbstractTableModel(parent),
    manager_(new net::download_manager(parent)),
    max_download_size_{4}
{    
    manager_->set_max_download_size(max_download_size_);
    connect(manager_, SIGNAL(download_finished(int_fast64_t, QString)),
            this, SLOT(download_finished(int_fast64_t, QString)));
    connect(manager_, SIGNAL(download_progress(int_fast64_t,qint64,qint64)),
            this, SLOT(download_progress(int_fast64_t,qint64,qint64)));
    connect(manager_, SIGNAL(downloading_size_decrease(size_t)),
            this, SLOT(download_size_changed(size_t)));
    //connect(manager_, SIGNAL(download_error(int_fast64_t, QString)),
    //        this, SLOT(download_error(int_fast64_t, QString)));
    //connect(manager_, SIGNAL(download_ready_read(int_fast64_t)),
    //        this, SLOT(download_ready_read(int_fast64_t)));
}

bool download_model::
append(QUrl const &value, QString const &save_at,
       QString const &save_as)
{
    auto const uuid = manager_->append(value, save_at, save_as);
    if(uuid != -1){
        download_item item(value.fileName(),
                           global::waiting, value,
                           uuid);
        item.save_as_ = save_as;
        item.save_at_ = save_at;
        auto &ran = data_.get<random>();
        if(ran.emplace_back(item).second){
            return insertRows(static_cast<int>(ran.size()),
                              1);
        }
    }

    return false;
}

int download_model::
columnCount(const QModelIndex&) const
{
    return 4;
}

QVariant download_model::data(const QModelIndex &index,
                              int role) const
{
    if(!index.isValid()){
        return {};
    }

    switch(role){
    case Qt::DisplayRole:{
        auto const &set = data_.get<random>();
        switch(static_cast<tag_enum>(index.column())){
        case tag_enum::name:{
            return set[index.row()].name_;
        }
        case tag_enum::percent:{
            return set[index.row()].percent_;
        }
        case tag_enum::size:{
            return set[index.row()].size_;
        }
        case tag_enum::status:{
            return set[index.row()].status_;
        }
        default:
            return false;
        }
    }
    }

    return {};
}

bool download_model::erase(int row)
{
    auto &ran = data_.get<random>();
    if(manager_->erase(ran[row].uuid_)){
        removeRows(row, 1);
        return true;
    }

    return false;
}

size_t download_model::get_max_download_size() const
{
    return max_download_size_;
}

QVariant download_model::
headerData(int section, Qt::Orientation orientation,
           int role) const
{
    if(role == Qt::DisplayRole){
        if(orientation == Qt::Horizontal){
            switch(static_cast<tag_enum>(section)){
            case tag_enum::name:{
                return "Name";
            }
            case tag_enum::percent:{
                return "Percent";
            }
            case tag_enum::size:{
                return "Size";
            }
            case tag_enum::status:{
                return "Status";
            }

            }
        }else if(orientation == Qt::Vertical){
            return section + 1;
        }
    }

    return {};
}

bool download_model::insertRows(int row, int count,
                                const QModelIndex&)
{
    beginInsertRows({}, row, row + count - 1);
    endInsertRows();

    return true;
}

bool download_model::removeRows(int row, int count,
                                const QModelIndex&)
{
    beginRemoveRows({}, row, row + count - 1);
    auto &ran = data_.get<random>();
    for(int i = 0; i != count; ++i){
        ran.erase(std::begin(ran) + row);
    }
    endRemoveRows();

    return true;
}

int download_model::rowCount(const QModelIndex&) const
{
    return static_cast<int>(data_.size());
}

bool download_model::
setData(const QModelIndex &index,
        const QVariant &value, int role)
{
    if(!index.isValid() && role == Qt::DisplayRole){
        return false;
    }

    auto &ran = data_.get<random>();
    auto it = std::begin(ran) + index.row();
    if(it == std::end(ran)){
        return {};
    }
    switch(static_cast<tag_enum>(index.column())){
    case tag_enum::name:{
        ran.modify(it,
                   [&](auto &e){e.name_ = value.toString();});
        emit dataChanged(index, index);
        return true;
    }
    case tag_enum::percent:{
        ran.modify(it,
                   [&](auto &e){e.percent_ = value.toString();});
        emit dataChanged(index, index);
        return true;
    }
    case tag_enum::size:{
        ran.modify(it,
                   [&](auto &e){e.size_ = value.toString();});
        emit dataChanged(index, index);
        return true;
    }
    case tag_enum::status:{
        ran.modify(it,
                   [&](auto &e){e.status_ = value.toString();});
        emit dataChanged(index, index);
        return true;
    }
    default:
        return false;
    }

    return true;
}

void download_model::set_max_download_size(size_t value)
{
    max_download_size_ = value;
    manager_->set_max_download_size(value);
}

void download_model::start_download(int row)
{
    auto const &ran = data_.get<random>();
    manager_->start_download(ran[row].uuid_);
}

void download_model::download_error(int_fast64_t uuid,
                                    QString error)
{
    int const row = get_row(uuid);
    setData(index(row, static_cast<int>(tag_enum::status)),
            global::error + error, Qt::DisplayRole);
    setData(index(row, static_cast<int>(tag_enum::percent)),
            tr("0%"), Qt::DisplayRole);
}

void download_model::download_size_changed(size_t value)
{
    if(value < max_download_size_){
        qDebug()<<__func__<<" : value < max download size";
        auto const &ran = data_.get<random>();
        auto func = [](auto const &v){return v.status_ == global::waiting;};
        auto r_it = std::find_if(std::begin(ran), std::end(ran), func);
        if(r_it != std::end(ran)){
            qDebug()<<__func__<<" : start download";
            manager_->start_download(r_it->uuid_);
        }
    }
}

void download_model::download_finished(int_fast64_t uuid, QString error)
{    
    int const row = get_row(uuid);
    auto const &ran = data_.get<random>();
    if(!ran[row].status_.contains(global::error)){
        if(error.isEmpty() && ran[row].size_ != "0/0"){
            setData(index(row, static_cast<int>(tag_enum::status)),
                    global::done, Qt::DisplayRole);
            //setData(index(row, static_cast<int>(tag_enum::percent)),
            //        tr("100%"), Qt::DisplayRole);
        }else{
            setData(index(row, static_cast<int>(tag_enum::status)),
                    global::error + error, Qt::DisplayRole);
        }
    }
}

void download_model::download_progress(int_fast64_t uuid,
                                       qint64 bytes_received,
                                       qint64 bytes_total)
{
    int const row = get_row(uuid);
    setData(index(row, static_cast<int>(tag_enum::status)),
            global::downloading, Qt::DisplayRole);
    if(bytes_total > 0){
        auto const size =
                QString::number(bytes_received) + "/" +
                QString::number(bytes_total);
        setData(index(row, static_cast<int>(tag_enum::size)),
                size, Qt::DisplayRole);
        float const percent = bytes_received/static_cast<float>(bytes_total);
        setData(index(row, static_cast<int>(tag_enum::percent)),
                QString::number(percent*100) + "%", Qt::DisplayRole);
    }else{
        setData(index(row, static_cast<int>(tag_enum::percent)),
                global::unknown, Qt::DisplayRole);
        setData(index(row, static_cast<int>(tag_enum::size)),
                global::unknown, Qt::DisplayRole);
    }
}

void download_model::download_ready_read(int_fast64_t uuid)
{
    setData(get_index(uuid, tag_enum::status),
            global::starting, Qt::DisplayRole);
}

const QModelIndex download_model::
get_index(int_fast64_t uuid, tag_enum col) const
{
    int const row = get_row(uuid);
    return index(row, static_cast<int>(col));
}

}

}
