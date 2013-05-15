/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <QDebug>
#include <QFile>
#include <QEvent>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/input.h>
#include "inputcontrol.h"

#define SUSPEND_THREAD_TIMEOUT 3

static InputControl* m_instance = NULL;

InputControl::InputControl(QObject *parent)
    : QThread(parent)
{
    if(m_instance) delete m_instance;
    m_instance = this;

    initialize();
}

InputControl::~InputControl()
{
    m_quit = true;
}

void InputControl::initialize()
{
    m_quit = false;
    m_connected = false;
}

void InputControl::cleanup()
{
    if(m_fd)
        close(m_fd);
}

bool InputControl::openDevice(const QString& devicePath)
{
        m_fd = -1;
        if ((m_fd = open(devicePath.toUtf8(), O_RDONLY)) < 0)
        {
                //qDebug() << "Could not read device " << devicePath;
                return false;
        }

        return true;
}

void InputControl::run()
{
    while (!m_quit)
    {
        if (!m_connected)
        {
            if (openDevice(m_devicePath))
            {
                m_connected = true;
                // qDebug() << "devicepath found and connected";
            }
            else
            {
                // qDebug() << "devicepath not connected, sleep for 10 secs";
                sleep(10);
            }
        }
        else
        {
            struct input_event ev;
            int rd = read(m_fd, &ev, sizeof(struct input_event));
            if (rd >= (int) sizeof(struct input_event))
            {
                    /*
                    qDebug() << "Event type: " << ev.type;
                    qDebug() << "Event code: " << ev.code;
                    qDebug() << "Event value: " << ev.value;
                    */

                    this->inputTriggered(ev.type, ev.code, ev.value);
            }
            else {
                //qDebug() << "Cannot read from device, maybe unplugged";
                m_connected = false;
            }
        }
    }

    cleanup();
}

void InputControl::setDevicePath(QString value)
{
    m_devicePath = value;

    if(!this->isRunning())
    {
        this->start();
    }

    emit devicePathChanged();
}



