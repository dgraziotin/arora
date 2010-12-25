/*
 * Copyright 2008 Benjamin C. Meyer <ben@meyerhome.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
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
** Exception version 1.2, which can be found at
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

#ifndef QUICKVIEWFITERMODEL_H
#define QUICKVIEWFITERMODEL_H

#include "modelmenu.h"

#include <qdatetime.h>
#include <qhash.h>
#include <qobject.h>
#include <qsortfilterproxymodel.h>
#include <qtimer.h>
#include <qurl.h>

#include <qwebhistoryinterface.h>
#include <history/historymanager.h>
#include <history/history.h>

/*!
    Proxy model that will remove any duplicate entries.
    Both m_sourceRow and m_historyHash store their offsets not from
    the front of the list, but as offsets from the back.
  */
class QuickViewFilterModel : public QAbstractProxyModel
{
    Q_OBJECT

public:
    QuickViewFilterModel(QAbstractItemModel *sourceModel, QObject *parent = 0);

    inline bool historyContains(const QString &url) const {
        return m_historyHash.contains(url);
    }
    int historyLocation(const QString &url) const;

    enum Roles {
        FrecencyRole = HistoryModel::MaxRole + 1,
        MaxRole = FrecencyRole
    };

    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    void setSourceModel(QAbstractItemModel *sourceModel);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex index(int, int, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index = QModelIndex()) const;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool isValid(const QUrl url) const;
    void recalculateFrecencies();

private slots:
    void sourceReset();
    void sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void sourceRowsInserted(const QModelIndex &parent, int start, int end);
    void sourceRowsRemoved(const QModelIndex &, int, int);

private:
    void load() const;

    struct HistoryData {
        int tailOffset;
        int frecency;

        HistoryData(int off, int f = 0) : tailOffset(off), frecency(f) { }

        bool operator==(const HistoryData &other) const {
            return (tailOffset == other.tailOffset)
                   && (frecency == -1 || other.frecency == -1 || frecency == other.frecency);
        }
        bool operator!=(const HistoryData &other) const {
            return !(*this == other);
        }
        // like the actual history entries, our index mapping data is sorted in reverse
        bool operator<(const HistoryData &other) const {
            return (tailOffset > other.tailOffset);
        }
    };
    int frecencyScore(const QModelIndex &sourceIndex) const;

    mutable QList<HistoryData> m_filteredRows;
    mutable QHash<QString, int> m_historyHash;
    mutable bool m_loaded;
    mutable QDateTime m_scaleTime;
};


#endif // QUICKVIEWFITERMODEL_H

