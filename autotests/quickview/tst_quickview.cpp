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

#include <QtTest>
#include "qtest_arora.h"

#include <historymanager.h>
#include <history.h>
#include <historycompleter.h>
#include <quickview/quickview.h>
#include <quickview/quickviewfiltermodel.h>
#include <modeltest.h>
#include <quickview.h>
#include <sstream>
#include <iostream>
#include <QMessageBox>
#include <QAbstractTableModel>
#include <QModelIndex>
#include <limits.h>

using namespace std;

class tst_QuickView : public QObject
{
    Q_OBJECT

public slots:
    /**
     * Called before the beginning of the Unittest.
     */
    void initTestCase();
    /**
     * Called after the end of the Unittest.
     * It restores the user history
     */
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    // BEGIN: tests against real history
    /**
     * Verifies that, given an empty history, different max number entries of QuickView
     * always produce a QList<HistoryFrecencyEntry> of length 0
     */
    void maxNumberEntriesEmptyHistory();

    /**
     * Verifies that, given an non empty history, different max number
     * entries of QuickView always produce a QList<HistoryFrecencyEntry>
     * of consistent length.
     */
    void maxNumberEntriesNonEmptyHistory();

    /**
     * Verifies QuickView::mostVisitedEntriesHTML() about its return value,
     * in all possible cases
     */
    void mostVisitedEntriesHTML();

    /**
     * Verifies QuickView::mostVisitedEntries() checking
     * consistency with QuickViewFilterModel
     */
    void mostVisitedEntriesConsistency();

    /**
     * Verifies QuickView::render() by using dangerous parameter values
     */
    void render();
    // END
    // BEGIN: tests against fake history
    /**
     * Heavily verifies the operator overloading of QuickView::HistoryFrecencyEntry class
     */
    void verifyOperatorFrecencies();
    /**
     * Verifies that the frencencies are correctly computed for having the Top N visited
     * hosts
     */
    void verifyFilterFrecencies();


private:
    QList<HistoryEntry> m_history;
};



// This will be called before the first test function is executed.
// It is only called once. We save the browser history
void tst_QuickView::initTestCase()
{
    HistoryManager* manager = BrowserApplication::historyManager();
    m_history = manager->history();
}


// This will be called after the last test function is executed.
// It is only called once.
void tst_QuickView::cleanupTestCase()
{
    HistoryManager* manager = BrowserApplication::historyManager();
    manager->setHistory(m_history);
}

// This will be called before each test function is executed. We clean browser history
void tst_QuickView::init()
{
    HistoryManager* manager = BrowserApplication::historyManager();
    QList<HistoryEntry> emtpyHistory;
    manager->history().clear();
    manager->setHistory(emtpyHistory);
}

// This will be called after every test function.
void tst_QuickView::cleanup()
{
}

void tst_QuickView::maxNumberEntriesEmptyHistory()
{
    int defaultMaxNumberEntries = QuickView::s_defaultMaxNumberEntries;
    QuickView* quickView = BrowserApplication::quickView(defaultMaxNumberEntries);
    QList<HistoryFrecencyEntry> last;

    last = quickView->mostVisitedEntries();
    QCOMPARE(last.size(), 0);

    quickView = BrowserApplication::quickView(defaultMaxNumberEntries + 1);
    last = quickView->mostVisitedEntries();
    QCOMPARE(last.size(), 0);

    quickView = BrowserApplication::quickView(defaultMaxNumberEntries - 1);
    last = quickView->mostVisitedEntries();
    QCOMPARE(last.size(), 0);

    quickView = BrowserApplication::quickView(0);
    last = quickView->mostVisitedEntries();
    QCOMPARE(last.size(), 0);

    quickView = BrowserApplication::quickView(-1);
    last = quickView->mostVisitedEntries();
    QCOMPARE(last.size(), 0);
}

void tst_QuickView::maxNumberEntriesNonEmptyHistory()
{

    HistoryManager* manager = BrowserApplication::historyManager();
    int defaultMaxNumberEntries = QuickView::s_defaultMaxNumberEntries;
    QuickView* quickView;
    QList<HistoryFrecencyEntry> last;


    // quickview must contain http://twitter.com and http://facebook.com
    // that are two entries.
    manager->addHistoryEntry("http://twitter.com/xyz");
    manager->addHistoryEntry("http://facebook.com/asd");
    manager->addHistoryEntry("http://facebook.com/poi");
    manager->addHistoryEntry("http://twitter.com/xyz");
    manager->addHistoryEntry("http://twitter.com/oki");

    quickView = BrowserApplication::quickView(defaultMaxNumberEntries + 1);
    last = quickView->mostVisitedEntries();
    QCOMPARE(last.size(), 2);

    quickView = BrowserApplication::quickView(defaultMaxNumberEntries - 1);
    last = quickView->mostVisitedEntries();
    QCOMPARE(last.size(), 2);

    quickView = BrowserApplication::quickView(defaultMaxNumberEntries);
    last = quickView->mostVisitedEntries();
    QCOMPARE(last.size(), 2);

    quickView = BrowserApplication::quickView(-1);
    last = quickView->mostVisitedEntries();
    QCOMPARE(last.size(), 2);

    manager->addHistoryEntry("http://google.com");

    quickView = BrowserApplication::quickView();
    last = quickView->mostVisitedEntries();
    QCOMPARE(last.size(), 3);

    quickView = BrowserApplication::quickView(2);
    last = quickView->mostVisitedEntries();
    QCOMPARE(last.size(), 2);

    quickView = BrowserApplication::quickView(1);
    last = quickView->mostVisitedEntries();
    QCOMPARE(last.size(), 1);

    quickView = BrowserApplication::quickView(-1);
    last = quickView->mostVisitedEntries();
    QCOMPARE(last.size(), 3);
}

