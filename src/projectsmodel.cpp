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

#include <QStringList>

#include <QtGui>
#include "projectsmodel.h"

 ProjectsItem::ProjectsItem(const QList<QVariant> &data, ProjectsItem *parent)
 {
     parentItem = parent;
     itemData = data;
 }

 ProjectsItem::~ProjectsItem()
 {
     qDeleteAll(childItems);
 }

 void ProjectsItem::appendChild(ProjectsItem *item)
 {
     childItems.append(item);
 }

 ProjectsItem *ProjectsItem::child(int row)
 {
     return childItems.value(row);
 }

 int ProjectsItem::childCount() const
 {
     return childItems.count();
 }

 int ProjectsItem::columnCount() const
 {
     return itemData.count();
 }

 QVariant ProjectsItem::data(int column) const
 {
     return itemData.value(column);
 }

 ProjectsItem *ProjectsItem::parent()
 {
     return parentItem;
 }

 int ProjectsItem::row() const
 {
     if (parentItem)
         return parentItem->childItems.indexOf(const_cast<ProjectsItem*>(this));

     return 0;
 }




 ProjectsModel::ProjectsModel(const QString &data,QObject *parent)
     : QAbstractItemModel(parent)
 {
     QList<QVariant> rootData;
     rootData << "Title";
     rootItem = new ProjectsItem(rootData);
	 setupModelData(data.split(QString("\n")), rootItem);
 }

 ProjectsModel::~ProjectsModel()
 {
     delete rootItem;
 }

 int ProjectsModel::columnCount(const QModelIndex &parent) const
 {
     if (parent.isValid())
         return static_cast<ProjectsItem*>(parent.internalPointer())->columnCount();
     else
         return rootItem->columnCount();
 }

 QVariant ProjectsModel::data(const QModelIndex &index, int role) const
 {
     if (!index.isValid())
         return QVariant();

     if (role != Qt::DisplayRole)
         return QVariant();

     ProjectsItem *item = static_cast<ProjectsItem*>(index.internalPointer());

     return item->data(index.column());
 }

 Qt::ItemFlags ProjectsModel::flags(const QModelIndex &index) const
 {
     if (!index.isValid())
         return 0;

     return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
 }

 QVariant ProjectsModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
 {
     if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
         return rootItem->data(section);

     return QVariant();
 }

 QModelIndex ProjectsModel::index(int row, int column, const QModelIndex &parent)
             const
 {
     if (!hasIndex(row, column, parent))
         return QModelIndex();

     ProjectsItem *parentItem;

     if (!parent.isValid())
         parentItem = rootItem;
     else
         parentItem = static_cast<ProjectsItem*>(parent.internalPointer());

     ProjectsItem *childItem = parentItem->child(row);
     if (childItem)
         return createIndex(row, column, childItem);
     else
         return QModelIndex();
 }

 QModelIndex ProjectsModel::parent(const QModelIndex &index) const
 {
     if (!index.isValid())
         return QModelIndex();

     ProjectsItem *childItem = static_cast<ProjectsItem*>(index.internalPointer());
     ProjectsItem *parentItem = childItem->parent();

     if (parentItem == rootItem)
         return QModelIndex();

     return createIndex(parentItem->row(), 0, parentItem);
 }

 int ProjectsModel::rowCount(const QModelIndex &parent) const
 {
     ProjectsItem *parentItem;
     if (parent.column() > 0)
         return 0;

     if (!parent.isValid())
         parentItem = rootItem;
     else
         parentItem = static_cast<ProjectsItem*>(parent.internalPointer());

     return parentItem->childCount();
 }

 void ProjectsModel::setupModelData(const QStringList &lines, ProjectsItem *parent)
 {
     QList<ProjectsItem*> parents;
     QList<QString> dirs;
     parents << parent;
     dirs << "rootihopenobodynamestheirdirectorythisinsanename";

	int number = 0;
	while (number < lines.count()-1) { 
		QStringList pathItems;
		pathItems << "rootihopenobodynamestheirdirectorythisinsanename";
		pathItems << lines[number].split("/"); 
		//Find out if pathItems parents and dir parents are same
		for(int j=1;j<dirs.size()||j<(pathItems.size()-1);j++) {
			if(j>=(pathItems.size()-1)) {
				//We found a partial path so pop back few parents
				for(int k=j;k<dirs.size();k++) {
					dirs.pop_back();
					parents.pop_back();
				}
				break;
			} else if(j>=dirs.size()) {
				//We found that an dir subchild(s) added
				for(int k=j;k<(pathItems.size()-1);k++) {
					dirs << pathItems[k];
					QList<QVariant> data;
					data << pathItems[k];
					parents.last()->appendChild(new ProjectsItem(data, parents.last()));
					parents << parents.last()->child(parents.last()->childCount()-1);
				}
				break;
			} else if(dirs[j] != pathItems[j]) {
				//pop remaining dirs and then add remaining paths items
				for(int k=j;k<=dirs.size();k++) {
					dirs.pop_back();
					parents.pop_back();
				}
				for(int k=j;k<(pathItems.size()-1);k++) {
					dirs << pathItems[k];
					QList<QVariant> data;
					data << pathItems[k];
					parents.last()->appendChild(new ProjectsItem(data, parents.last()));
					parents << parents.last()->child(parents.last()->childCount()-1);
				}
				break;
			}
				
		}
		//add the new child
		QList<QVariant> data2;
		data2 << pathItems.last();
		parents.last()->appendChild(new ProjectsItem(data2, parents.last()));

		number++;
	}

}
