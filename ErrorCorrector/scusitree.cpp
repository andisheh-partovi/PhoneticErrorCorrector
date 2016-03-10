#include "scusitree.h"

//OBSOLETE:Not used in the project anymore

SCUSITree::SCUSITree()
{
    //nodes = new QList<Node*>();
}

void SCUSITree::addNode(QString parents, QString children,
                        QString ID, QString kind, QString fileName, QString expansion,
                        QString ml, QString rank)
{

    Node* newNode = getNodeID(ID);

    //processing the input
    int inputRank = rank.toInt();
    double inputMl = ml.toDouble();
    QStringList inputParents = parents.split(",");
    QStringList inputChildren = children.split(",");

    inputParents.removeOne("");
    inputChildren.removeOne("");

    //if the node to be added is already in the tree, retrieve it and set its variables
    if (newNode)
    {
        newNode->setParameters(inputParents, inputChildren, kind, fileName, expansion, inputMl, inputRank);
    }
    else //if the node to be added is not in the tree, create and add it
    {
        nodes.append(new Node(inputParents, inputChildren, ID, kind, fileName, expansion, inputMl, inputRank));
    }
}

SCUSITree::Node* SCUSITree::getNodeID(QString ID)
{
    foreach (Node* node, nodes) {
        if (node->getID().compare(ID) == 0)
            return node;
    }

    return NULL;
}

void SCUSITree::displayRoots(QString ID)
{
    foreach (Node* node, getRoots(ID)) {
        qDebug() << node->toString();
    }
}

String2StringListMap SCUSITree::getIDMap()
{
    String2StringListMap IDMap;
    QStringList parents;




//    foreach (Node* node, nodes) {

//    }

//    //testing by outputting:
//    QMapIterator <QString, QStringList> Iter(IDMap);
//    while (Iter.hasNext())
//    {
//        Iter.next();
//        qDebug() << Iter.key() << "\t" << Iter.value() << "\n";
//    }

    return IDMap;
}

//returns the ID of the root(s) of a node with the input ID
QStringList SCUSITree::getRootsID (QString ID)
{
    QStringList roots;

    foreach (Node* node, getRoots(ID)) {
        roots.append(node->getID());
    }

    return roots;
}

//returns all the nodes that are the parents of the node which it's given the ID of
QList<SCUSITree::Node*> SCUSITree::getRoots(QString ID)
{
    Node* inputNode = getNodeID(ID);
    QList<Node*> processQueue = getParentsNodes (inputNode);
    QList<Node*> roots;
    Node* currentNode ;

    while (!processQueue.isEmpty())
    {
        currentNode = processQueue.takeFirst();

        if (currentNode->getKind().compare("subinterp_plaintext_c") == 0)
        {
            if (!roots.contains(currentNode))
                roots.append(currentNode);
        }
        else
          processQueue.append(getParentsNodes(currentNode));
    }

    return roots;
}

//find all the parent nodes of an input node
QList<SCUSITree::Node*> SCUSITree::getParentsNodes (Node* inputNode)
{
    QList<Node*> parents;

    QStringList parentsID = inputNode->getParents();

    foreach (QString parent, parentsID)
    {
        parents.append(getNodeID(parent));
    }

    return parents;
}

//QList<SCUSITree::Node*> SCUSITree::getRootsAuxilary(Node* inputNode, QList<Node*> &parents)
//{
//    QStringList parentsID = inputNode->getParents();

//    if (parentsID.size() == 1 && parentsID.contains(""))
//        return parents;

//    foreach (QString parent, parentsID)
//    {
//        parents.append(getNodeID(parent));
//    }

//    foreach (Node* parent, parents)
//    {
//        //remove parent from parents
//        parents.removeOne(parent);
//        parents += getRootsAuxilary(parent, parents);
//    }

//    return parents;
//}

void SCUSITree::displayNodes()
{
    foreach (Node* node, nodes) {
        //qDebug() << node;
        qDebug() << node->toString() << "\n\n";
    }
}

QList<SCUSITree::Node*> SCUSITree::getNodes()
{
    return this->nodes;
}

SCUSITree::~SCUSITree()
{
}

