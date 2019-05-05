#include "gstreamer_concepts.hpp"

extern "C"
{

#include <gst/gst.h>

}


#include <iostream>
using namespace std;

int gstreamer_concepts(int argc, char *argv[])
{
    cout<<"Initialize GStreamer"<<endl;
    gst_init (&argc, &argv);

    cout<<"Create the elements"<<endl;
    GstElement *source = gst_element_factory_make ("videotestsrc", "source");
    GstElement *sink = gst_element_factory_make ("autovideosink", "sink");

    cout<<"Create the empty pipeline"<<endl;
    GstElement *pipeline = gst_pipeline_new ("test-pipeline");

    if(!pipeline || !source || !sink){
        g_printerr ("Not all elements could be created.\n");
        return -1;
    }

    cout<<"Build the pipeline"<<endl;
    gst_bin_add_many (GST_BIN (pipeline), source, sink, NULL);
    if (gst_element_link (source, sink) != TRUE) {
        g_printerr ("Elements could not be linked.\n");
        gst_object_unref (pipeline);
        return -1;
    }

    cout<<"Modify the source's properties"<<endl;
    g_object_set (source, "pattern", 0, NULL);

    cout<<"Start playing"<<endl;
    GstStateChangeReturn ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        gst_object_unref (pipeline);
        return -1;
    }

    cout<<"Wait until error or EOS"<<endl;
    GstBus *bus = gst_element_get_bus (pipeline);
    cout<<"gst_element_get_bus"<<endl;
    GstMessage *msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GstMessageType(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    cout<<"Parse message"<<endl;
    if (msg != NULL) {
        switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_ERROR:
            GError *err;
            gchar *debug_info;
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

    cout<<"Free resources"<<endl;
    gst_object_unref (bus);
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);
    return 0;
}
