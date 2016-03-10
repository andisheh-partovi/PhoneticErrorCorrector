#ifndef SCUSITREE_H
#define SCUSITREE_H

#include <QDebug>
#include "definitions.h"

//This class holds the data read from subinterps XML
//OBSOLETE:Not used in the project anymore

class SCUSITree
{
public:

    class Node
    {
    private:
        //QList<Node*> parents;
        //QList<Node*> children;

        QStringList parents;
        QStringList children;
        QString ID;
        QString kind;
        QString fileName;
        QString expansion;
        double ml;
        int rank;


    public:

        //------------    Constructors   ------------
        Node(QStringList parents, QStringList children,
             QString ID, QString kind, QString fileName, QString expansion,
             double ml, int rank)
        {
            this->ID = ID;
            setParameters(parents, children, kind, fileName, expansion, ml, rank);
        }

        Node (QString ID)
        {
            this->ID = ID;
        }


        //------------    Getters   ------------
        QString getID()
        {
            return this->ID;
        }
        QString getKind()
        {
            return this->kind;
        }
        QString getFileName()
        {
            return this->fileName;
        }
        QString getExpansion()
        {
            return this->expansion;
        }
        int getRank()
        {
            return this->rank;
        }
        double getMl()
        {
            return this->ml;
        }
        QStringList getChildren()
        {
            return this->children;
        }
        QStringList getParents()
        {
            return this->parents;
        }

        //------------    Setters   ------------
        void setParameters(QStringList parents, QStringList children,
                           QString kind, QString fileName, QString expansion,
                           double ml, int rank)
        {
            this->kind = kind;
            this->fileName = fileName;
            this->expansion = expansion;
            this->ml = ml;
            this->rank = rank;
            this->parents = parents;
            this->children = children;

            //            this->parents = new QStringList();
            //            this->children = new QStringList();

            //            qCopy(parents.begin(), parents.end(), this->parents.begin());
            //            qCopy(children.begin(), children.end(), this->children.begin());
        }

        //<< overload and toString()
        friend QDataStream &operator <<(QDataStream &stream, Node &myclass) {

//            stream<< "Node ID " << myclass.getID() << ":";
//            stream<< "\nChildren: " << myclass.getChildren();
//            stream<< "\nParents: " << myclass.getParents();
//            stream<<  "\nkind: " << myclass.getKind();
//            stream<<  "\nfile name: " << myclass.getFileName();
//            stream<<  "\nexpansion: " << myclass.getExpansion();
//            stream<<  "\nrank: " << myclass.getRank();
//            stream<<  "\nml: " << myclass.getMl();

            return stream;
        }

        QString toString()
        {
            QString stream;

            stream += "Node ID " + this->getID() + ":";
            stream += "\nChildren: ";
            foreach (QString str, this->getChildren()) {
                stream += str + ",";
            }
            stream += "\nParents: ";
            foreach (QString str, this->getParents()) {
                stream += str + ",";
            }
            stream +=  "\nkind: " + this->getKind();
            stream +=  "\nfile name: " + this->getFileName();
            stream +=  "\nexpansion: " + this->getExpansion();
            stream +=  "\nrank: " + QString::number(this->getRank());
            stream +=  "\nml: " + QString::number(this->getMl());

            return stream;
        }

    };

    SCUSITree();
    ~SCUSITree();

    void addNode(QString parents, QString children,
                 QString ID, QString kind, QString fileName, QString expansion,
                 QString ml, QString rank);
    Node* getNodeID(QString ID);
    void displayNodes();
    void displayRoots(QString ID);
    QStringList getRootsID (QString ID);
    String2StringListMap getIDMap();
    QList<Node*> getNodes();

private:

    QList<Node*> nodes;

    QList<Node*> getRoots(QString ID);
    QList<Node*> getRootsAuxilary(Node* inputNode, QList<Node*> &parents);
    QList<Node*> getParentsNodes (Node* inputNode);
};

#endif // SCUSITREE_H
