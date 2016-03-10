#ifndef IOHANDLER_H
#define IOHANDLER_H

#include <QtCore>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QtXml>
#include <QDirIterator>

#include "scusitree.h"
#include "sspoutpputnode.h"
#include "dotfilenode.h"
#include "icginfonode.h"
#include "textinfonode.h"

//This class handles all the file reading and writing of the project

class IOHandler
{
public:
    IOHandler();

    //generic reader/writers
    void readTextFile (QString filePath);
    void write2File (QString filePath, QString text);

    //reading knowledge base files
    SynonymDictionary readSynonymFile (QString filePath);
    StringDictionary readDictionaryFile (QString filePath);
    String2StringMap readImageFile(QString filePath);

    //reading the data files
    DotFileNode* readDotFile(QString filePath);
    QList<ICGInfoNode*> readICGInfoFile(QString filePath);
    QList<TextInfoNode*> readTextInfoFile(QString filePath);

    //OBSOLETE methods:
    DotFileNode* readDotFile2(QString filePath);
    SCUSITree* readSubInterpsXML (QString fileName);
    QList<SSPOutpputNode*> readAW7XML (QString fileName);
    QString readSubinterp_PlaintextXML (QString fileName);
    bool writeSubInterpsXML (SCUSITree*inputTree, QString filePath);

};

#endif // IOHANDLER_H
