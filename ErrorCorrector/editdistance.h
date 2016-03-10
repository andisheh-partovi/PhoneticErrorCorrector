#ifndef EDITDISTANCE_H
#define EDITDISTANCE_H

#include "definitions.h"

class EditDistance
{
public:
    EditDistance();
    ~EditDistance();

    double getDistance(QStringList input1, QStringList input2);

private:

    //Broad Sound Groups (BSGs)
    //Vowels
    QStringList monophthongs;
    QStringList diphthongs;
    QStringList rColoureds;

    QStringList customGroup1; //o
    QStringList customGroup2; //aa
    QStringList customGroup3; //e
    QStringList customGroup4; //a

    // Consonants
    QStringList semivowels;
    QStringList stops;
    QStringList affricates;
    QStringList fricatives;
    QStringList aspirates;
    QStringList liquids;
    QStringList nasals;

    //contains all the BSGs
    QList<QStringList> phonemes;

    double getEditDistance(QStringList word1, QStringList word2);
    double getReplace(QString p1, QString p2);
};

#endif // EDITDISTANCE_H
