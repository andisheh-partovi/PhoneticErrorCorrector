#ifndef DOTFILENODE_H
#define DOTFILENODE_H

#include "definitions.h"

//This class holds the data read from the a dot file

class DotFileNode
{

public:

    class DotFileItem
    {

    public:
        DotFileItem(QString actualWord, QString assignedWord, QString role,
                    double Prob, double PR_lex, double PR_color,
                    double PR_size, double PR_unk)
        {
            this->actualWord = actualWord;
            this->assignedWord = assignedWord;
            this->role = role;
            this->Prob = Prob;
            this->PR_lex = PR_lex;
            this->PR_color = PR_color;
            this->PR_size = PR_size;
            this->PR_unk = PR_unk;
        }

        QString getAssignedWord()
        {
            return assignedWord;
        }

        QString getRole()
        {
            return role;
        }

        double getPR_lex()
        {
            return PR_lex;
        }

        double getAllProbsMultiplied()
        {
            double mult = 1;

            if (PR_lex != 0) mult *= PR_lex;
            if (PR_color != 0) mult *= PR_color;
            if (PR_size != 0) mult *= PR_size;
            if (PR_unk != 0) mult *= PR_unk;

            return mult;
        }

        QString toString()
        {
            QString returnValue = "\n";

            returnValue += actualWord + "/" + assignedWord + "/" + role + "/" +
                    QString::number(Prob) + "/" + QString::number(PR_lex) + "/" +
                    QString::number(PR_color) + "/" + QString::number(PR_size) + "/" +
                    QString::number(PR_unk);

            return returnValue;
        }

    private:
        QString actualWord;
        QString assignedWord;
        QString role;
        double Prob;
        double PR_lex;
        double PR_color;
        double PR_size;
        double PR_unk;
    };// end of DotFileItem class

    DotFileNode(double ml);
    DotFileNode();
    ~DotFileNode();

    void setML (double ml);
    void setWeight (double weight);
    void addLocationProbability (double locationProbability);
    void addItem(QString actualWord, QString assignedWord, QString role,
                 double Prob, double PR_lex, double PR_color,
                 double PR_size, double PR_unk);

    QList<DotFileItem*> getItems();
    int getWeight();
    double getAllProbsMultiplied();
    QString toString();

private:

    QList<DotFileItem*> items;
    double ml;
    QList<double> locationProbability;
    int weight;
};

#endif // DOTFILENODE_H
