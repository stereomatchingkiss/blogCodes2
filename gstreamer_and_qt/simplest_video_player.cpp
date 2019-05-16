#include "simplest_video_player.hpp"

#include <QApplication>
#include <QDebug>
#include <QTimer>

extern "C"
{

#include <glib.h>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>

}

struct simplest_video_player::pimpl
{
    pimpl() = default;
    ~pimpl()
    {
        clear();
    }

    void clear()
    {
        if(pipeline_){
            gst_element_set_state(pipeline_, GST_STATE_NULL);
            gst_object_unref(pipeline_);
            pipeline_ = nullptr;
            sink_ = nullptr;
        }
    }

    GstElement *pipeline_ = nullptr;
    GstElement *sink_ = nullptr;
};

simplest_video_player::simplest_video_player() :
    impl_(std::make_unique<pimpl>())
{
    resize(320, 240);
    setWindowTitle("GstVideoOverlay Qt demo");
}

simplest_video_player::~simplest_video_player()
{

}

void simplest_video_player::play(const QString &command)
{
    impl_->clear();
    impl_->pipeline_ = gst_parse_launch(command.toLatin1().data(), nullptr);
    impl_->sink_ = gst_bin_get_by_interface(GST_BIN(impl_->pipeline_), GST_TYPE_VIDEO_OVERLAY);
    if(impl_->sink_ == nullptr){
        qDebug()<<__func__<<"Couldn't find a working video sink.";
        impl_->clear();
        return;
    }
    gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (impl_->sink_), winId());

    GstStateChangeReturn const sret = gst_element_set_state (impl_->pipeline_, GST_STATE_PLAYING);
    if(sret == GST_STATE_CHANGE_FAILURE){
        impl_->clear();
        // Exit application
        QTimer::singleShot(0, QApplication::activeWindow(), SLOT(quit()));
    }else{
        show();
    }
}
