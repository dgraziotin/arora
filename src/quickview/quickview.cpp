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
#include <QFile>
#include <QHash>
#include <qalgorithms.h>
#include <quickviewfiltermodel.h>
#include <QBuffer>
#include <QByteArray>
#include <QSettings>

bool compareHistoryFrecencyEntries(const HistoryFrecencyEntry& a, const HistoryFrecencyEntry& b)
{
    // for a reverse quickSort.
    return a > b;
}

HistoryFrecencyEntry::HistoryFrecencyEntry(const QString &u, const QDateTime &d, const QString &t, const int f, const QString &i):
    HistoryEntry(u, d, t), frecency(f), icon(i)  {}

QList<HistoryFrecencyEntry> QuickView::mostVisitedEntries(int numberEntries)
{

    if(numberEntries < 1)
        return QList<HistoryFrecencyEntry>();

    QuickViewFilterModel* model = BrowserApplication::historyManager()->quickViewFilterModel();
    int rowCount = model->rowCount();

    if(rowCount < numberEntries)
        numberEntries = rowCount;

    QList<HistoryFrecencyEntry> list;

    for(int i = 0; i < numberEntries; i++) {
        QModelIndex index = model->index(i, 0, QModelIndex());
        QUrl url(index.data(HistoryModel::UrlRole).toString());
        QDateTime datetime = index.data(HistoryModel::DateTimeRole).toDateTime();
        QString finalUrl = url.scheme() + QString::fromLatin1("://") + url.host();
        QString finalTitle = url.host();
        int frecency =  index.data(HistoryFilterModel::FrecencyRole).toInt();
        QIcon icon = BrowserApplication::instance()->icon(url);
        HistoryFrecencyEntry entry(finalUrl, datetime, finalTitle, frecency, this->toBase64(icon));
        list.append(entry);
    }
    qSort(list.begin(), list.end(), compareHistoryFrecencyEntries);
    return list;
}

QString QuickView::mostVisitedEntriesHTML(QList<HistoryFrecencyEntry> mostVisitedEntries)
{
    if(mostVisitedEntries.isEmpty())
        return QLatin1String("<p>No recent websites</p>");
    QString htmlMessage;
    QString linkFormat = QLatin1String("<p><a href=\"%1\"><img src=\"data:image/png;base64,%3\"/>%2</a></p>");
    for(int i = 0; i < mostVisitedEntries.size(); i++) {
        QUrl entryUrl(mostVisitedEntries.at(i).url);
        QString finalUrl = entryUrl.scheme() + QString::fromLatin1("://") + entryUrl.host();
        QString finalTitle = entryUrl.host();
        QString icon = mostVisitedEntries.at(i).icon;
        QString entry = linkFormat.arg(finalUrl, finalTitle, icon);
        htmlMessage += entry;
    }
    return htmlMessage;
}


QByteArray QuickView::quickViewPage(QString mostVisitedEntriesHTML)
{
    QFile quickViewPage(QLatin1String(":/quickview.html"));
    if(!quickViewPage.open(QIODevice::ReadOnly))
        return QByteArray("");

    QString html = QLatin1String(quickViewPage.readAll());

    html = html.arg(mostVisitedEntriesHTML);
    return QByteArray(html.toLatin1());
}

QByteArray QuickView::render(int numberEntries)
{
    QList<HistoryFrecencyEntry> mostVisited = mostVisitedEntries(numberEntries);
    QString mostVisitedHtmlEntries = mostVisitedEntriesHTML(mostVisited);
    return quickViewPage(mostVisitedHtmlEntries);
}


QHash<QString, int> QuickView::getFrecencies(int numberEntries)
{

    if(numberEntries < 1)
        return QHash<QString, int>();

    QuickViewFilterModel* model = BrowserApplication::historyManager()->quickViewFilterModel();
    int rowCount = model->rowCount();

    if(rowCount < numberEntries)
        numberEntries = rowCount;

    QHash<QString, int> list;

    for(int i = 0; i < numberEntries; i++) {
        QModelIndex index = model->index(i, 0, QModelIndex());

        QUrl url(index.data(HistoryModel::UrlRole).toString());
        QString finalUrl = url.scheme() + QString::fromLatin1("://") + url.host();
        int frec = index.data(HistoryFilterModel::FrecencyRole).toInt();
        list.insert(finalUrl, frec);
    }
    return list;
}

QString QuickView::toBase64(QIcon icon)
{
    if(icon.isNull())
        return QString();
    QImage image(icon.pixmap(20, 20).toImage());
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    image.save(&buffer, "PNG"); // writes image into ba in PNG format.
    return QString::fromLatin1(byteArray.toBase64().data());
}

