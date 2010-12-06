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

#include <QtTest/QtTest>
#include "qtest_arora.h"

#include <historymanager.h>
#include <history.h>
#include <historycompleter.h>
#include <modeltest.h>

class tst_QuickView : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
	void historyLength();



public:
    QList<HistoryEntry> m_history;
};

// Subclass that exposes the protected functions.
class SubHistory : public HistoryManager
{
public:
    SubHistory() : HistoryManager()
    {
        QWidget w;
        setParent(&w);
        if (QWebHistoryInterface::defaultInterface() == this)
            QWebHistoryInterface::setDefaultInterface(0);
        setParent(0);
    }

    ~SubHistory() {
        setDaysToExpire(30);
    }

    void addHistoryEntry(const HistoryEntry &item)
        { HistoryManager::addHistoryEntry(item); }
};

// This will be called before the first test function is executed.
// It is only called once.
void tst_QuickView::initTestCase()
{
    QCoreApplication::setApplicationName("historytest");
    QFile file("myhistory.txt");
    if (!file.open(QFile::ReadOnly)) {
        qWarning() << "couldn't open file:" << file.fileName();
        return;
    }
    QTextStream stream(&file);

    QList<HistoryEntry> list;
    do {
        QString url = stream.readLine();
        QString title = stream.readLine();
        QString date = stream.readLine();
        QDateTime dateTime = QDateTime::fromString(date);
        QVERIFY(dateTime.isValid());
        HistoryEntry item(url, dateTime, title);
        list.prepend(item);
    } while (!stream.atEnd());
    m_history = list;
}


// This will be called after the last test function is executed.
// It is only called once.
void tst_QuickView::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_QuickView::init()
{
}

// This will be called after every test function.
void tst_QuickView::cleanup()
{
}

// At this point, our history contains six elements
void tst_QuickView::historyLength()
{
	QCOMPARE(m_history.count(),6);
}


QTEST_MAIN(tst_QuickView)
#include "tst_quickview.moc"

