/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <QtDeclarative/qdeclarative.h>
#include <QDir>
#include <QFileInfoList>
#include <QFileSystemWatcher>
#include <QRegExp>
#include <mdesktopentry.h>
#include "menumodel.h"
#include "desktop.h"

MenuModel::MenuModel(QObject *parent) :
    QAbstractItemModel(parent),
    m_type("Application")
{
    m_watcher = new QFileSystemWatcher(this);
    connect(m_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(appsDirChanged(QString)));

    // Default dirs
    m_watcher->addPath("/usr/share/applications");
    m_watcher->addPath("/usr/share/meego-ux-ivi/applications");

    QHash<int, QByteArray> roles;
    roles[id]="id";
    roles[name]="name";
    roles[exec]="exec";
    roles[comment]="comment";
    roles[icon]="icon";
    roles[filename]="filename";
    roles[custom]="custom";

    setRoleNames(roles);

}

void MenuModel::appsDirChanged(QString changedDir)
{
    QList<Desktop *> added;
    QList<Desktop *> removed;
    QDir dir (changedDir);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);

    foreach (QFileInfo info, dir.entryInfoList())
    {
        bool found = false;
        foreach (Desktop *d, m_apps)
        {
            if (info.absoluteFilePath().endsWith(d->filename().right(d->filename().lastIndexOf('/'))))
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            Desktop *desktopEntry = new Desktop(info.absoluteFilePath());
            if (!desktopEntry->isValid() || (desktopEntry->type() != m_type))
            {
                delete desktopEntry;
            }
            else
            {
                added << desktopEntry;
            }
        }
    }

    foreach (Desktop *d, m_apps)
    {
        if (!d->filename().contains(changedDir))
            continue;

        bool found = false;
        foreach (QFileInfo info, dir.entryInfoList())
        {
            if (d->filename() == info.absoluteFilePath())
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            removed << d;
        }
    }

    if (removed.length() > 0)
    {
        QList<Desktop *> tmp;
        while (!m_apps.isEmpty())
        {
            Desktop *d = m_apps.takeLast();
            if (removed.contains(d))
            {
                delete d;
            }
            else
            {
                tmp << d;
            }
        }

        m_apps = tmp;
    }

    if (added.length() > 0)
    {
        foreach (Desktop *d, added)
        {
            m_apps << d;
        }
    }

    emit appsChanged();
}

void MenuModel::setDirectories(QStringList directories)
{
    m_watcher->removePaths(m_watcher->directories());

    foreach(QString directory, directories)
    {
        QString path;
        if (directory.startsWith("~"))
            path = QDir::homePath() + directory.remove(0, 1);
        else if (directory == ".")
            path = QDir::currentPath();
        else
            path = directory;

        // If directory doesn't exist, then attempt to create it
        if (!QDir(path).exists())
        {
            QDir().mkpath(path);
        }

        m_watcher->addPath(path);
    }

    resetApps();
}

void MenuModel::resetApps()
{
    bool found = false;

    while (!m_apps.isEmpty())
        delete m_apps.takeFirst();

    if (m_rootItem)
    {
        delete m_rootItem;
        m_rootItem = NULL;
    }
    m_appsHash.clear();

    buildMenuTreeModel();

    QStringList addedDirectories;

    foreach (QString target, m_watcher->directories())
    {
        QDir dir(target);
        dir.setFilter(QDir::Files | QDir::NoSymLinks);

        QStringList filters;
        filters << "*.desktop";
        foreach (QFileInfo fileInfo, dir.entryInfoList(filters))
        {

            if(!fileInfo.fileName().endsWith(".desktop"))
                continue;

            if(addedDirectories.contains(fileInfo.fileName()))
                continue;

            addedDirectories<<fileInfo.fileName();

            Desktop *desktopEntry = new Desktop(fileInfo.absoluteFilePath());
            if (!desktopEntry->isValid() || (desktopEntry->type() != m_type))
            {
                delete desktopEntry;
                continue;
            }

            m_apps << desktopEntry;

            found = false;
            foreach(QString category, desktopEntry->categories())
            {
                // check if this desktop entry's category matches a menu's category list
                if(!found && m_appsHash.contains(category.toLower()))
                {
                    MenuItem* menu = m_appsHash[category.toLower()];
                    MenuItem* child = new MenuItem(desktopEntry, menu);
                    menu->appendChild(child);
                    found = true;
                }
            }

            if(!found)
            {
                // no match, add it to the "More Apps" menu, the hash key is '*'
                MenuItem* menu = m_appsHash["*"];
                if(menu)
                {
                    MenuItem* child = new MenuItem(desktopEntry, menu);
                    menu->appendChild(child);
                }
            }
        }
    }

    emit appsReset();
}

