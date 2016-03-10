#ifndef APPLICATION_H
#define APPLICATION_H


#include "iohandler.h"
#include "editdistance.h"

#include <QtMath>

class Application
{
public:
    Application();
    ~Application();
    bool run(QString currentDescription, int topN, QString outputFilePath, double constantDivider, int option, bool onlyObj);

    void writeFinalLog(QString log, int optionNumber);
    QString getEpsilonLog();
private:

    //internal handles
    IOHandler* ioHandler;
    EditDistance* editDistanceHandle;

    //knowledgebase variables
    SynonymDictionary synonymDictionary;
    SynonymDictionary synonymProbability;
    StringDictionary phoneticDictionary;
    String2StringMap imageMap;

    QStringList excludedSynonyms;

    //log file entry
    QString log;
    QString epsilonLog;

    bool allWordsEpsilon;

    //helper internal methods
    QVector<double> getMinMaxSynonymScores (SynonymDictionary inputDictionary);
    SynonymDictionary createSynonymProbabilities (SynonymDictionary inputDictionary, double min_acceptable_score
                                                  , double max_lex_match, double lex_acceptable_match);
    void displaySynonymDictionary(SynonymDictionary inputDictionary);
    double getEditDistance(QString inputPhrase1, QString inputPhrase2);
    void openDataBaseFiles();
    int getMinNumberOfUCGs(QList<ICGInfoNode*> ICGInfoNodes);
    QStringList getThingRoles(QList<TextInfoNode*> textInfoNodes);
    QStringList getThingRolesUnrobust(QList<TextInfoNode*> textInfoNodes);
    QStringList getEpsilonProbRoles(QMap<int , DotFileNode*> dotFileNodesPerRank);
    QStringList getEpsilonProbRolesComplete(QMap<int , DotFileNode*> dotFileNodesPerRank);
    int getNumberOfUniqueObjects(QMap<int, DotFileNode *> dotFileNodesPerRank, QString role);
    String2DoubleMap findSynonymsOfAWord(QString canonicalWord);
    double getSum(QList<double> inputList);
    double ML2Prob(double ML);
    double Prob2ML(double probability);
    double getMax(QList<double> inputList);
    QList<double> getProbsFromScores(QList<double> inputScores);
    double getMaxPhoneticSize (QString inputPhrase1, QString inputPhrase2);
    QStringList getPhonetic (QString inputPhrase);

};

#endif // APPLICATION_H
