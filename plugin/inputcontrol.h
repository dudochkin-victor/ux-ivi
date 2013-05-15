/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef INPUTCONTROL_H
#define INPUTCONTROL_H

#include <QObject>
#include <QThread>

class InputControl: public QThread
{
	Q_OBJECT
        Q_PROPERTY(QString devicePath READ devicePath WRITE setDevicePath NOTIFY devicePathChanged)

public:

        InputControl(QObject* = 0);
        ~InputControl();
        void run();

public slots:
        void setDevicePath(QString value);
        QString devicePath() { return m_devicePath; }

signals:
        void devicePathChanged();
        void inputTriggered(int type, int code, int value);

private:
        void initialize();
        void cleanup();
        bool openDevice(const QString &devicePath);
        bool m_quit;
        int m_fd;
        QString m_devicePath;
        bool m_connected;
};

#endif // INPUTCONTROL_H
