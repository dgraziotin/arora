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
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
        // BEGIN: tests against real history
	void getMostVisited();
        void getLastHistoryEntries();
        void getHtmlMessage();
        void render();
        // END
        // BEGIN: tests against fake history
        void verifyFilterCount();
        void verifyOperatorFrecencies();
        void verifyFilterFrecencies();
	

public:
    QList<HistoryEntry> m_history;
};



// This will be called before the first test function is executed.
// It is only called once.
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

// This will be called before each test function is executed.
void tst_QuickView::init()
{

}

// This will be called after every test function.
void tst_QuickView::cleanup()
{
}

void tst_QuickView::getMostVisited()
{
	QuickView quickView;
	int desiredLastPages = 6;
        QList<HistoryFrecencyEntry> last = quickView.getLastHistoryEntries(desiredLastPages);
	QVERIFY(last.size() <= desiredLastPages);
}

void tst_QuickView::getLastHistoryEntries(){
    QuickView quickView;
    int desiredLastPages = -1;
    QList<HistoryFrecencyEntry> last = quickView.getLastHistoryEntries(desiredLastPages);
    QVERIFY(last.size() == 0);

    QuickViewFilterModel* model = BrowserApplication::historyManager()->quickViewFilterModel();
    int rowCount = model->rowCount();

    desiredLastPages = numeric_limits<int>::max();
    last = quickView.getLastHistoryEntries(desiredLastPages);

    if (rowCount == 0)
        QVERIFY(last.size() == 0);
    else if (rowCount > 0 && rowCount < 7)
        QVERIFY(last.size() == rowCount);
    else
        QVERIFY(last.size() > 0);

}

void tst_QuickView::getHtmlMessage(){
    QuickView quickView;
    int desiredLastPages = -1;
    QList<HistoryFrecencyEntry> last = quickView.getLastHistoryEntries(desiredLastPages);
    if (last.size() == 0)
        QVERIFY(quickView.getHtmlMessage(last).compare("<p>No recent websites</p>") == 0);
    else
        QVERIFY(quickView.getHtmlMessage(last).compare("<p>No recent websites</p>") > 0);

    QVERIFY(quickView.getHtmlMessage(last).isEmpty() == false);
    QVERIFY(quickView.getHtmlMessage(last).isNull() == false);
}

void tst_QuickView::render(){
    QuickView quickView;
    QVERIFY(quickView.render(0).isEmpty() == false);
    QVERIFY(quickView.render(5).isEmpty() == false);
    QVERIFY(quickView.render(15).isEmpty() == false);
    QVERIFY(quickView.render(-1).isEmpty() == false);
}

void tst_QuickView::verifyFilterCount(){

    HistoryManager* manager = BrowserApplication::historyManager();
    QuickViewFilterModel* model = manager->quickViewFilterModel();
    QuickView quickView;

    QList<HistoryEntry> emtpyHistory;

    manager->history().clear();
    manager->setHistory(emtpyHistory);

    QVERIFY(manager->history().count() == 0);
    manager->addHistoryEntry("http://twitter.com/xyz");
    manager->addHistoryEntry("http://facebook.com/asd");
    manager->addHistoryEntry("http://twitter.com/abc");
    QVERIFY(manager->history().count() == 3);
    // at this point, we espect two results from QuickViewFilterModel
    // with the domains twitter.com and facebook.com
    QCOMPARE(model->rowCount(), 2);
    QList<HistoryFrecencyEntry> mostVisited = quickView.getLastHistoryEntries(6);
    QCOMPARE(mostVisited.count(), 2);

    manager->history().clear();
    manager->setHistory(emtpyHistory);
}

void tst_QuickView::verifyOperatorFrecencies(){
    QList<HistoryEntry> emtpyHistory;
    HistoryManager* manager = BrowserApplication::historyManager();
    manager->history().clear();
    manager->setHistory(emtpyHistory);

    HistoryFrecencyEntry twitter(QString("http://twitter.com"),QDateTime(),QString("Twitter"),100);
    HistoryFrecencyEntry facebook(QString("http://facebook.com"),QDateTime(),QString("Twitter"),200);
    HistoryFrecencyEntry google(QString("http://google.com"),QDateTime(),QString("Google"),500);
    HistoryFrecencyEntry yahoo(QString("http://yahoo.com"),QDateTime(),QString("Yahoo"),200);

    QCOMPARE((twitter < facebook), true);
    QCOMPARE((twitter > facebook), false);
    QCOMPARE((twitter == facebook), false);

    QCOMPARE((facebook == yahoo), true);
    QCOMPARE((facebook > yahoo), false);
    QCOMPARE((google > facebook), true);

    manager->history().clear();
    manager->setHistory(emtpyHistory);
}

void tst_QuickView::verifyFilterFrecencies(){

    HistoryManager* manager = BrowserApplication::historyManager();
    QuickViewFilterModel* model = manager->quickViewFilterModel();
    QuickView quickView;

    QList<HistoryEntry> emtpyHistory;

    manager->history().clear();
    manager->setHistory(emtpyHistory);

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
    QList<HistoryFrecencyEntry> mostVisited = quickView.getLastHistoryEntries(6);
    QVERIFY(mostVisited.count() == 2);
    cout << "most visited:" << mostVisited.first().url.toStdString() << ": " << mostVisited.first().frecency << endl;
    cout << "less visited:" << mostVisited.last().url.toStdString() << ": " << mostVisited.last().frecency << endl;
    QVERIFY(mostVisited.first().url.compare(QString("http://twitter.com")) == 0);

    QVERIFY(mostVisited.last().url.compare(QString("http://facebook.com")) == 0);

    manager->history().clear();
    manager->setHistory(emtpyHistory);
}

QTEST_MAIN(tst_QuickView)
#include "tst_quickview.moc"

