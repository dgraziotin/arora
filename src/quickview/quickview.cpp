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
#include <QDebug>

bool compareHistoryFrecencyEntries(const HistoryFrecencyEntry& a, const HistoryFrecencyEntry& b)
{
    // for a reverse quickSort.
    return a > b;
}

HistoryFrecencyEntry::HistoryFrecencyEntry(const QString &u, const QDateTime &d, const QString &t, const int f, const QString &i):
    HistoryEntry(u, d, t), frecency(f), icon(i)  {}

QuickView::QuickView(QObject* parent)
    :QObject(parent)
{
    QuickView::QuickView(0 , parent);
}

QuickView::QuickView(int numberEntries, QObject* parent)
    :QObject(parent)
{
    if (numberEntries > 0 && numberEntries != s_defaultMaxNumberEntries)
        m_maxNumberEntries = numberEntries;
    else
        m_maxNumberEntries = s_defaultMaxNumberEntries;

    m_mostVisitedEntries = QList<HistoryFrecencyEntry>();
    calculate();
    m_timer = new QTimer(this);
    m_timer->setInterval(s_msUpdateInterval);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(calculate()));
    m_timer->start();
}

QList<HistoryFrecencyEntry> QuickView::mostVisitedEntries()
{
    return m_mostVisitedEntries;
}

void QuickView::calculate(){

    QuickViewFilterModel* model = BrowserApplication::historyManager()->quickViewFilterModel();

    int rowCount = model->rowCount();
    int numberEntries = m_maxNumberEntries;

    if(rowCount < numberEntries)
        numberEntries = rowCount;

    m_mostVisitedEntries.clear();

    for(int i = 0; i < numberEntries; i++) {
        QModelIndex index = model->index(i, 0, QModelIndex());
        QUrl url(index.data(HistoryModel::UrlRole).toString());
        if (!url.isEmpty()){
            QDateTime datetime = index.data(HistoryModel::DateTimeRole).toDateTime();
            QString finalUrl = url.scheme() + QString::fromLatin1("://") + url.host();
            QString finalTitle = url.host();
            int frecency =  index.data(HistoryFilterModel::FrecencyRole).toInt();
            QIcon icon = BrowserApplication::instance()->icon(url);
            HistoryFrecencyEntry entry(finalUrl, datetime, finalTitle, frecency, this->toBase64(icon));
            m_mostVisitedEntries.append(entry);
        }
    }
    qSort(m_mostVisitedEntries.begin(), m_mostVisitedEntries.end(), compareHistoryFrecencyEntries);
}

QString QuickView::mostVisitedEntriesHTML()
{
    if(m_mostVisitedEntries.isEmpty())
        return QLatin1String("<p>No recent websites</p>");
    QString htmlMessage;
    QString linkFormat = QLatin1String("<p><a href=\"%1\"><img src=\"data:image/png;base64,%3\"/>%2</a></p>");
    for(int i = 0; i < m_mostVisitedEntries.size(); i++) {
        QUrl entryUrl(m_mostVisitedEntries.at(i).url);
        QString finalUrl = entryUrl.scheme() + QString::fromLatin1("://") + entryUrl.host();
        QString finalTitle = entryUrl.host();
        QString icon = m_mostVisitedEntries.at(i).icon;
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

QByteArray QuickView::render()
{
    QString mostVisitedHtmlEntries = mostVisitedEntriesHTML();
    return quickViewPage(mostVisitedHtmlEntries);
}


QHash<QString, int> QuickView::getFrecencies()
{
    QuickViewFilterModel* model = BrowserApplication::historyManager()->quickViewFilterModel();
    int rowCount = model->rowCount();

    int numberEntries = m_maxNumberEntries;

    if(rowCount < m_maxNumberEntries)
        numberEntries = rowCount;

    QHash<QString, int> list;

    for(int i = 0; i < m_maxNumberEntries; i++) {
        QModelIndex index = model->index(i, 0, QModelIndex());

        QUrl url(index.data(HistoryModel::UrlRole).toString());
        QString finalUrl = url.scheme() + QString::fromLatin1("://") + url.host();
        int frec = index.data(HistoryFilterModel::FrecencyRole).toInt();
        list.insert(finalUrl, frec);
    }
    return list;
}

int QuickView::maxNumberEntries(){
    return m_maxNumberEntries;
}

QString QuickView::toBase64(QIcon icon){
    if (icon.isNull())
        return QString();
    QImage image(icon.pixmap(20,20).toImage());
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    image.save(&buffer, "PNG"); // writes image into ba in PNG format.
    return QString::fromLatin1(byteArray.toBase64().data());
}
