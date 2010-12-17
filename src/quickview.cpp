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

#include <quickview.h>
#include <browserapplication.h>
#include <history.h>
#include <historymanager.h>
#include <QDateTime>

QList<HistoryEntry> QuickView::getLastHistoryEntries(int numberEntries){

    if (numberEntries < 1)
        return QList<HistoryEntry>();

    QuickViewFilterModel* model = BrowserApplication::historyManager()->quickViewFilterModel();
    int rowCount = model->rowCount();

    if (rowCount < numberEntries)
        numberEntries = rowCount;

    QList<HistoryEntry> list;

    for (int i = 0; i < numberEntries; i++){
        QModelIndex index = model->index(i,0,QModelIndex());
        QString url = index.data(HistoryModel::UrlRole).toString();
        QString title = index.data(HistoryModel::TitleRole).toString();
        QDateTime datetime = index.data(HistoryModel::DateTimeRole).toDateTime();
        HistoryEntry entry(url, datetime, title);
        /*
        HistoryEntry(const QString &u,
                    const QDateTime &d = QDateTime(), const QString &t = QString())
                : url(u), title(t), dateTime(d) {}
        */
        list.append(entry);
    }
    return list;
}

QString QuickView::getHtmlMessage(QList<HistoryEntry> history){
    if(history.isEmpty())
        return QString();
    QString link = QLatin1String("<p><a href=\"%1\">%2</a></p>");
    for(int i = 0;i < history.length();i++)
            link += link.arg(history.at(i).url, history.at(i).title);
    return link;
}
