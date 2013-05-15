/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <QDebug>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "ttscontrol.h"

#define SUSPEND_THREAD_TIMEOUT 3

TTSControl::TTSControl(QObject *parent)
    : QThread(parent)
{
    initialize();
}

TTSControl::~TTSControl()
{
    m_quit = true;
}

void TTSControl::initialize()
{
    m_paused = false;
    m_quit = false;
    m_timer = 0;
    m_isOn = true;
    m_initialized = true;

    if(m_isOn)
    {
        this->start();
    }

    emit isOnChanged();
}

void TTSControl::cleanup()
{
    m_mutex.lock();
    while(!m_speechQueue.isEmpty())
        m_speechQueue.dequeue();
    m_mutex.unlock();
}

void TTSControl::run()
{
    QString sentence;

    while(!m_quit)
    {
        // too much voice commands queued up, dumping everything except last
        if(!m_speechQueue.isEmpty())
        {
            // acquire lock
            m_mutex.lock();
            do {
                sentence = m_speechQueue.dequeue();
                // qDebug() << "Removing from queue: " << sentence;
            }
            while(!m_speechQueue.isEmpty());
            m_mutex.unlock();

            if(m_isOn && sentence.trimmed() != "")
            {
                QProcess process;
                QString command = "festival --tts";
                process.start(command);
                process.waitForStarted();
                process.write(sentence.toLatin1());
                process.closeWriteChannel();
                process.waitForFinished();
                qDebug() << "Say: " << sentence;
            }
        }

        usleep(10000);
        m_timer += 10000;

        if(m_timer > (SUSPEND_THREAD_TIMEOUT * 1000000))
        {
            // this means no TTS commands after 5 seconds, suspend thread
            waitConditionMutex.lock();
            m_paused = true;
            qDebug() << "TTS thread suspended";
            waitCondition.wait(&waitConditionMutex);
            m_paused = false;
            waitConditionMutex.unlock();

            // thread continued, reset timer
            m_timer = 0;
        }
    }

    cleanup();
}

void TTSControl::say(QString sentence)
{
    if(!m_initialized || !m_isOn)
        return;

    // acuire lock
    m_mutex.lock();
    m_speechQueue.enqueue(sentence);
    m_mutex.unlock();
    qDebug() << "Adding to queue: " << sentence;

    if(m_paused)
    {
        waitCondition.wakeAll();
    }
}

void TTSControl::setIsOn(bool enabled)
{
    qDebug()<<"Setting tts to "<< enabled;

    if(this->isRunning() && enabled)
    {
     this->start();
    }

    m_isOn = enabled;
    emit isOnChanged();
}



