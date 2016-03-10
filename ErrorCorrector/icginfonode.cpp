#include "icginfonode.h"

ICGInfoNode::ICGInfoNode(int ICGRank, QString ICGID, QStringList textIDs, String2DoubleMap parentUCGMLs)
{
    this->ICGRank = ICGRank;
    this->ICGID = ICGID;
    this->textIDs = textIDs;
    this->parentUCGMLs = parentUCGMLs;
}

int ICGInfoNode::getICGRank()
{
    return this->ICGRank;
}

QString ICGInfoNode::getICGID()
{
    return this->ICGID;
}

QStringList ICGInfoNode::getTextIDs()
{
    return this->textIDs;
}

String2DoubleMap ICGInfoNode::getParentUCGMLs()
{
    return this->parentUCGMLs;
}

QString ICGInfoNode::toString()
{
    QString returnString;

    returnString = QString::number(ICGRank) + "\t" + ICGID + "\t";

    foreach (QString ID, textIDs) {
        returnString += ID + ":";
    }
    returnString += "\t";

    //testing image reader:
    QMapIterator <QString, double> Iter(parentUCGMLs);

    while (Iter.hasNext())
    {
        Iter.next();
        returnString +=  Iter.key() + ":" + QString::number(Iter.value()) + "\t";
    }

//    QList<QString> UCGs = parentUCGMLs.keys();
//    QList<double> MLs = parentUCGMLs.values();

//    foreach (QString UCG, UCGs) {
//        returnString += UCG + ":";
//    }
//    returnString += "\t";

//    foreach (double ML, MLs) {
//        returnString += QString::number(ML) + ":";
//    }
//    returnString += "\t";

    return returnString;
}

ICGInfoNode::~ICGInfoNode()
{

}

