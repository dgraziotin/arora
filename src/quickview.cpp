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

#include "quickview.h"
#include "fileaccesshandler.h"
#include <qapplication.h>
#include <qcryptographichash.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qfileiconprovider.h>
#include <QNetworkRequest>
#include <qhash.h>
#include <qstyle.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qwebsettings.h>
#include <historymanager.h>
#include <QTextStream>

QuickView::QuickView(){}

QList<HistoryEntry> QuickView::getLastHistoryEntries(QList<HistoryEntry> history, int numberEntries){
    if(history.isEmpty())
        return QList<HistoryEntry>();
    if(history.count() < numberEntries)
        numberEntries = history.count();

    int historyLength = history.count();
    int lastSixPosition = historyLength - numberEntries;

    QList<HistoryEntry> lastSix = history.mid(lastSixPosition,-1);
    return lastSix;
}

QString QuickView::getHtmlMessage(QList<HistoryEntry> history){
    if(history.isEmpty())
        return QString();
    QString link = QLatin1String("<p><a href=\"%1\">%2</a></p>");
    for(int i = 0;i < history.length();i++)
        link += link.arg(history.at(i).url, history.at(i).title);
    return link;
}

void QuickView::render(QString htmlMessage){
    // Save result to buffer
    //QBuffer buffer;
    //QTextStream stream(&buffer);
    //stream << htmlMessage;
    //stream.flush();
    //buffer.reset();
    // Publish result
    //setHeader(QNetworkRequest::ContentTypeHeader, QByteArray("text/html"));
    //setHeader(QNetworkRequest::ContentLengthHeader, buffer.bytesAvailable());
    //setAttribute(QNetworkRequest::HttpStatusCodeAttribute, 200);
    //setAttribute(QNetworkRequest::HttpReasonPhraseAttribute, QByteArray("Ok"));
    //emit metaDataChanged();
    //emit downloadProgress(buffer.size(), buffer.size());

    //if (errorCode != QNetworkReply::NoError) {
    //    emit error(errorCode);
    //} else if (buffer.size() > 0) {
    //    emit readyRead();
    //}

    //emit finished();
}
/*
void FileAccessReply::listDirectory()
{
    QDir dir(url().toLocalFile());
    if (!dir.exists()) {
        setError(QNetworkReply::ContentNotFoundError, tr("Error opening: %1: No such file or directory").arg(dir.absolutePath()));
        emit error(QNetworkReply::ContentNotFoundError);
        emit finished();
        return;
    }
    if (!dir.isReadable()) {
        setError(QNetworkReply::ContentAccessDenied, tr("Unable to read %1").arg(dir.absolutePath()));
        emit error(QNetworkReply::ContentAccessDenied);
        emit finished();
        return;
    }

    // Format a html page for the directory contents
    QFile dirlistFile(QLatin1String(":/dirlist.html"));
    if (!dirlistFile.open(QIODevice::ReadOnly))
        return;
    QString html = QLatin1String(dirlistFile.readAll());
    html = html.arg(dir.absolutePath(), tr("Contents of %1").arg(dir.absolutePath()));

    // Templates for the listing
    QString link = QLatin1String("<a class=\"%1\" href=\"%2\">%3</a>");
    QString row = QLatin1String("<tr%1> <td class=\"name\">%2</td> <td class=\"size\">%3</td> <td class=\"modified\">%4</td> </tr>\n");

    QFileIconProvider iconProvider;
    QHash<QString, bool> existingClasses;
    int iconSize = QWebSettings::globalSettings()->fontSize(QWebSettings::DefaultFontSize);
    QFileInfoList list = dir.entryInfoList(QDir::AllEntries | QDir::Hidden, QDir::Name | QDir::DirsFirst);
    QString dirlist, classes;

    // Write link to parent directory first
    if (!dir.isRoot()) {
        QIcon icon = qApp->style()->standardIcon(QStyle::SP_FileDialogToParent);
        classes += cssLinkClass(icon, iconSize).arg(QLatin1String("link_parent"));

        QString addr = QString::fromUtf8(QUrl::fromLocalFile(QFileInfo(dir.absoluteFilePath(QLatin1String(".."))).canonicalFilePath()).toEncoded());
        QString size, modified; // Empty by intention
        dirlist += row.arg(QString()).arg(link.arg(QLatin1String("link_parent")).arg(addr).arg(QLatin1String(".."))).arg(size).arg(modified);
    }

    for (int i = 0; i < list.count(); ++i) {
        // Skip '.' and '..'
        if (list[i].fileName() == QLatin1String(".") || list[i].fileName() == QLatin1String("..")) {
            continue;
        }

        // Fetch file icon and generate a corresponding CSS class if neccessary
        QIcon icon = iconProvider.icon(list[i]);
        QString cssClass = cssLinkClass(icon, iconSize);
        QByteArray cssData = cssClass.toLatin1();
        QString className = QString(QLatin1String("link_%1")).arg(QLatin1String(QCryptographicHash::hash(cssData, QCryptographicHash::Md4).toHex()));
        if (!existingClasses.contains(className)) {
            classes += cssClass.arg(className);
            existingClasses.insert(className, true);
        }

        QString addr = QString::fromUtf8(QUrl::fromLocalFile(list[i].canonicalFilePath()).toEncoded());
        QString size, modified;
        if (list[i].fileName() != QLatin1String("..")) {
            if (list[i].isFile())
                size = tr("%1 KB").arg(QString::number(list[i].size()/1024));
            modified = list[i].lastModified().toString(Qt::SystemLocaleShortDate);
        }

        QString classes;
        if (list[i].isHidden())
            classes = QLatin1String(" class=\"hidden\"");
        dirlist += row.arg(classes).arg(link.arg(className).arg(addr).arg(list[i].fileName())).arg(size).arg(modified);
    }

    html = html.arg(classes).arg(dirlist).arg(tr("Show Hidden Files"));

    // Save result to buffer
    QTextStream stream(&buffer);
    stream << html;
    stream.flush();
    buffer.reset();

    // Publish result
    setHeader(QNetworkRequest::ContentTypeHeader, QByteArray("text/html"));
    setHeader(QNetworkRequest::ContentLengthHeader, buffer.bytesAvailable());
    setAttribute(QNetworkRequest::HttpStatusCodeAttribute, 200);
    setAttribute(QNetworkRequest::HttpReasonPhraseAttribute, QByteArray("Ok"));
    emit metaDataChanged();
    emit downloadProgress(buffer.size(), buffer.size());
    QNetworkReply::NetworkError errorCode = error();
    if (errorCode != QNetworkReply::NoError) {
        emit error(errorCode);
    } else if (buffer.size() > 0) {
        emit readyRead();
    }

    emit finished();
}
*/
