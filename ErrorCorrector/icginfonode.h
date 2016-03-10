#ifndef ICGINFONODE_H
#define ICGINFONODE_H

#include "definitions.h"

//This class holds the data read from the a ICGinfo file

class ICGInfoNode
{
private:
    int ICGRank;
    QString ICGID;
    QStringList textIDs;
    String2DoubleMap parentUCGMLs;

public:
    ICGInfoNode(int ICGRank, QString ICGID, QStringList textIDs, String2DoubleMap parentUCGMLs);
    ~ICGInfoNode();

    int getICGRank();
    QString getICGID();
    QStringList getTextIDs();
    String2DoubleMap getParentUCGMLs();
    QString toString();
};

#endif // ICGINFONODE_H
