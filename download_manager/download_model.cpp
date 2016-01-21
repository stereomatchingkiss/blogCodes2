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
    connect(manager_, SIGNAL(download_finished(int_fast64_t)),
            this, SLOT(download_finished(int_fast64_t)));
    connect(manager_, SIGNAL(download_progress(int_fast64_t,qint64,qint64)),
            this, SLOT(download_progress(int_fast64_t,qint64,qint64)));
    //connect(manager_, SIGNAL(download_ready_read(int_fast64_t)),
    //        this, SLOT(download_ready_read(int_fast64_t)));
}

bool download_model::
append(QUrl const &value, QString const &save_at,
       QString const &save_as)
{
    auto const uuid = manager_->append(value, save_at, save_as);
    if(uuid != -1){
        auto &ran = data_.get<random>();
        if(ran.emplace_back(value.fileName(),
                            "Waiting", value,
                            uuid).second){
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
    if(!index.isValid() || role == Qt::EditRole){
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
                   [&](auto &e){e.size_ = value.value<size_t>();});
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

void download_model::download_size_changed(size_t value)
{
    if(value < max_download_size_){
       auto const &ran = data_.get<random>();
       auto func = [](auto const &v){return v.status_ == global::waiting;};
       auto r_it = std::find_if(std::begin(ran), std::end(ran), func);
       manager_->start_download(r_it->url_);
    }
}

void download_model::download_finished(int_fast64_t uuid)
{
    int const row = get_row(uuid);
    setData(index(row, static_cast<int>(tag_enum::status)),
            global::done, Qt::DisplayRole);
    setData(index(row, static_cast<int>(tag_enum::percent)),
            "", Qt::DisplayRole);
}

void download_model::download_progress(int_fast64_t uuid,
                                       qint64 bytes_received,
                                       qint64 bytes_total)
{
    int const row = get_row(uuid);
    setData(index(row, static_cast<int>(tag_enum::status)),
            global::downloading, Qt::DisplayRole);
    if(bytes_total != 0){
        auto const percent =
                QString::number(bytes_received) + "/" +
                QString::number(bytes_total);
        setData(index(row, static_cast<int>(tag_enum::percent)),
                percent, Qt::DisplayRole);
    }else{
        setData(index(row, static_cast<int>(tag_enum::percent)),
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
