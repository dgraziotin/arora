/*
 * Copyright 2010 Daniel Graziotin <daniel.graziotin@acm.org>
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

#ifndef QUICKVIEW_H
#define QUICKVIEW_H


#include <qdatetime.h>
#include <qhash.h>
#include <qobject.h>
#include <qsortfilterproxymodel.h>
#include <qtimer.h>
#include <qurl.h>
#include <history.h>
#include <historymanager.h>
#include <qwebhistoryinterface.h>


class QuickView {
public:
    QList<HistoryEntry> getLastHistoryEntries(int numberEntries);
    QString getHtmlMessage(QList<HistoryEntry> history);
    QByteArray getHtmlPage(QString htmlMessage);
    QByteArray render(int numberEntries);
    QHash<QString, int> getFrecencies(int numberEntries);
};

#endif // QUICKVIEW_H
