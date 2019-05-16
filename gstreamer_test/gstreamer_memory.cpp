#include "gstreamer_memory.hpp"

extern "C"
{

#include <gst/gst.h>

}


void gst_bus_deleter::operator()(GstBus *ptr) const
{
    if(ptr){
        gst_object_unref(ptr);
    }
}

void gst_caps_deleter::operator()(GstCaps *ptr) const
{
    if(ptr){
        gst_caps_unref(ptr);
    }
}

gst_element_deleter::gst_element_deleter(int operation_when_delete) :
    operation_(operation_when_delete)
{

}

void gst_element_deleter::operator()(GstElement *ptr) const
{
    if(ptr){
        if(operation_ >= 0){
            gst_element_set_state (ptr, static_cast<GstState>(operation_));
        }
        gst_object_unref(ptr);
    }
}

void gst_pad_deleter::operator()(GstPad *ptr) const
{
    if(ptr){
        gst_object_unref(ptr);
    }
}


void gst_message_deleter::operator()(_GstMessage *ptr) const
{
    if(ptr){
        gst_message_unref(ptr);
    }
}
