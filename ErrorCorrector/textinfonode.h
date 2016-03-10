#ifndef TEXTINFONODE_H
#define TEXTINFONODE_H

#include "definitions.h"

//This class holds the data read from the a TextFileInfo file

class TextInfoNode
{
public:

    class NewWord
    {

    public:

        class OriginalWord
        {
        private:

            QString word;
            QString EDLabel;
            double probability;

        public:
            OriginalWord(QString word, QString EDLabel, double probability)
            {
                this->word = word;
                this->EDLabel = EDLabel;
                this->probability = probability;
            }

            QString getWord() { return word;}
            QString getEDLabel() { return EDLabel;}
            double getProbability() { return probability;}

            QString toString() { return word + ":" + EDLabel + ":" + QString::number(probability); }
        };

        NewWord (QString role, QString PoS, QString word)
        {
            this->role = role;
            this->PoS = PoS;
            this->word = word;
        }

        void addOriginalWord(OriginalWord* originalWord)
        {
            originalWords.append(originalWord);
        }

        QString getRole()
        {
            return this->role;
        }

        QString getWord()
        {
            return this->word;
        }

        QList<OriginalWord*> getOriginalWords()
        {
            return this->originalWords;
        }

        QString toString()
        {
            QString returnString;

            returnString = word + "\t" + role + "\t" + PoS;
            foreach (OriginalWord* word, originalWords) {
                returnString += word->toString() + "\t";
            }

            return returnString;
        }

    private:
        QString role;
        QString PoS;
        QString word;
        QList<OriginalWord*> originalWords;
    };


    TextInfoNode(QString textID, QStringList altNumbers, QString sentence);
    ~TextInfoNode();

    QString getTextID();
    QList<NewWord*> getWords();
    QString toString();

    void addNewWord(NewWord* newWord);

private:
    QString textID;
    QStringList altNumbers;
    QString sentence;
    QList<NewWord*> words;
};

#endif // TEXTINFONODE_H
