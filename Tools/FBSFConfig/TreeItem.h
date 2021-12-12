/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef TREEITEM_H
#define TREEITEM_H
#include "ItemParams.h"

#include <QList>
#include <QVariant>
#include <QDebug>

class ParamList;
class TreeItem
{
public:
    // Constructor for new items
    explicit TreeItem(QString aName,QString aType,
                      QString aCategory=QString(""),
                      QString aModuleType=QString(""),
                      TreeItem *parent = nullptr);
    // Constructor for xml items
    explicit TreeItem(QMap<QString,QString>& aXmlParamList,
                      QString aName,QString aType,
                      QString aCategory=QString(""),
                      QString aModuleType=QString(""),
                      TreeItem *parent = nullptr);

    ~TreeItem();

    TreeItem&       operator=(const TreeItem& other);
                    TreeItem(const TreeItem &other);

    void            createItemParamList();
    void            getXmlItemData(QString& aXmlConfig,int level=0);
    QList<QObject*>& getParamList();
    QString         checkParamList();

    TreeItem        *child(int row);
    int             childCount() const;
    QList<TreeItem*>&    childItems(){return mChildItems;}
    int             columnCount() const;
    QVariant        data(int column) const;
    bool            setData(int column, const QVariant &value);

    int             row() const;
    void            setParentItem(TreeItem *parentItem);
    TreeItem *      parentItem();

    bool            insertChildren(int position, int count, QVector<QVariant> &data);
    bool            removeChildren(int position, int count);
    bool            moveChild(int from, int to);

    void            insertChild(TreeItem *item,int position);
    void            appendChild(TreeItem *child);

    QString         name() const {return data(0).toString(); }
    void            name(const QString aName)  {mDescriptor[0]=aName; }
    QString         type() const {return data(1).toString(); }
    QString         category() const {return data(2).toString(); }
    QString         moduleType() const {return data(3).toString(); }

    bool            isSelected() const {return mSelected; }
    void            setSelected(bool aFlag) {mSelected=aFlag;}

    bool            hasError() const {return mHasError; }
    void            hasError(bool aFlag) {mHasError=aFlag;}

    TreeItem *      getItem(const QModelIndex &index) const;

private:
    QList<TreeItem*>    mChildItems;
    QVector<QVariant>   mDescriptor;
    ParamList           mItemParams;
    TreeItem *          mParentItem=nullptr;
    bool                mSelected=false;
    bool                mHasError=false;

};
//! [0]

static QString typeConfig="config";
static QString typeFork="fork";
static QString typeSequence="sequence";
static QString typeSubSequence="SubSequence";
static QString typePluginList="plugins";
static QString typeModule="module";
static QString typePlugin="plugin";
static QString typeVisual="visual";
static QString typeFmu="fmu";
static QString typeManual="manual";
#endif // TREEITEM_H
