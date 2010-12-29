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

/**
 * A specialized HistoryEntry that also holds the frecency value and favicon.ico.
 * The operators <, > and = are overloaded to automatically handle
 * sorting and comparison based on the frecency
 */
class HistoryFrecencyEntry: public HistoryEntry
{
public:
    /**
     * Constructor of a HistoryFrecencyEntry object. Every field is mandatory.
     * @param url the URL of the Entry (a domain or a host)
     * @param datetime the datetime of the last visit - not used
     * @param title the title of the last visited page - not used
     * @param frecency the retrieved frecency of the entry
     * @param icon the favicon.ico of the entry
     */
    HistoryFrecencyEntry(const QString &url,
                         const QDateTime &datetime = QDateTime(),
                         const QString &title = QString(),
                         const int frecency = -1,
                         const QIcon &icon = QIcon());

    inline bool operator <(const HistoryFrecencyEntry &other) const {
        return frecency < other.frecency;
    }

    inline bool operator >(const HistoryFrecencyEntry &other) const {
        return frecency > other.frecency;
    }

    inline bool operator ==(const HistoryFrecencyEntry &other) const {
        return frecency == other.frecency;
    }
    /**
     * Holds the frecency (recency + frequency) of the Entry
     */
    int frecency;
    /**
     * Holds the favicon.ico file of the Entry
     */
    QIcon icon;
};

/**
 * QuickView is a new modality inspired by Google Chrome, Safari and Opera.
 * Based on frecency data, it uses QuickViewFilterModel to retrieve the
 * most visited sites (hosts or domains). The list is created in descending
 * order (first entry is the one with highest frecency).
 */
class QuickView
{

public:
    /**
     * Given a number n of entries, it retrieves the most n visited hosts
     * and encapsulates them in a QList of HistoryFrecencyEntry objects.
     * The order is descending, the first element of the list has the
     * highest frecency.
     * @param numberEntries how many HistoryFrecencyEntry must be generated
     * @return a list of HistoryFrecencyEntry objects, order descending
     * @see HistoryFrecencyEntry
     */
    QList<HistoryFrecencyEntry> mostVisitedEntries(int numberEntries);
    /**
     * Converts a list of HistoryFrecencyEntry objects into a QString
     * encoded in HTML representing the objects.
     * @param mostVisitedEntries a list of the most
     * @return a QString representation in HTML of the most visited entries
     */
    QString mostVisitedEntriesHTML(QList<HistoryFrecencyEntry> mostVisitedEntries);
    /**
     * Loads a HTML file and replaces the place marker %1 in the HTML file with
     * the supplied QString. It returns the substituted HTML file as a QByteArray
     * object.
     * @param mostVisitedEntriesHTML the most visited entries
     * @return the QuickView HTML page with the most visited entries
     */
    QByteArray quickViewPage(QString mostVisitedEntriesHTML);
    /**
     * Convenience method that should be the only one public.
     * Given a number of entries, it returns QuickView's HTML page with the
     * most visited entries.
     * @param numberEntries how many HistoryFrecencyEntry must be generated
     * @return the complete QuickView HTML page, ready to be displayed
     */
    QByteArray render(int numberEntries);
    /**
     * Right now it is used just for the Unittests. It returns a QHASH
     * of the frecencies of each most visited entry.
     * @param numberEntries how many entries must be considered
     * @return a hash table of URLs and frecencies
     */
    QHash<QString, int> getFrecencies(int numberEntries);

    /**
     * Default number of most visited entries to be considered
     */
    static const int s_numberEntries = 8;
};

#endif // QUICKVIEW_H
