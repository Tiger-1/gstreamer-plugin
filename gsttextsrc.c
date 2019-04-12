/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2019 hugo <<user@hostname.org>>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-textsrc
 *
 * FIXME:Describe textsrc here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! textsrc ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>

#include "gsttextsrc.h"

GST_DEBUG_CATEGORY_STATIC (gst_textsrc_debug);
#define GST_CAT_DEFAULT gst_textsrc_debug

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_SILENT,
  PROP_TEXT,
  PROP_CLOCK,
  PROP_TIME,
  PROP_TXPOS,
  PROP_CPOS,
  PROP_TPOS
};

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
//static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
//    GST_PAD_SINK,
//    GST_PAD_ALWAYS,
//    GST_STATIC_CAPS ("ANY")
//    );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

#define gst_textsrc_parent_class parent_class
G_DEFINE_TYPE (Gsttextsrc, gst_textsrc, GST_TYPE_BASE_SRC);

static void gst_textsrc_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_textsrc_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

//static gboolean gst_textsrc_sink_event (GstPad * pad, GstObject * parent, GstEvent * event);
//static gboolean gst_textsrc_src_event (GstPad *pad, GstObject * parent, GstEvent * event);
//static gboolean gst_textsrc_src_query (GstPad *pad, GstObject * parent, GstQuery * query);
/* GObject vmethod implementations */


static gboolean gst_textsrc_start (GstBaseSrc * basesrc);
static gboolean gst_textsrc_stop (GstBaseSrc * basesrc);
//static gboolean gst_textsrc_size (GstBaseSrc * src, guint64 * size);
static GstFlowReturn gst_textsrc_fill (GstBaseSrc * src, guint64 offset,
    guint length, GstBuffer * buf);

static gboolean gst_textsrc_set_data(Gsttextsrc *src, const gchar *text);




/* initialize the textsrc's class */
static void
gst_textsrc_class_init (GsttextsrcClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  
  GstBaseSrcClass *gstbasesrc_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;
  gstbasesrc_class = (GstBaseSrcClass *) klass;

  gobject_class->set_property = gst_textsrc_set_property;
  gobject_class->get_property = gst_textsrc_get_property;

//  g_object_class_install_property (gobject_class, PROP_SILENT,
//	  g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
//          FALSE, G_PARAM_READWRITE));

  g_object_class_install_property(gobject_class, PROP_TEXT,
		  g_param_spec_string("text","Text Message",
			  "Adhoc text message for continuous stream", FALSE,
			  G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
			  GST_PARAM_MUTABLE_READY));
  g_object_class_install_property(gobject_class, PROP_CLOCK,
		  g_param_spec_boolean("clock","True or false",
			  "clock stream should be on or off", FALSE,
			  G_PARAM_READWRITE | GST_PARAM_MUTABLE_READY));

  g_object_class_install_property(gobject_class, PROP_TIME,
		  g_param_spec_boolean("time","True or false",
			  "Time stream should be on or off", FALSE,
			  G_PARAM_READWRITE | GST_PARAM_MUTABLE_READY));
  
  g_object_class_install_property(gobject_class, PROP_TXPOS,
		  g_param_spec_int("txpos","Center top bottom corner", 
			  "Position of text stream", 0, 7, 0,
			  G_PARAM_READWRITE | GST_PARAM_MUTABLE_READY));

  g_object_class_install_property(gobject_class, PROP_TPOS,
		  g_param_spec_int("tpos","Center top bottom corner", 
			  "Position of time stream", 0, 7, 0,
			  G_PARAM_READWRITE | GST_PARAM_MUTABLE_READY));

  g_object_class_install_property(gobject_class, PROP_CPOS,
		  g_param_spec_int("cpos","Center top bottom corner", 
			  "Position of clock stream", 0, 7, 0,
			  G_PARAM_READWRITE | GST_PARAM_MUTABLE_READY));

  gst_element_class_set_details_simple(gstelement_class,
    "Text Source",
    "Source/Text",
    "Encoded text stream for video",
    "Abhinav <imabhinavjain08@gmail.com>");

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&src_factory));

