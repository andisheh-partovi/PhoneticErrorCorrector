#include "sspoutpputnode.h"

//OBSOLETE:Not used in the project anymore

SSPOutpputNode::SSPOutpputNode(QString utternace,
                               QStringList alternativeIDs, QList<Word *> words)
{
    this->utternace = utternace;
    this->alternativeIDs = alternativeIDs;
    this->words = words;
}

QString SSPOutpputNode::toString()
{
    QString returnStr = "Utterance: " + this->utternace + "\nalternatives:";

    foreach (QString str, this->alternativeIDs) {
        returnStr += str + ",";
    }

    foreach (Word* word, this->words) {
        returnStr += "\n" + word->toString();
    }

    returnStr += "\n------------";

    return returnStr;
}

SSPOutpputNode::~SSPOutpputNode()
{

}

