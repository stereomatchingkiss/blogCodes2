#include "time_management.hpp"

extern "C"{

#include <gst/gst.h>

}

#include <iostream>

using namespace std;

namespace{

// Structure to contain all our information, so we can pass it around
struct CustomData
{
    GstElement *playbin = nullptr;  // Our one and only element
    bool playing = false;      // Are we in the PLAYING state?
    bool terminate = false;    // Should we terminate execution?
    gboolean seek_enabled = false; // Is seeking enabled for this media?
    bool seek_done = false;    // Have we performed the seek already?
    gint64 duration = 0;       // How long does this media last, in nanoseconds
};

// Forward definition of the message processing function
void handle_message (CustomData *data, GstMessage *msg)
{   
    switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_ERROR:{
        GError *err;
        gchar *debug_info;
        gst_message_parse_error (msg, &err, &debug_info);
        g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
        g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
        g_clear_error (&err);
        g_free (debug_info);
        data->terminate = TRUE;
        break;
    }
    case GST_MESSAGE_EOS:{
        g_print ("End-Of-Stream reached.\n");
        data->terminate = TRUE;
        break;
    }
    case GST_MESSAGE_DURATION:{
        // The duration has changed, mark the current one as invalid
        data->duration = GST_CLOCK_TIME_NONE;
        break;
    }
    case GST_MESSAGE_STATE_CHANGED: {
        GstState old_state, new_state, pending_state;
        gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
        if (GST_MESSAGE_SRC (msg) == GST_OBJECT (data->playbin)) {
            g_print ("Pipeline state changed from %s to %s:\n",
                     gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));

            cout<<"Remember whether we are in the PLAYING state or not"<<endl;
            data->playing = (new_state == GST_STATE_PLAYING);

            if (data->playing) {
                cout<<"We just moved to PLAYING. Check if seeking is possible"<<endl;
                GstQuery *query;
                gint64 start, end;
                query = gst_query_new_seeking (GST_FORMAT_TIME);
                if (gst_element_query (data->playbin, query)) {
                    gst_query_parse_seeking (query, NULL, &data->seek_enabled, &start, &end);
                    if (data->seek_enabled) {
                        g_print ("Seeking is ENABLED from %" GST_TIME_FORMAT " to %" GST_TIME_FORMAT "\n",
                                 GST_TIME_ARGS (start), GST_TIME_ARGS (end));
                    } else {
                        g_print ("Seeking is DISABLED for this stream.\n");
                    }
                }
                else {
                    g_printerr ("Seeking query failed.");
                }
                gst_query_unref (query);
            }
        }
        break;
    }
    default:
        // We should not reach here
        g_printerr ("Unexpected message received.\n");
        break;
    }
    gst_message_unref (msg);
}

}

int time_management(int argc, char *argv[])
{
    CustomData data;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;

    data.duration = GST_CLOCK_TIME_NONE;

    cout<<"Initialize GStreamer"<<endl;
    gst_init (&argc, &argv);

    cout<<"Create the elements"<<endl;
    data.playbin = gst_element_factory_make ("playbin", "playbin");

    if (!data.playbin) {
        g_printerr ("Not all elements could be created.\n");
        return -1;
    }

    cout<<"Set the URI to play"<<endl;
    g_object_set (data.playbin, "uri", "https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm", NULL);

    cout<<"Start playing"<<endl;
    ret = gst_element_set_state (data.playbin, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        gst_object_unref (data.playbin);
        return -1;
    }

    cout<<"Listen to the bus"<<endl;
    bus = gst_element_get_bus (data.playbin);
    do {
        msg = gst_bus_timed_pop_filtered (bus, 100 * GST_MSECOND,
                                          GstMessageType(GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS | GST_MESSAGE_DURATION));

        cout<<"Parse message"<<endl;
        if (msg != NULL) {
            handle_message (&data, msg);
        } else {
            cout<<"We got no message, this means the timeout expired"<<endl;
            if (data.playing) {
                gint64 current = -1;

                cout<<"Query the current position of the stream"<<endl;
                if (!gst_element_query_position (data.playbin, GST_FORMAT_TIME, &current)) {
                    g_printerr ("Could not query current position.\n");
                }

                cout<<"If we didn't know it yet, query the stream duration"<<endl;
                if (!GST_CLOCK_TIME_IS_VALID (data.duration)) {
                    if (!gst_element_query_duration (data.playbin, GST_FORMAT_TIME, &data.duration)) {
                        g_printerr ("Could not query current duration.\n");
                    }
                }

                cout<<"Print current position and total duration"<<endl;
                g_print ("Position %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r",
                         GST_TIME_ARGS (current), GST_TIME_ARGS (data.duration));

                cout<<"If seeking is enabled, we have not done it yet, and the time is right, seek"<<endl;
                if (data.seek_enabled && !data.seek_done && current > 10 * GST_SECOND) {
                    g_print ("\nReached 10s, performing seek...\n");
                    gst_element_seek_simple (data.playbin, GST_FORMAT_TIME,
                                             GstSeekFlags(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT), 30 * GST_SECOND);
                    data.seek_done = TRUE;
                }
            }
        }
    } while (!data.terminate);

    cout<<"Free resources"<<endl;
    gst_object_unref (bus);
    gst_element_set_state (data.playbin, GST_STATE_NULL);
    gst_object_unref (data.playbin);
    return 0;
}

