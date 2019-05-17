#include "simplest_video_player.hpp"

extern "C"
{

#include <gst/gst.h>

}

#include <QApplication>

int main(int argc, char *argv[])
{
    gst_init (&argc, &argv);
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(true);

    simplest_video_player svp;
    svp.play("playbin uri=https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm");

    return app.exec();
}
