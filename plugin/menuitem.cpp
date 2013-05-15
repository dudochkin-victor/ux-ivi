/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <QStringList>

#include "menuitem.h"
#include "menumodel.h"

MenuItem::MenuItem(Desktop *data, MenuItem *parent)
{
    parentItem = parent;
    itemData = data;
}

MenuItem::~MenuItem()
{
    qDeleteAll(childItems);
}

void MenuItem::setCategoryName(QString name)
{
    if(!itemData)
        categoryName = name;
}

QString MenuItem::getCategoryName()
{
    return categoryName;
}

void MenuItem::setCategoryIcon(QString icon)
{
    if(!itemData)
        categoryIcon = icon;
}

QString MenuItem::getCategoryIcon()
{
    return categoryIcon;
}

void MenuItem::setCategoryGroups(QList<QString> groups)
{
    if(!itemData)
    {
        while(!categoryGroups.isEmpty())
            categoryGroups.removeFirst();
        foreach (QString item, groups)
            categoryGroups << item;
    }
}

QList<QString> MenuItem::getCategoryGroups()
{
    return categoryGroups;
}

bool MenuItem::containsGroup(QString group)
{
    return categoryGroups.contains(group);
}

void MenuItem::appendChild(MenuItem *item)
{
    childItems.append(item);
}

MenuItem *MenuItem::child(int row)
{
    return childItems.value(row);
}

int MenuItem::childCount() const
{
    return childItems.count();
}

int MenuItem::columnCount() const
{
    return 1;
}

QVariant MenuItem::data(int column, int role) const
{
    Q_UNUSED(column);
    if(role == MenuModel::name)
    {
        if(itemData)
            return itemData->title();
        else
            return categoryName;
    }
    else if(role == MenuModel::exec)
    {
        if(itemData)
            return itemData->exec();
        else
            return "";
    }
    else if(role == MenuModel::comment)
    {
        if(itemData)
            return itemData->comment();
        else
            return "";
    }
    else if(role == MenuModel::icon)
    {
        if(itemData)
            return itemData->icon();
        else
            return categoryIcon;
    }
    else if(role == MenuModel::filename)
    {
        if(itemData)
            return itemData->filename();
        else
            return "";
    }
    else if(role == MenuModel::custom)
    {
/*
        if(!itemData->contains(m_customValue))
        {
            qDebug()<<"couldn't find custom field: "<< m_customValue;
        }
        return itemData->value(m_customValue);
 */
    }

    return QVariant();
}

MenuItem *MenuItem::parent()
{
    return parentItem;
}

int MenuItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<MenuItem*>(this));

    return 0;
}
