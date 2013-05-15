/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <QDebug>
#include <QFile>
#include <glib.h>
#include "voicecontrol.h"

gboolean VoiceControl::message_callback(GstBus *bus,GstMessage *msg,gpointer data)
{
    Q_UNUSED(bus);
    const GstStructure *s = gst_message_get_structure(msg);
    emit ((VoiceControl *)data)->speechRecognized(gst_structure_get_string(s,"hyp"));
    qDebug() << "(hyp): " << gst_structure_get_string(s,"hyp");
    return TRUE;
}

void VoiceControl::result_callback(GstElement* object,gchararray arg0, gchararray arg1, gpointer user_data)
{
    Q_UNUSED(user_data);
    GstStructure *s = gst_structure_new("result",
                                        "hyp", G_TYPE_STRING, arg0,
                                        "uttid", G_TYPE_STRING, arg1,
                                        NULL);
    gst_element_post_message(object,gst_message_new_application(GST_OBJECT(object),s));
}


VoiceControl::VoiceControl(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
    m_isOn = false;
}

VoiceControl::~VoiceControl()
{
    if(m_initialized)
        cleanup();
    m_initialized = false;
    m_isOn = false;
}

void VoiceControl::initialize()
{
    gst_init(NULL, NULL);

    m_gst_pipeline = gst_pipeline_new("voicecontrol");

    if(!m_gst_pipeline)
    {
        qDebug() << "Create gst pipepline failed - gstreamer failed to initialize";
        return;
    }

    m_source = gst_element_factory_make("pulsesrc" , "src");

    if (!m_source)
    {
        qDebug() << "Create pulsesrc source failed";
        return;
    }

    m_filter1 = gst_element_factory_make("audioconvert","filter1");

    if (!m_filter1)
    {
        qDebug() << "Create audioconvert filter failed";
        return;
    }

    m_filter2 = gst_element_factory_make("audioresample","filter2");

    if (!m_filter2)
    {
        qDebug() << "Create audioresample filter failed";
        return;
    }

    m_filter3 = gst_element_factory_make("vader","filter3");

    if (!m_filter3)
    {
        qDebug() << "Create vader filter failed";
        return;
    }

    g_object_set(G_OBJECT(m_filter3),"threshold",0.015625,NULL);

    m_filter4 = gst_element_factory_make("pocketsphinx","filter4");

    if(!m_filter4)
    {
        qDebug() << "Create pocketsphinx filter failed";
        return;
    }

    QFile am(m_accousticModel);
    if(!am.exists())
    {
        qDebug() << "Accoustic model directory doesn't exists: " << m_accousticModel;
        return;
    }

    QFile dict(m_dictionary);
    if(!dict.exists())
    {
        qDebug() << "Dictioanry doesn't exists: " << m_dictionary;
        return;
    }

    QFile fsg(m_grammar);
    QFile lm(m_languageModel);
    if(!fsg.exists() && !lm.exists())
    {
        qDebug() << "Finite state grammar or language model doesn't exists: ";
        qDebug() << "FSG - " << m_grammar;
        qDebug() << "LM - " << m_languageModel;
        return;
    }

    g_object_set(G_OBJECT(m_filter4), "hmm", (const char*) m_accousticModel.toLatin1(), NULL);
    g_object_set(G_OBJECT(m_filter4), "dict", (const char*) m_dictionary.toLatin1(), NULL);
    if(fsg.exists())
        g_object_set(G_OBJECT(m_filter4), "fsg", (const char*) m_grammar.toLatin1(), NULL);
    else
        g_object_set(G_OBJECT(m_filter4), "lm", (const char*) m_languageModel.toLatin1(), NULL);

    m_sink = gst_element_factory_make("fakesink","sink");

    if(!m_sink)
    {
        qDebug() << "Create fakesink failed";
        return;
    }

    gst_bin_add_many(GST_BIN (m_gst_pipeline),
                     m_source,
                     m_filter1,
                     m_filter2,
                     m_filter3,
                     m_filter4,
                     m_sink,
                     NULL);

    if(!gst_element_link_many(m_source,
                              m_filter1,
                              m_filter2,
                              m_filter3,
                              m_filter4,
                              m_sink,
                              NULL))
    {
        qDebug() << "Gst failed to link";
        return;
    }

    g_object_set(G_OBJECT(m_filter4),"configured",TRUE,NULL);
    g_signal_connect(m_filter4,"result",G_CALLBACK(result_callback),this);

    m_bus = gst_pipeline_get_bus (GST_PIPELINE (m_gst_pipeline));
    gst_bus_add_signal_watch(m_bus);
    g_signal_connect(m_bus,"message::application",G_CALLBACK(message_callback),(gpointer)this);
    gst_element_set_state (m_gst_pipeline, GST_STATE_PAUSED);

    m_initialized = true;
}

void VoiceControl::cleanup()
{
    gst_object_unref(m_bus);
    gst_object_unref(GST_OBJECT(m_source));
    gst_object_unref(GST_OBJECT(m_filter1));
    gst_object_unref(GST_OBJECT(m_filter2));
    gst_object_unref(GST_OBJECT(m_filter3));
    gst_object_unref(GST_OBJECT(m_filter4));
    gst_object_unref(GST_OBJECT(m_sink));
    gst_object_unref (m_gst_pipeline);
}

void VoiceControl::setIsOn(bool enabled)
{
    qDebug()<<"Setting voice control to "<< enabled;

    if(!m_initialized && enabled)
    {
        initialize();
    }

    if(m_initialized)
    {
        qDebug() << "Speech recognition initialized";
        if(!m_isOn && enabled)
        {
            gst_element_set_state(GST_ELEMENT(m_gst_pipeline),GST_STATE_PLAYING);
            m_isOn = true;
            qDebug() << "Speech recognition turned on";
        }
        else if(m_isOn && !enabled)
        {
            g_object_set(G_OBJECT(m_filter3),"silent",TRUE,NULL);
            gst_element_set_state(GST_ELEMENT(m_gst_pipeline),GST_STATE_PAUSED);
            m_isOn = false;
            qDebug() << "Speech recognition turned off";
        }
    }
    else
        qDebug() << "Speech recognition failed to initialize";

    emit isOnChanged();
}