void MenuModel::buildMenuTreeModel()
{
    MenuItem *parent;

    /* build top level menu */
    m_defaultRootItem = m_rootItem = new MenuItem(NULL);

    addCategory(m_rootItem,
                "Navigation",
                "/usr/share/themes/1024-600-10/icons/ivi/nav.svg",
                "Geography,GPS,Navigation");

    addCategory(m_rootItem,
                "Driver Assistance",
                "/usr/share/themes/1024-600-10/icons/ivi/car.svg",
                "");

    parent = addCategory(m_rootItem,
                         "Communication",
                         "/usr/share/themes/1024-600-10/icons/ivi/communication.svg",
                         "");

    addCategory(parent,
                "Phone",
                "/usr/share/themes/1024-600-10/icons/ivi/dialer.svg",
                "Telephony,TelephonyTools");

    addCategory(parent,
                "Web",
                "/usr/share/themes/1024-600-10/icons/ivi/web.svg",
                "Webservices,WebBrowser,Internet");

    addCategory(parent,
                "Connect",
                "/usr/share/themes/1024-600-10/icons/ivi/connect.svg",
                "Chat,Communications,Email,InstantMessaging,IRCClient,FileTransfer,News,P2P,RemoteAccess,VideoConference");

    parent = addCategory(m_rootItem,
                         "Media",
                         "/usr/share/themes/1024-600-10/icons/ivi/media.svg",
                         "");

    addCategory(parent,
                "Photos",
                "/usr/share/themes/1024-600-10/icons/ivi/photos.svg",
                "Graphics,Photography,Viewer");

    addCategory(parent,
                "Music",
                "/usr/share/themes/1024-600-10/icons/ivi/music.svg",
                "Audio,Music");

    addCategory(parent,
                "Video",
                "/usr/share/themes/1024-600-10/icons/ivi/video.svg",
                "AudioVideo,Video");

    parent = addCategory(m_rootItem,
                         "Other",
                         "/usr/share/themes/1024-600-10/icons/ivi/folder.svg",
                         "");

    addCategory(parent,
                "Activities",
                "/usr/share/themes/1024-600-10/icons/ivi/activities.svg",
                "Education,Game");

    addCategory(parent,
                "Settings",
                "/usr/share/themes/1024-600-10/icons/ivi/settings.svg",
                "Settings,DesktopSettings,HardwareSettings");

    addCategory(parent,
                "Tools",
                "/usr/share/themes/1024-600-10/icons/ivi/tools.svg",
                "Office,Security,Archiving,System,Documentation");

    addCategory(parent,
                "More Apps",
                "/usr/share/themes/1024-600-10/icons/ivi/app.svg",
                "*");
}

MenuItem* MenuModel::addCategory(MenuItem *parent, QString name, QString icon, QString groups)
{
    MenuItem *category;

    category = new MenuItem(NULL, parent);
    category->setCategoryName(name);
    category->setCategoryIcon(icon);
    QList<QString> grouplist = groups.toLower().split(",");
    foreach (QString group, grouplist)
    {
        /* create a hash for retrieve the menu item directly */
        m_appsHash[group] = category;
    }

    category->setCategoryGroups(grouplist);
    parent->appendChild(category);
    m_categories << category;
    return category;
}