//   gst_element_class_add_static_pad_template (gstelement_class, &src_factory);

  gstbasesrc_class->start = GST_DEBUG_FUNCPTR (gst_textsrc_start);
  gstbasesrc_class->stop = GST_DEBUG_FUNCPTR (gst_textsrc_stop);
 // gstbasesrc_class->get_size = GST_DEBUG_FUNCPTR (gst_textsrc_size);
  gstbasesrc_class->fill = GST_DEBUG_FUNCPTR (gst_textsrc_fill);

  g_print("In gst_class_init.\n");
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_textsrc_init (Gsttextsrc * filter)
{
   filter->srcpad = gst_pad_new_from_static_template (&src_factory, "textsrc");
    
//gst_pad_set_event_function (filter->srcpad,
//		   GST_DEBUG_FUNCPTR(gst_textsrc_src_event));
//   gst_pad_set_query_function (filter->srcpad,
//		   GST_DEBUG_FUNCPTR(gst_textsrc_src_query));

//  GST_PAD_SET_PROXY_CAPS (filter->sinkpad);
  GST_PAD_SET_PROXY_CAPS (filter->srcpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

  filter->silent = FALSE;
  filter->text = NULL;
  filter->clock = FALSE;
  filter->time = FALSE;
  filter->txpos = 0;
  filter->tpos = 0;
  filter->cpos = 0;

	g_print("In gst_textsrc_init \n");
  gst_base_src_set_blocksize(GST_BASE_SRC(filter), 4096);
}

static void
gst_textsrc_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  Gsttextsrc *filter = GST_TEXTSRC (object);

  switch (prop_id) {
    case PROP_SILENT:
      filter->silent = g_value_get_boolean (value);
      break;
    case PROP_TEXT:
     // filter->text = g_value_get_string (value);
      gst_textsrc_set_data(filter, g_value_get_string(value));
      break;
    case PROP_TIME:
      filter->time = g_value_get_boolean (value);
      break;
    case PROP_CLOCK:
      filter->clock = g_value_get_boolean (value);
      break;
    case PROP_TXPOS:
      filter->txpos = g_value_get_int (value);
      break;
    case PROP_TPOS:
      filter->tpos = g_value_get_int (value);
      break;
    case PROP_CPOS:
      filter->cpos = g_value_get_int (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_textsrc_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  Gsttextsrc *filter = GST_TEXTSRC (object);

  switch (prop_id) {
    case PROP_SILENT:
      g_value_set_boolean (value, filter->silent);
      break;
    case PROP_TEXT:
      g_value_set_string (value, filter->text);
      break;
    case PROP_TIME:
      g_value_set_boolean (value, filter->time);
      break;
    case PROP_CLOCK:
      g_value_set_boolean (value, filter->clock);
      break;
    case PROP_TXPOS:
      g_value_set_boolean (value, filter->txpos);
      break;
    case PROP_TPOS:
      g_value_set_boolean (value, filter->tpos);
      break;
    case PROP_CPOS:
      g_value_set_boolean (value, filter->cpos);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* GstElement vmethod implementations */
/* this function handles src querys */
#if 0
static gboolean
gst_textsrc_src_query (GstPad * pad, GstObject * parent, GstQuery * query)
{
  Gsttextsrc *filter;
  gboolean ret;

  filter = GST_TEXTSRC (parent);
  GST_LOG_OBJECT (filter, "Received %s query: %" GST_PTR_FORMAT,
	GST_QUERY_TYPE_NAME (query), query);

  switch (GST_QUERY_TYPE (query)) {
    case GST_QUERY_CAPS:
    {
      GstCaps * caps;

      gst_query_parse_caps(query, &caps);

      ret = gst_pad_query_default (pad, parent, query);
      break;
    }
    default:
      ret = gst_pad_query_default (pad, parent, query);
      break;
  }
  return ret;
}
/* this function handles src events */
static gboolean
gst_textsrc_src_event (GstPad * pad, GstObject * parent, GstEvent * event)
{
  Gsttextsrc *filter;
  gboolean ret;

  filter = GST_TEXTSRC (parent);

  GST_LOG_OBJECT (filter, "Received %s event: %" GST_PTR_FORMAT,
      GST_EVENT_TYPE_NAME (event), event);

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_CAPS:
    {
      GstCaps * caps;

      gst_event_parse_caps (event, &caps);
      /* do something with the caps */

      /* and forward */
      ret = gst_pad_event_default (pad, parent, event);
      break;
    }
    default:
      ret = gst_pad_event_default (pad, parent, event);
      break;
  }
  return ret;
}

/* chain function
 * this function does the actual processing
 */

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
#endif

static gboolean
gst_textsrc_set_data(Gsttextsrc *src, const gchar *text){

	GstState state;

	GST_OBJECT_LOCK(src);
	state = GST_STATE (src);

	if(state != GST_STATE_READY && state != GST_STATE_NULL)
		g_print("WRONG STATE\n");
	GST_OBJECT_UNLOCK(src);

	g_free(src->text);

	if(text == NULL)
		src->text = NULL;
	else{
		src->text = g_strdup(text);
		GST_INFO("Text: %s", src->text);
	}

	g_object_notify(G_OBJECT(src), "text");

	return TRUE;

}



static gboolean
gst_textsrc_start (GstBaseSrc * basesrc)
{
 // Gsttextsrc *src = GST_BASE_SRC (basesrc);
//  struct stat stat_results;

	
  return TRUE;
}


static gboolean 
gst_textsrc_stop (GstBaseSrc * basesrc)
{
	return TRUE;
}
static gboolean 
gst_textsrc_size (GstBaseSrc * src, guint64 * size)
{
	return TRUE;
}
static GstFlowReturn 
gst_textsrc_fill (GstBaseSrc * src, guint64 offset,
    guint length, GstBuffer * buf)
{

  Gsttextsrc *tsrc = (Gsttextsrc *)src;
  GstMapInfo info;

	return GST_FLOW_OK;
}




static gboolean
textsrc_init (GstPlugin * textsrc)
{
  /* debug category for fltering log messages
   *
   * exchange the string 'Template textsrc' with your description
   */
  GST_DEBUG_CATEGORY_INIT (gst_textsrc_debug, "textsrc",
      0, "Template textsrc");

  return gst_element_register (textsrc, "textsrc", GST_RANK_NONE,
      GST_TYPE_TEXTSRC);
}

/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirsttextsrc"
#endif

/* gstreamer looks for this structure to register textsrcs
 *
 * exchange the string 'Template textsrc' with your textsrc description
 */
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    textsrc,
    "Template textsrc",
    textsrc_init,
    VERSION,
    "LGPL",
    "GStreamer",
    "http://gstreamer.net/"
)
