#ifndef GSTREAMER_MEMORY_HPP
#define GSTREAMER_MEMORY_HPP

#include <memory>

struct _GstBus;
struct _GstCaps;
struct _GstElement;
struct _GstMessage;
struct _GstPad;

struct gst_bus_deleter
{
public:
    void operator()(struct _GstBus *ptr) const;
};

struct gst_caps_deleter
{
public:
    void operator()(struct _GstCaps *ptr) const;
};

struct gst_element_deleter
{
public:
    explicit gst_element_deleter(int operation_when_delete = 1);
    ~gst_element_deleter() = default;

    void operator()(struct _GstElement *ptr) const;

private:
    int operation_; //GST_STATE_NULL
};

struct gst_message_deleter
{
public:
    void operator()(struct _GstMessage *ptr) const;
};

struct gst_pad_deleter
{
public:
    void operator()(struct _GstPad *ptr) const;
};

using unique_gst_bus = std::unique_ptr<struct _GstBus, gst_bus_deleter>;
using unique_gst_caps = std::unique_ptr<struct _GstCaps, gst_caps_deleter>;
using unique_gst_elem = std::unique_ptr<struct _GstElement, gst_element_deleter>;
using unique_gst_msg = std::unique_ptr<struct _GstMessage, gst_message_deleter>;
using unique_gst_pad = std::unique_ptr<struct _GstPad, gst_pad_deleter>;

#endif // GSTREAMER_MEMORY_HPP
