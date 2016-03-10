#include "textinfonode.h"

TextInfoNode::TextInfoNode(QString textID, QStringList altNumbers, QString sentence)
{
    this->textID = textID;
    this->altNumbers = altNumbers;
    this->sentence = sentence;
}

void TextInfoNode::addNewWord(NewWord *newWord)
{
    words.append(newWord);
}

QString TextInfoNode::getTextID()
{
    return textID;
}

QList<TextInfoNode::NewWord*> TextInfoNode::getWords()
{
    return this->words;
}

QString TextInfoNode::toString()
{
    QString returnString;

    returnString = "textID=" + textID + "\taltnum=";
    foreach (QString alt, altNumbers) {
        returnString += alt + ":";
    }
    returnString += "\tsen=" + sentence + "\n";
    foreach (NewWord* word, words) {
        returnString += word->toString() + "\n";
    }
    returnString += "\n";

    return returnString;
}


TextInfoNode::~TextInfoNode()
{

}

