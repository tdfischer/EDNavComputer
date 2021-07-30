#include "journalloader.h"

#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QDebug>
#include <QDirIterator>

#include <shlobj.h>

JournalLoader::JournalLoader(QObject *parent) : QObject(parent)
{
}

void
JournalLoader::load()
{
    PWSTR savesPath = NULL;
    HRESULT r;
    r = SHGetKnownFolderPath(FOLDERID_SavedGames, KF_FLAG_CREATE, NULL, &savesPath);

    if (savesPath == NULL) {
        qDebug() << "Couldn't locate saved games directory!";
        return;
    }

    QString savedGamesPath(QString::fromWCharArray(savesPath));
    QString journalPath(savedGamesPath + "\\Frontier Developments\\Elite Dangerous\\");

    qDebug() << "Loading journals from" << journalPath;
    QDirIterator it(journalPath);
    while (it.hasNext()) {
        QFile journalFile(it.next());
        qDebug() << "Processing journal file" << journalFile.fileName();

        if (!journalFile.fileName().endsWith(".log")) {
            continue;
        }
        journalFile.open(QIODevice::ReadOnly | QIODevice::Text);

        while(!journalFile.atEnd()) {
            JournalEntry entry = JournalEntry(QJsonDocument::fromJson(journalFile.readLine()).object());
            emit newEvent(entry);
        }
    }

    emit finished();
}
