/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef MENUITEM_H
#define MENUITEM_H

#include <QList>
#include <QVariant>
#include <desktop.h>

class MenuItem
{
public:
    MenuItem(Desktop* data, MenuItem *parent = 0);
    ~MenuItem();

    void appendChild(MenuItem *child);
    void setCategoryName(QString name);
    QString getCategoryName();
    void setCategoryIcon(QString icon);
    QString getCategoryIcon();
    void setCategoryGroups(QList<QString> groups);
    QList<QString> getCategoryGroups();
    bool containsGroup(QString group);

    MenuItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column, int role) const;
    int row() const;
    MenuItem *parent();

private:
    QList<MenuItem*> childItems;
    Desktop* itemData;
    MenuItem *parentItem;

    QString categoryName;
    QString categoryIcon;
    QList<QString> categoryGroups;
};

#endif
