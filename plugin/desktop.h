/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef DESKTOP_H
#define DESKTOP_H

#include <QDebug>
#include <QFile>
#include <QObject>
#include <QIcon>
#include <mdesktopentry.h>

class Desktop : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id);
    Q_PROPERTY(QString type READ type NOTIFY typeChanged);
    Q_PROPERTY(QString title READ title NOTIFY titleChanged);
    Q_PROPERTY(QString comment READ comment NOTIFY commentChanged);
    Q_PROPERTY(QString icon READ icon NOTIFY iconChanged);
    Q_PROPERTY(QString exec READ exec NOTIFY execChanged);
    Q_PROPERTY(QStringList categories READ categories NOTIFY categoriesChanged);
    Q_PROPERTY(QString filename READ filename NOTIFY filenameChanged);

public:
    Desktop(const QString &filename, QObject *parent = 0);
    ~Desktop();

    QString id() const {
        return m_id;
    }

    bool isValid() const {
        QStringList onlyShowIn = m_entry->onlyShowIn();
        if (!onlyShowIn.isEmpty() && !onlyShowIn.contains("X-MEEGO") &&
            !onlyShowIn.contains("X-MEEGO-HS"))
            return false;

        QStringList notShowIn = m_entry->notShowIn();
        if (!notShowIn.isEmpty() && (notShowIn.contains("X-MEEGO") ||
                                     notShowIn.contains("X-MEEGO-HS")))
            return false;

        return m_entry->isValid();
    }

    QString type() const {
        return m_entry->type();
    }

    QString title() const {
        return m_entry->name();
    }

    QString comment() const {
        return m_entry->comment();
    }

    QString icon() const {
        return QString("image://systemicon/") + m_entry->icon();
    }

    QString exec() const {
        return m_entry->exec();
    }

    QStringList categories() const {
        return m_entry->categories();
    }

    QString filename() const {
        return m_filename;
    }    Q_PROPERTY(QString exec READ exec NOTIFY execChanged);

    enum Role {
        Type = Qt::UserRole + 1,
        Title = Qt::UserRole + 2,
        Comment = Qt::UserRole + 3,
        Icon = Qt::UserRole + 4,
        Exec = Qt::UserRole + 5,
        Categories = Qt::UserRole + 6,
        Filename = Qt::UserRole + 7
    };

public slots:

    QString value(QString key) const {
        return m_entry->value(key);
    }

    bool contains(QString val) const {
        return m_entry->contains(val);
    }

    bool uninstall() {
        if (m_entry->type() == "Widget")
        {
            return QFile::remove(m_entry->fileName());
        }

        return false;
    }

signals:
    void typeChanged();
    void titleChanged();
    void commentChanged();
    void iconChanged();
    void execChanged();
    void categoriesChanged();
    void filenameChanged();

private:
    QString m_filename;
    MDesktopEntry *m_entry;
    QString m_id;

    bool m_assigned;
    Q_DISABLE_COPY(Desktop)
};

#endif // DESKTOP_H
