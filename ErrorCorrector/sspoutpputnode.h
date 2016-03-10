#ifndef SSPOUTPPUTNODE_H
#define SSPOUTPPUTNODE_H

#include "definitions.h"

//OBSOLETE:Not used in the project anymore

class SSPOutpputNode
{
public:

    class Word
    {
    private:
        double probability;
        int start_time;
        QString role;
        QString PoS;
        QString word;

    public:
        Word (double probability, int start_time,QString role, QString PoS, QString word)
        {
            this->probability = probability;
            this->start_time = start_time;
            this->role = role;
            this->PoS = PoS;
            this->word = word;
        }

        QString toString()
        {
            return QString::number(probability) + " " + QString::number(start_time) + " " + role + " " + PoS + " " + word;
        }
    };

    SSPOutpputNode(QString utternace, QStringList alternativeIDs, QList<Word*> words);
    ~SSPOutpputNode();
    QString toString();

private:

    QString utternace;
    QStringList alternativeIDs;
    QList<Word*> words;
};

#endif // SSPOUTPPUTNODE_H
