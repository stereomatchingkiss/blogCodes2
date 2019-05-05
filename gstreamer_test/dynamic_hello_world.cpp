#include "dynamic_hello_world.hpp"

#include "gstreamer_memory.hpp"

extern "C"
{

#include <gst/gst.h>

}


#include <iostream>

using namespace std;

namespace{

// Structure to contain all our information, so we can pass it to callbacks
struct CustomData
{
    unique_gst_elem pipeline;
    GstElement *source;
    GstElement *audio_convert;
    GstElement *audio_sink;
    GstElement *video_convert;
    GstElement *video_sink;
};

// Handler for the pad-added signal
void pad_added_handler(GstElement *src, GstPad *new_pad, CustomData *data)
{
    g_print ("Received new pad '%s' from '%s':\n", GST_PAD_NAME (new_pad), GST_ELEMENT_NAME (src));

    cout<<"Check the new pad's type"<<endl;
    unique_gst_caps new_pad_caps(gst_pad_get_current_caps(new_pad));
    GstStructure *new_pad_struct = gst_caps_get_structure (new_pad_caps.get(), 0);
    const gchar *new_pad_type = gst_structure_get_name (new_pad_struct);
    if(g_str_has_prefix (new_pad_type, "audio/x-raw")) {
        unique_gst_pad sink_pad(gst_element_get_static_pad (data->audio_convert, "sink"));
        cout<<"If our converter is already linked, we have nothing to do here"<<endl;
        if(gst_pad_is_linked(sink_pad.get())){
            g_print ("We are already linked. Ignoring.\n");
            return;
        }

        cout<<"Attempt the link"<<endl;
        GstPadLinkReturn ret = gst_pad_link (new_pad, sink_pad.get());
        if (GST_PAD_LINK_FAILED (ret)) {
            g_print ("Type is '%s' but link failed.\n", new_pad_type);
        } else {
            g_print ("Link succeeded (type '%s').\n", new_pad_type);
        }

        return;
    }
    if(g_str_has_prefix (new_pad_type, "video/x-raw")) {
        unique_gst_pad sink_pad(gst_element_get_static_pad (data->video_convert, "sink"));
        cout<<"If our converter is already linked, we have nothing to do here"<<endl;
        if(gst_pad_is_linked(sink_pad.get())){
            g_print ("We are already linked. Ignoring.\n");
            return;
        }

        cout<<"Attempt the link"<<endl;
        GstPadLinkReturn ret = gst_pad_link (new_pad, sink_pad.get());
        if (GST_PAD_LINK_FAILED (ret)) {
            g_print ("Type is '%s' but link failed.\n", new_pad_type);
        } else {
            g_print ("Link succeeded (type '%s').\n", new_pad_type);
        }

        return;
    }
}

}

int dynamic_hello_world(int argc, char *argv[])
{
    cout<<"Initialize GStreamer"<<endl;
    gst_init (&argc, &argv);

    cout<<"Create the elements"<<endl;
    CustomData data;
    data.source = gst_element_factory_make ("uridecodebin", "source");
    data.audio_convert = gst_element_factory_make ("audioconvert", "audio_convert");
    data.audio_sink = gst_element_factory_make ("autoaudiosink", "audio_sink");
    data.video_convert = gst_element_factory_make ("videoconvert", "video_convert");
    data.video_sink = gst_element_factory_make ("autovideosink", "video_sink");

    cout<<"Create the empty pipeline"<<endl;    
    data.pipeline.reset(gst_pipeline_new("test-pipeline"));

    if(!data.pipeline || !data.source || !data.audio_convert || !data.audio_sink ||
            !data.video_convert || !data.video_sink) {
        g_printerr ("Not all elements could be created.\n");
        return -1;
    }

    cout<<"Build the pipeline. Note that we are NOT linking the source at this "
          "point. We will do it later."<<endl;
    gst_bin_add_many(GST_BIN (data.pipeline.get()), data.source, data.audio_convert , data.audio_sink,
                     data.video_convert, data.video_sink, nullptr);
    if(!gst_element_link (data.audio_convert, data.audio_sink)) {
        g_printerr ("Audio elements could not be linked.\n");
        return -1;
    }
    if(!gst_element_link (data.video_convert, data.video_sink)) {
        g_printerr ("Video elements could not be linked.\n");
        return -1;
    }

    cout<<"Set the URI to play"<<endl;
    g_object_set (data.source, "uri",
                  "https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm", nullptr);

    cout<<"Connect to the pad-added signal"<<endl;
    g_signal_connect (data.source, "pad-added", G_CALLBACK (pad_added_handler), &data);

    cout<<"Start playing"<<endl;
    GstStateChangeReturn const ret = gst_element_set_state (data.pipeline.get(), GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        return -1;
    }

    cout<<"Listen to the bus"<<endl;
    unique_gst_bus bus(gst_element_get_bus (data.pipeline.get()));
    bool terminate = false;
    do {
        unique_gst_msg msg(gst_bus_timed_pop_filtered (bus.get(), GST_CLOCK_TIME_NONE,
                                                       GstMessageType(GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR |
                                                                      GST_MESSAGE_EOS)));

        //cout<<"Parse message"<<endl;
        if(msg){
            GError *err;
            gchar *debug_info;

            switch (GST_MESSAGE_TYPE (msg.get())) {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error (msg.get(), &err, &debug_info);
                g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg.get()->src), err->message);
                g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
                g_clear_error (&err);
                g_free (debug_info);
                terminate = true;
                break;
            case GST_MESSAGE_EOS:
                g_print ("End-Of-Stream reached.\n");
                terminate = true;
                break;
            case GST_MESSAGE_STATE_CHANGED:
                cout<<"We are only interested in state-changed messages from the pipeline"<<endl;
                if (GST_MESSAGE_SRC (msg.get()) == GST_OBJECT (data.pipeline.get())) {
                    GstState old_state, new_state, pending_state;
                    gst_message_parse_state_changed (msg.get(), &old_state, &new_state, &pending_state);
                    g_print ("Pipeline state changed from %s to %s:\n",
                             gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
                }
                break;
            default:
                cout<<"We should not reach here"<<endl;
                g_printerr ("Unexpected message received.\n");
                break;
            }
        }
    } while(!terminate);

    cout<<"Free resources"<<endl;
    return 0;
}
