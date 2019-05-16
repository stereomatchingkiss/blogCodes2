#include "hello_world.hpp"

extern "C"
{

#include <gst/gst.h>

}

#include <iostream>

using namespace std;

void hello_world(int argc, char *argv[])
{
    cout<<"Initialize GStreamer"<<endl;

    gst_init(&argc, &argv);

    cout<<"Build the pipeline"<<endl;    
    //C:/Users/yyyy/programs/Qt/blogCodes2/object_detection_and_framing/videos/ice_video_20181002-000025.webm
    GstElement *pipeline =
            gst_parse_launch
            ("playbin uri=https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm",
             nullptr);

    cout<<"Start playing"<<endl;
    gst_element_set_state (pipeline, GST_STATE_PLAYING);

    cout<<"Wait until error or EOS "<<endl;
    GstBus *bus = gst_element_get_bus (pipeline);
    cout<<"Get bus "<<endl;
    GstMessage *msg = gst_bus_timed_pop_filtered (bus, static_cast<GstClockTime>(GST_CLOCK_TIME_NONE),
                                                  static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
    cout<<"Get msg "<<endl;

    cout<<"Free resources"<<endl;
    if (msg != nullptr) {
        GError *err;
        gchar *debug_info;

        switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error (msg, &err, &debug_info);
            g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
            g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
            g_clear_error (&err);
            g_free (debug_info);
            break;
        case GST_MESSAGE_EOS:
            g_print ("End-Of-Stream reached.\n");
            break;
        default:
            // We should not reach here because we only asked for ERRORs and EOS
            g_printerr ("Unexpected message received.\n");
            break;
        }
        gst_message_unref (msg);
    }
    gst_object_unref (bus);
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);//*/
}
