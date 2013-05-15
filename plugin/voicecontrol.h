/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef VOICECONTROL_H
#define VOICECONTROL_H

#include <QObject>
#include <gst/gst.h>

class VoiceControl: public QObject
{
	Q_OBJECT
        Q_PROPERTY(bool isOn READ isOn WRITE setIsOn NOTIFY isOnChanged)
        Q_PROPERTY(QString accousticModel READ accousticModel WRITE setAccousticModel NOTIFY accousticModelChanged)
        Q_PROPERTY(QString dictionary READ dictionary WRITE setDictionary NOTIFY dictionaryChanged)
        Q_PROPERTY(QString grammar READ grammar WRITE setGrammar NOTIFY grammarChanged)
        Q_PROPERTY(QString languageModel READ languageModel WRITE setLanguageModel NOTIFY languageModelChanged)

public:

        VoiceControl(QObject* = 0);
        ~VoiceControl();

public slots:
        void setIsOn(bool enabled);
        bool isOn() { return m_isOn; }
        void setAccousticModel(QString value) {
            m_accousticModel = value;
            emit accousticModelChanged();
        }
        QString accousticModel() { return m_accousticModel; }
        void setDictionary(QString value) {
            m_dictionary = value;
            emit dictionaryChanged();
        }
        QString dictionary() { return m_dictionary; }
        void setGrammar(QString value) {
            m_grammar = value;
            emit grammarChanged();
        }
        QString grammar() { return m_grammar; }
        void setLanguageModel(QString value) {
            m_languageModel = value;
            emit languageModelChanged();
        }
        QString languageModel() { return m_languageModel; }

signals:
        void isOnChanged();
        void accousticModelChanged();
        void dictionaryChanged();
        void grammarChanged();
        void languageModelChanged();
        void speechRecognized(const QString str);

private:
        void initialize();
	void cleanup();
        static void result_callback(GstElement* object, gchararray arg0, gchararray arg1, gpointer user_data);
        static gboolean message_callback(GstBus *bus, GstMessage *msg, gpointer data);

        bool m_initialized;
        bool m_isOn;
        QString m_accousticModel;
        QString m_dictionary;
        QString m_grammar;
        QString m_languageModel;
        GstElement *m_gst_pipeline;
        GstElement *m_source, *m_filter1, *m_filter2, *m_filter3, *m_filter4, *m_sink;
        GstBus *m_bus;
};

#endif // TTSCONTROL_H
