#ifndef __PROJECTSMODEL_H__
#define __PROJECTSMODEL_H__

 /****************************************************************************
 **
 ** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
 **
 ** This file is part of the example classes of the Qt Toolkit.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License versions 2.0 or 3.0 as published by the Free Software
 ** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
 ** included in the packaging of this file.  Alternatively you may (at
 ** your option) use any later version of the GNU General Public
 ** License if such license has been publicly approved by Trolltech ASA
 ** (or its successors, if any) and the KDE Free Qt Foundation. In
 ** addition, as a special exception, Trolltech gives you certain
 ** additional rights. These rights are described in the Trolltech GPL
 ** Exception version 1.1, which can be found at
 ** http://www.trolltech.com/products/qt/gplexception/ and in the file
 ** GPL_EXCEPTION.txt in this package.
 **
 ** Please review the following information to ensure GNU General
 ** Public Licensing requirements will be met:
 ** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
 ** you are unsure which license is appropriate for your use, please
 ** review the following information:
 ** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
 ** or contact the sales department at sales@trolltech.com.
 **
 ** In addition, as a special exception, Trolltech, as the sole
 ** copyright holder for Qt Designer, grants users of the Qt/Eclipse
 ** Integration plug-in the right for the Qt/Eclipse Integration to
 ** link to functionality provided by Qt Designer and its related
 ** libraries.
 **
 ** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
 ** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
 ** granted herein.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ****************************************************************************/

 #include <QList>
 #include <QVariant>

 #include <QAbstractItemModel>
 #include <QModelIndex>
 #include <QVariant>


 enum STATUS { NORMAL, MODIFIED, MERGE_CONFLICT };
 class ProjectsItem
 {
 public:
     ProjectsItem(const QList<QVariant> &data, ProjectsItem *parent = 0);
     ~ProjectsItem();

     void appendChild(ProjectsItem *child);

     ProjectsItem *child(int row);
     int childCount() const;
     int columnCount() const;
     QVariant data(int column) const;

     int row() const;
     int status;
     ProjectsItem *parent();

     QList<ProjectsItem*> childItems;

     private:
     QList<QVariant> itemData;
     ProjectsItem *parentItem;
 };


 class ProjectsModel : public QAbstractItemModel
 {
     Q_OBJECT

 public:
     ProjectsModel(const QString &data,QObject *parent = 0,const QString &title="title");
     ~ProjectsModel();

     QString filepath(const QModelIndex &index);
     QVariant data(const QModelIndex &index, int role) const;
     Qt::ItemFlags flags(const QModelIndex &index) const;
     QVariant headerData(int section, Qt::Orientation orientation,
                         int role = Qt::DisplayRole) const;
     QModelIndex index(int row, int column,
                       const QModelIndex &parent = QModelIndex()) const;
     QModelIndex parent(const QModelIndex &index) const;
     int rowCount(const QModelIndex &parent = QModelIndex()) const;
     int columnCount(const QModelIndex &parent = QModelIndex()) const;

     void setFilesModified(QString files);

private:

	QString modifiedFiles;
     void setupModelData(const QStringList &lines, ProjectsItem *parent);
	QModelIndex search(const QString &string);
     ProjectsItem *rootItem;
 };



#endif // __PROJECTSMODEL_H__
