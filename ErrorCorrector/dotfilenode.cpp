#include "dotfilenode.h"

DotFileNode::DotFileNode(double ml)
{
    this->ml = ml;
}

//default constructor
DotFileNode::DotFileNode()
{
}

//adds an item to the node
void DotFileNode::addItem(QString actualWord, QString assignedWord, QString role,
                          double Prob, double PR_lex, double PR_color,
                          double PR_size, double PR_unk)
{
    items.append(new DotFileItem (actualWord, assignedWord, role, Prob, PR_lex, PR_color, PR_size, PR_unk));
}

void DotFileNode::setML (double ml)
{
    this->ml = ml;
}

void DotFileNode::setWeight (double weight)
{
    this->weight = weight;
}

void DotFileNode::addLocationProbability (double locationProbability)
{
    this->locationProbability.append(locationProbability);
}

QList<DotFileNode::DotFileItem*> DotFileNode::getItems()
{
    return this->items;
}

//gets the multiplication of all the probabilites for each item and the location probability
//it is used to recalculate probabilities, once the Pr_lex is updated
double DotFileNode::getAllProbsMultiplied()
{
    double mult = 1;
    foreach (double prob, this->locationProbability) {
        if (prob != 0)
            mult *= prob;
    }


    foreach (DotFileItem* item, this->items) {
        mult *= item->getAllProbsMultiplied();
    }

    return mult;
}

int DotFileNode::getWeight()
{
    return this->weight;
}

QString DotFileNode::toString()
{
    QString returnValue = "\n";

    returnValue += "items size: " + QString::number(items.size()) + "\n";

    foreach (DotFileItem* item, items) {
        returnValue += item->toString() + "\n";
    }

    returnValue +=  "\nlocationProbabilites:";
    foreach (double prob, locationProbability) {
        returnValue += QString::number(prob)+ "\t";
    }


    return returnValue + "\nML:" + QString::number(ml)+ "\tweight:" + QString::number(weight);
}

DotFileNode::~DotFileNode()
{

}

