#ifndef AUXILIARYORIGINALWORDCONTAINER_H
#define AUXILIARYORIGINALWORDCONTAINER_H

#include "textinfonode.h"

//used in the claculations to temperarily hold the original words

class AuxiliaryOriginalWordContainer
{
public:
    AuxiliaryOriginalWordContainer(QString word
                                   , QList<TextInfoNode::NewWord::OriginalWord* > heardWords
                                   , bool isThing)
    {
        this->heardWords = heardWords;
        this->isThing = isThing;
        this->word = word;
    }

    QList<TextInfoNode::NewWord::OriginalWord* > getHeardWords()
    {
        return this->heardWords;
    }

    bool getIsThing()
    {
        return this->isThing;
    }

    bool getIsLexEpsilon()
    {
        return this->isLexEpsilon;
    }

    double getCurrentMultiply()
    {
        return this->currentMultiply;
    }

    double getPr_I_given_U_prime()
    {
        return this->Pr_I_given_U_prime;
    }

    double getPr_I_given_U()
    {
        return this->Pr_I_given_U;
    }

    QString getWord()
    {
        return this->word;
    }

    void setCurrentMultiply(double currentMultiply)
    {
        this->currentMultiply = currentMultiply;
    }

    void setPr_I_given_U_prime(double Pr_I_given_U_prime)
    {
        this->Pr_I_given_U_prime = Pr_I_given_U_prime;
    }

    void setPr_I_given_U(double Pr_I_given_U)
    {
        this->Pr_I_given_U = Pr_I_given_U;
    }

    void setIsLexEpsilon(bool isLexEpsilon)
    {
        this->isLexEpsilon = isLexEpsilon;
    }

    void setWord(QString word)
    {
        this->word = word;
    }

    ~AuxiliaryOriginalWordContainer();

private:
    QList<TextInfoNode::NewWord::OriginalWord* > heardWords;
    bool isThing;
    bool isLexEpsilon;
    QString word;
    double currentMultiply;
    double Pr_I_given_U_prime;
    double Pr_I_given_U;
};

#endif // AUXILIARYORIGINALWORDCONTAINER_H