QModelIndex MenuModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    MenuItem *parentItem;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<MenuItem*>(parent.internalPointer());

    MenuItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex MenuModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    MenuItem *childItem = static_cast<MenuItem*>(child.internalPointer());
    MenuItem *parentItem = childItem->parent();

    if (parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int MenuModel::rowCount(const QModelIndex &parent) const
{
    MenuItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<MenuItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int MenuModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<MenuItem*>(parent.internalPointer())->columnCount();
    else
        return m_rootItem->columnCount();
}

QVariant MenuModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    MenuItem *item = static_cast<MenuItem*>(index.internalPointer());

    if(role == name ||
       role == exec ||
       role == icon ||
       role == comment ||
       role == filename ||
       role == custom)
    {
        return item->data(index.column(), role);
    }
    return QVariant();
}

bool MenuModel::hasChildren ( const QModelIndex & parent ) const
{
    if(rowCount(parent) > 0)
        return true;
    else
        return false;
}

QDeclarativeListProperty<Desktop> MenuModel::apps()
{
    return QDeclarativeListProperty<Desktop>(this, m_apps);
}

QString MenuModel::value(QString id, QString key)
{
	foreach(Desktop* item, m_apps)
	{
		if(item->id() == id)
			return item->value(key);
	}
	return "";
}

void MenuModel::launchDesktop(QObject *object)
{
    Desktop *target = qobject_cast<Desktop *>(object);
    if (!target)
        return;

    QString cmd = target->exec();

    // http://standards.freedesktop.org/desktop-entry-spec/latest/ar01s06.html
    cmd.replace(QRegExp("%k"), target->filename());
    cmd.replace(QRegExp("%i"), QString("--icon ") + target->icon());
    cmd.replace(QRegExp("%c"), target->title());
    cmd.replace(QRegExp("%[fFuU]"), target->filename()); // stuff we don't handle

    QProcess::startDetached(cmd);

    emit startingApp(target->title(), target->icon());
}

void MenuModel::launchDesktopByName(QString name)
{

    if (m_apps.length() == 0)
        return;

    for (int i = 0; i < m_apps.length(); i++)
    {
        Desktop *d = m_apps[i];
        if (d->filename().contains(name))
        {
            launchDesktop(d);
            return;
        }
    }
}

void MenuModel::launch(QString cmd)
{
    QProcess::startDetached(cmd);
}

void MenuModel::setRootIndex(QString name)
{
    beginResetModel();
    if(name.isEmpty())
        m_rootItem = m_defaultRootItem;
    else
    {
        foreach (MenuItem* cat, m_categories)
        {
            if(cat->getCategoryName() == name)
                m_rootItem = cat;
        }
    }
    endResetModel();
}

void MenuModel::setRootIndexToParent()
{
    beginResetModel();
    if(m_rootItem->parent() != NULL)
        m_rootItem = m_rootItem->parent();
    endResetModel();
}

bool MenuModel::isRoot()
{
    if(m_rootItem == m_defaultRootItem)
        return true;
    else
        return false;
}

int MenuModel::getParentIndex()
{
    if(m_rootItem->parent() != NULL)
    {
        m_rootItem = m_rootItem->parent();
        return m_rootItem->row();
    }
    else
        return 0;
}

int MenuModel::getChildCount(int idx)
{
    int size = rowCount(index(idx, 0, QModelIndex()));
    return size;
}

QVariant MenuModel::getNameByIndex(int idx)
{
    return data(index(idx, 0, QModelIndex()), name);
}

QVariant MenuModel::getExecByIndex(int idx)
{
    return data(index(idx, 0, QModelIndex()), exec);
}

QVariant MenuModel::getCommentByIndex(int idx)
{
    return data(index(idx, 0, QModelIndex()), comment);
}

QVariant MenuModel::getIconByIndex(int idx)
{
    return data(index(idx, 0, QModelIndex()), icon);
}

QVariant MenuModel::getFileNameByIndex(int idx)
{
    return data(index(idx, 0, QModelIndex()), filename);
}

MenuModel::~MenuModel()
{
    while (!m_apps.isEmpty())
        delete m_apps.takeFirst();
    delete m_rootItem;
}

QML_DECLARE_TYPE(MenuModel);
