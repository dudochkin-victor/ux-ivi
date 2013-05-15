/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef TTSCONTROL_H
#define TTSCONTROL_H

#include <QObject>
#include <QQueue>
#include <QtCore/QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QProcess>

class TTSControl: public QThread
{
	Q_OBJECT
        Q_PROPERTY(bool isOn READ isOn WRITE setIsOn NOTIFY isOnChanged)

public:

        TTSControl(QObject* = 0);
        ~TTSControl();
        void run();

public slots:
        void say(QString sentence);
        void setIsOn(bool enabled);
        bool isOn() { return m_isOn; }

signals:
        void isOnChanged();

private:
        void initialize();
        void cleanup();
        bool m_initialized;
        bool m_isOn;
        bool m_paused;
        bool m_quit;
        int m_timer;
        QString m_voice;
        QQueue<QString> m_speechQueue;
        QMutex m_mutex;

        QWaitCondition waitCondition;
        QMutex waitConditionMutex;
};

#endif // TTSCONTROL_H