void tst_QuickView::mostVisitedEntriesHTML()
{
    HistoryManager* manager = BrowserApplication::historyManager();
    QuickView* quickView;

    quickView = BrowserApplication::quickView();
    quickView->calculate();

    QCOMPARE(quickView->mostVisitedEntriesHTML().compare("<p>No recent websites</p>"), 0);

    // quickview must contain http://twitter.com and http://facebook.com
    // that are two entries.
    manager->addHistoryEntry("http://twitter.com/xyz");
    manager->addHistoryEntry("http://facebook.com/asd");
    manager->addHistoryEntry("http://facebook.com/poi");
    manager->addHistoryEntry("http://twitter.com/xyz");
    manager->addHistoryEntry("http://twitter.com/oki");

    quickView->calculate();
    QVERIFY(quickView->mostVisitedEntriesHTML().compare("<p>No recent websites</p>") < 0);

}


void tst_QuickView::mostVisitedEntriesConsistency()
{
    HistoryManager* manager = BrowserApplication::historyManager();
    QuickView* quickView = BrowserApplication::quickView(-1);
    QuickViewFilterModel* model = BrowserApplication::historyManager()->quickViewFilterModel();
    int nLast;
    int rowCount;

    nLast = quickView->mostVisitedEntries().size();
    rowCount = model->rowCount();
    QVERIFY((nLast == 0) && rowCount == 0);

    // quickview and model must contain http://twitter.com and http://facebook.com
    // that are two entries.
    manager->addHistoryEntry("http://twitter.com/xyz");
    manager->addHistoryEntry("http://facebook.com/asd");
    manager->addHistoryEntry("http://facebook.com/poi");
    manager->addHistoryEntry("http://twitter.com/xyz");
    manager->addHistoryEntry("http://twitter.com/oki");

    quickView->calculate();
    nLast = quickView->mostVisitedEntries().size();
    rowCount = model->rowCount();

    QVERIFY((nLast == 2) && rowCount == 2);

    manager->addHistoryEntry("http://google.com");

    nLast = quickView->mostVisitedEntries().size();
    rowCount = model->rowCount();

    // calculate() not yet forced!
    QVERIFY((nLast == 2) && rowCount == 3);

    quickView->calculate();
    nLast = quickView->mostVisitedEntries().size();
    QVERIFY((nLast == 3) && rowCount == 3);
}

void tst_QuickView::render()
{
    QuickView* quickView = BrowserApplication::quickView(0);
    QVERIFY(quickView->render().isEmpty() == false);
    quickView = BrowserApplication::quickView(5);
    QVERIFY(quickView->render().isEmpty() == false);
    quickView = BrowserApplication::quickView(15);
    QVERIFY(quickView->render().isEmpty() == false);
    quickView = BrowserApplication::quickView(-1);
    QVERIFY(quickView->render().isEmpty() == false);
}


void tst_QuickView::verifyOperatorFrecencies()
{    
    HistoryFrecencyEntry twitter(QString("http://twitter.com"), QDateTime(), QString("Twitter"), 100);
    HistoryFrecencyEntry facebook(QString("http://facebook.com"), QDateTime(), QString("Twitter"), 200);
    HistoryFrecencyEntry google(QString("http://google.com"), QDateTime(), QString("Google"), 500);
    HistoryFrecencyEntry yahoo(QString("http://yahoo.com"), QDateTime(), QString("Yahoo"), 200);

    QCOMPARE((twitter < facebook), true);
    QCOMPARE((twitter > facebook), false);
    QCOMPARE((twitter == facebook), false);

    QCOMPARE((facebook == yahoo), true);
    QCOMPARE((facebook > yahoo), false);
    QCOMPARE((google > facebook), true);
}


void tst_QuickView::verifyFilterFrecencies()
{

    HistoryManager* manager = BrowserApplication::historyManager();
    QuickView* quickView = BrowserApplication::quickView(-1);
    QuickViewFilterModel* model = BrowserApplication::historyManager()->quickViewFilterModel();

    QVERIFY(manager->history().count() == 0);
    manager->addHistoryEntry("http://facebook.com/lol");
    manager->addHistoryEntry("http://twitter.com/xyz");
    manager->addHistoryEntry("http://facebook.com/asd");
    manager->addHistoryEntry("http://twitter.com/abc");
    manager->addHistoryEntry("http://twitter.com/def");
    manager->addHistoryEntry("http://twitter.com/def");
    manager->addHistoryEntry("http://twitter.com/qwe");
    manager->addHistoryEntry("http://twitter.com/def");
    manager->addHistoryEntry("http://twitter.com/poi");
    manager->addHistoryEntry("http://facebook.com/ooo");
    QVERIFY(manager->history().count() == 10);
    // at this point, we espect two results from QuickViewFilterModel
    // with the domains twitter.com as most visited and facebook.com as second one
    QVERIFY(model->rowCount() == 2);
    quickView->calculate();
    QList<HistoryFrecencyEntry> mostVisited = quickView->mostVisitedEntries();
    QVERIFY(mostVisited.count() == 2);
    QVERIFY(mostVisited.first().url.compare(QString("http://twitter.com")) == 0);
    QVERIFY(mostVisited.last().url.compare(QString("http://facebook.com")) == 0);

}


QTEST_MAIN(tst_QuickView)
#include "tst_quickview.moc"

