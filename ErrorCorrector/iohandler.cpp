#include "iohandler.h"

IOHandler::IOHandler()
{
}

//reading lexical.db synonym file
SynonymDictionary IOHandler::readSynonymFile(QString filePath)
{
    QFile file (filePath);
    QString eachLine;

    SynonymDictionary synonymsList;	//dynamic array holding word synonyms in form of maps (synonym, probability)
    String2DoubleMap eachWord;
    bool startingNew;

    QString firstWord;
    QString synonym;
    QString score;
    QStringList splitted;


    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "ERROR: cannot open " << filePath << " file for reading";
        return synonymsList;
    }

    QTextStream in (&file);

    while(!in.atEnd())
    {
        eachLine = in.readLine();

        if (eachLine.compare("<item>") == 0)
            startingNew = true;
        else if (eachLine.compare("") == 0)	//an empty line
            ;//pass
        else if (eachLine.compare("</item>") == 0)
        {
            synonymsList.append(eachWord);
            eachWord.clear();
        }
        else	//a word line. Add a new element to the map
        {
            //first split the line
            splitted = eachLine.split(" ");
            firstWord = splitted[0];
            synonym = splitted[1];
            score = splitted[2];

            //qDebug() << firstWord << " " << synonym << " " << score << "\n";

            if (startingNew)	//create the first element of the map which is only the original word
            {
                eachWord.insert(firstWord, -1);
                startingNew = false;
            }

            eachWord.insert(synonym, score.toDouble());
        }
    } //end of reading each line


//	//testing by outputting:
//	qDebug() << "list size" << synonymsList.size();

//    foreach (String2DoubleMap eachWord, synonymsList)
//	{
//        QMapIterator <QString, double> Iter(eachWord);

//		while (Iter.hasNext())
//		{
//			Iter.next();
//			qDebug() << Iter.key() << "\t" << Iter.value() << "\n";
//		}

//		qDebug() << "\n\n" ;
//    }



    file.close();

    return synonymsList;
}

//reading the CMU_Phonetic dictionary file
StringDictionary IOHandler::readDictionaryFile (QString filePath)
{
    QFile file (filePath);
    QString eachLine;

    StringDictionary dictionary;
    QStringList splitted;
    QStringList phonetics;

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "cannot open file for reading";
        return dictionary;
    }

    QTextStream in (&file);

    while(!in.atEnd())
    {
        phonetics.clear();

        eachLine = in.readLine();
        //qDebug() << eachLine;

        //first split the line
        splitted = eachLine.split(" ");

        foreach (QString phoneme, splitted)
            phonetics.append(phoneme);

        //removing the first element as it's the word itself
        phonetics.pop_front();

        dictionary.insert(splitted[0], phonetics);

    } //end of reading each line


    //testing by outputting:
    //testing size:
//	qDebug() << "list size" << dictionary.size();

//	QHashIterator < QString, QStringList> Iter(dictionary);

//     while (Iter.hasNext())
//     {
//         Iter.next();
//         qDebug() << Iter.key() << "\t" << Iter.value() << "\n";
//     }

    file.close();

    return dictionary;
}

//generic text file reader
void IOHandler::readTextFile (QString filePath)
{
    QFile file (filePath);
    QString fileContent;


    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "cannot open file for reading";
        return;
    }

    QTextStream in (&file);

    fileContent = in.readAll(); //reading the entire file

    qDebug() << fileContent;

    file.close();
}

//generic text file writer
void IOHandler::write2File (QString filePath, QString text)
{
    QFile file (filePath);

    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug() << "cannot open " << filePath << " for writting";
        return;
    }

    QTextStream out (&file);

    out << text;

    file.flush();
    file.close();
}

//reading subinterms XML file
SCUSITree* IOHandler::readSubInterpsXML (QString fileName)
{
    SCUSITree* tree = new SCUSITree();

    QDomDocument document;  //the document that will store the XML file

    //Load file:
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "ERROR: Failed to open " << fileName << " file for reading";
    }
    else
    {
        //qt automatically loads the entire document
        if (!document.setContent(&file))
            qDebug() << "ERROR: Failed to load the document in readSSPOutputXML";

        file.close();
    }

    //make the root element
    QDomElement root = document.firstChildElement();

    QDomNodeList items = root.elementsByTagName("subinterp_node");//gets all the elements that has the tagName
    //qDebug() << "Total items # " << items.count();

    //assume that everything is a node first, then check if they are actually elements or not
    for (int i = 0 ; i < items.count() ; ++i)
    {
        QDomNode itemNode = items.at(i);

        //convert to element if it is an element:
        if (itemNode.isElement())
        {
            QDomElement itemelement = itemNode.toElement();

            tree->addNode(itemelement.attribute("parents"), itemelement.attribute("children"),
                         itemelement.attribute("id"), itemelement.attribute("kind"), itemelement.attribute("filename"),
                         itemelement.attribute("expansion"), itemelement.attribute("ml"), itemelement.attribute("rank"));

            //testing:
//            qDebug() << "\nparents: " << itemelement.attribute("parents");
//            qDebug() << "\nchildren: " << itemelement.attribute("children");
//            qDebug() << "\nID: " << itemelement.attribute("id");
//            qDebug() << "\nkind: " << itemelement.attribute("kind");
//            qDebug() << "\nfileName: " << itemelement.attribute("filename");
//            qDebug() << "\nexpansion: " << itemelement.attribute("expansion");
//            qDebug() << "\nml: " << itemelement.attribute("ml");
//            qDebug() << "\nrank: " << itemelement.attribute("rank") << "\n\n";
        }
    }

    return tree;
}

//reading AW7 XML file
QList<SSPOutpputNode*> IOHandler::readAW7XML (QString fileName)
{
    QString num;
    QString utternace;
    QStringList numSplitted;
    QList<SSPOutpputNode*> returnList;
    double probability;
    int start_time;
    QString role;
    QString PoS;
    QString word;
    QList<SSPOutpputNode::Word*> wordsInUtterance;

    QDomDocument document;  //the document that will store the XML file

    //Load file:
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "ERROR: Failed to open " << fileName << " file for reading";
    }
    else
    {
        //qt automatically loads the entire document
        if (!document.setContent(&file))
            qDebug() << "ERROR: Failed to load the document in readAW7XML";

        file.close();
    }

    //make the root element
    QDomElement root = document.firstChildElement();

    QDomNodeList alternatives = root.elementsByTagName("alt");//gets all the elements that has the alt tagName

    //assume that everything is a node first, then check if they are actually elements or not
    for (int i = 0 ; i < alternatives.count() ; ++i)
    {
        QDomNode alternativesNode = alternatives.at(i);

        //convert to element if it is an element:
        if (alternativesNode.isElement())
        {
            QDomElement alternativeElement = alternativesNode.toElement();

            num = alternativeElement.attribute("num");

            QDomNodeList words = alternativeElement.elementsByTagName("word");//gets all the elements that has the alt tagName

            for (int j = 0 ; j < words.count() ; ++j)
            {
                QDomNode wordsNode = words.at(j);

                //convert to element if it is an element:
                if (wordsNode.isElement())
                {
                    QDomElement wordElement = wordsNode.toElement();

                    utternace += wordElement.text() + " ";

                    probability = wordElement.attribute("prob").toDouble();
                    start_time = wordElement.attribute("start-time").toInt();
                    role = wordElement.attribute("role");
                    PoS = wordElement.attribute("PoS");
                    word = wordElement.text();
                }

                wordsInUtterance.append( new SSPOutpputNode::Word (probability, start_time, role, PoS, word) );
            }
        }

        //process the data, create the SSPOutput object, and add it to the list:
        //preprocessing num:
        num.remove('*');
        numSplitted = num.split(":");

        //adding to the list:
        returnList.append(new SSPOutpputNode(utternace, numSplitted, wordsInUtterance));

        //post processing:
        utternace ="";
        wordsInUtterance.clear();
    }

    return returnList;
}

//reading plain text XML file
QString IOHandler::readSubinterp_PlaintextXML (QString fileName)
{
    QString utterance;

    QDomDocument document;  //the document that will store the XML file

    //Load file:
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "ERROR: Failed to open " << fileName << " file for reading";
    }
    else
    {
        //qt automatically loads the entire document
        if (!document.setContent(&file))
            qDebug() << "ERROR: Failed to load the document in readSubinterp_PlaintextXML";

        file.close();
    }

    //make the root element
    QDomElement root = document.firstChildElement();

    QDomNodeList items = root.elementsByTagName("SpeechRecognition");//gets all the elements that has the SpeechRecognition

    //assume that everything is a node first, then check if they are actually elements or not
    for (int i = 0 ; i < items.count() ; ++i)
    {
        QDomNode itemNode = items.at(i);

        //convert to element if it is an element:
        if (itemNode.isElement())
        {
            QDomElement itemelement = itemNode.toElement();
            utterance = itemelement.attribute("text");
        }
    }

    return utterance;
}

//reading dot file
DotFileNode* IOHandler::readDotFile(QString filePath)
{
    QFile file (filePath);
    QString eachLine;
    QString targetLine;
    QStringList splitted;
    QStringList targetLineSplitted;

    QString item;
    QStringList itemSplitted;
    QStringList ProbItem;
    QStringList lexItem;
    QStringList colorItem;
    QStringList sizeItem;
    QStringList unkItem;
    QStringList mlSplitted;

    QString actualWord;
    QString assignedWord;
    QString role = "O";
    double Prob;
    double PR_lex;
    double PR_color;
    double PR_size;
    double PR_unk;

    double ml;
    double locationProbability;
    int weight = 0;

    DotFileNode* returnNode = new DotFileNode();

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "ERROR: Failed to open " << filePath << " for reading";
        return NULL;
    }

    QTextStream in (&file);

    while(!in.atEnd())
    {
        eachLine = in.readLine();
        //qDebug() << eachLine;

        //first split the line
        splitted = eachLine.split(" ");

        if (eachLine.contains("Graph_Title_Description"))
        {
            targetLine = splitted.join(" "); //putting the splitted string back together

            targetLineSplitted = targetLine.split("{"); //now split on {

            for (int i = 1 ; i < targetLineSplitted.size() ; ++i)
            {
                item = targetLineSplitted[i];
                itemSplitted = item.split(" ");

                if (itemSplitted.contains("->"))    //handling item nodes
                {
                    actualWord = itemSplitted[0];
                    assignedWord = itemSplitted[2].remove(",");

                    ProbItem = itemSplitted[5].split("\\");
                    Prob = ProbItem[0].toDouble();

                    lexItem = itemSplitted[7].split("\\");
                    PR_lex = lexItem[0].toDouble();
                    if (PR_lex != 0) weight++;

                    colorItem = itemSplitted[9].split("\\");
                    PR_color = colorItem[0].toDouble();
                    if (PR_color != 0) weight++;

                    sizeItem = itemSplitted[11].split("\\");
                    PR_size = sizeItem[0].toDouble();
                    if (PR_size != 0) weight++;

                    unkItem = itemSplitted[13].split("\\");
                    PR_unk = unkItem[0].remove("}").toDouble();
                    if (PR_unk != 0) weight++;

                    //setting the role:
                    if (i == 1)
                        role = "O";
                    else if (i % 2 == 0)
                        role = "P";
                    else
                        role = "L";

                    //testing:
        //                qDebug() << "actualWord: " << actualWord << "\n";
        //                qDebug() << "assignedWord: " << assignedWord << "\n";
        //                qDebug() << "Prob: " << Prob << "\n";
        //                qDebug() << "PR_lex: " << PR_lex << "\n";
        //                qDebug() << "PR_color: " << PR_color << "\n";
        //                qDebug() << "PR_size: " << PR_size << "\n";
        //                qDebug() << "PR_unk: " << PR_unk << "\n";

                    returnNode->addItem ( actualWord, assignedWord, role, Prob, PR_lex, PR_color, PR_size, PR_unk);

                }//handling item nodes

                else    //handling P:
                {
                    //setting the location probability
                    returnNode->addLocationProbability(itemSplitted[1].split("}").at(0).toDouble());

                    weight++;
                }
            }


            //item = targetLineSplitted[targetLineSplitted.size() - 1];
            //itemSplitted = item.split(" ");

            //handling the last entry and getting ML:
            //two cases:
            if (itemSplitted.contains("->"))
            {
                mlSplitted = itemSplitted[15].split("\\");
                ml = mlSplitted[0].toDouble();

                //adding one weight for location
                if(targetLineSplitted.size() <= 2)
                {
                    returnNode->addLocationProbability(0);
                }
                else
                {
                    //setting the location probability also based on the last entrt:
                    returnNode->addLocationProbability(EPSILON);
                    weight++;
                }
            }
            else
            {
                mlSplitted = itemSplitted[3].split("\\");
                ml = mlSplitted[0].toDouble();
            }

            //setting values:
            returnNode->setML(ml);
            returnNode->setWeight(weight);
        }
     }//end of reading each line

    file.close();

    return returnNode;
}

//reading dot file
//old version (OBSOLETE)
DotFileNode* IOHandler::readDotFile2(QString filePath)
{
    QFile file (filePath);
    QString eachLine;
    QString targetLine;
    QStringList splitted;
    QStringList targetLineSplitted;

    QString item;
    QStringList itemSplitted;
    QStringList ProbItem;
    QStringList lexItem;
    QStringList colorItem;
    QStringList sizeItem;
    QStringList unkItem;
    QStringList mlSplitted;

    QString actualWord;
    QString assignedWord;
    QString role = "O";
    double Prob;
    double PR_lex;
    double PR_color;
    double PR_size;
    double PR_unk;

    double ml;
    double locationProbability;
    int weight = 0;

    DotFileNode* returnNode = new DotFileNode();

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "ERROR: Failed to open " << filePath << " for reading";
        return NULL;
    }

    QTextStream in (&file);

    while(!in.atEnd())
    {
        eachLine = in.readLine();
        //qDebug() << eachLine;

        //first split the line
        splitted = eachLine.split(" ");

        if (eachLine.contains("Graph_Title_Description"))
        {
            //it is hacky the way it's broken down to having or not having "P:"
            //but there was no other way given the time
            if(eachLine.contains("P:"))
            {
                targetLine = splitted.join(" "); //putting the splitted string back together

                targetLineSplitted = targetLine.split("{"); //now split on {

                for (int i = 1 ; i < targetLineSplitted.size() - 1 ; ++i)
                {
                    item = targetLineSplitted[i];

                    itemSplitted = item.split(" ");

                    actualWord = itemSplitted[0];
                    assignedWord = itemSplitted[2].remove(",");

                    ProbItem = itemSplitted[5].split("\\");
                    Prob = ProbItem[0].toDouble();

                    lexItem = itemSplitted[7].split("\\");
                    PR_lex = lexItem[0].toDouble();
                    if (PR_lex != 0) weight++;

                    colorItem = itemSplitted[9].split("\\");
                    PR_color = colorItem[0].toDouble();
                    if (PR_color != 0) weight++;

                    sizeItem = itemSplitted[11].split("\\");
                    PR_size = sizeItem[0].toDouble();
                    if (PR_size != 0) weight++;

                    unkItem = itemSplitted[13].split("\\");
                    PR_unk = unkItem[0].remove("}").toDouble();
                    if (PR_unk != 0) weight++;

                    //setting the role:
                    switch (i)
                    {
                    case 1:
                        role = "O";
                        break;
                    case 2:
                        role = "P";
                        break;
                    case 3:
                        role = "L";
                        break;
                    case 4:
                        role = "P";
                        break;
                    case 5:
                        role = "L";
                        break;
                    case 6:
                        role = "P";
                        break;
                    case 7:
                        role = "L";
                        break;
                    default:
                        qDebug() << "ERROR: Unknown case in the first switch in dotFileReader";
                    }

                    //testing:
    //                qDebug() << "actualWord: " << actualWord << "\n";
    //                qDebug() << "assignedWord: " << assignedWord << "\n";
    //                qDebug() << "Prob: " << Prob << "\n";
    //                qDebug() << "PR_lex: " << PR_lex << "\n";
    //                qDebug() << "PR_color: " << PR_color << "\n";
    //                qDebug() << "PR_size: " << PR_size << "\n";
    //                qDebug() << "PR_unk: " << PR_unk << "\n";

                    returnNode->addItem ( actualWord, assignedWord, role, Prob, PR_lex, PR_color, PR_size, PR_unk);
                }

                weight++;//this case requires 1 additional weight for location_prob

                //handling the last entry and getting ML:
                item = targetLineSplitted[targetLineSplitted.size() - 1];
                itemSplitted = item.split(" ");
                mlSplitted = itemSplitted[3].split("\\");
                ml = mlSplitted[0].toDouble();

                //setting the location probability also based on the last entrt:
                locationProbability = itemSplitted[1].split("}").at(0).toDouble();

                returnNode->setML(ml);
                //returnNode->setLocationProbability(locationProbability);
            }
            else
            {
                targetLine = splitted.join(" "); //putting the splitted string back together

                targetLineSplitted = targetLine.split("{"); //now split on {

                for (int i = 1 ; i < targetLineSplitted.size() ; ++i)
                {
                    item = targetLineSplitted[i];

                    itemSplitted = item.split(" ");

                    actualWord = itemSplitted[0];
                    assignedWord = itemSplitted[2].remove(",");

                    ProbItem = itemSplitted[5].split("\\");
                    Prob = ProbItem[0].toDouble();

                    lexItem = itemSplitted[7].split("\\");
                    PR_lex = lexItem[0].toDouble();
                    if (PR_lex != 0) weight++;

                    colorItem = itemSplitted[9].split("\\");
                    PR_color = colorItem[0].toDouble();
                    if (PR_color != 0) weight++;

                    sizeItem = itemSplitted[11].split("\\");
                    PR_size = sizeItem[0].toDouble();
                    if (PR_size != 0) weight++;

                    unkItem = itemSplitted[13].split("\\");
                    PR_unk = unkItem[0].remove("}").toDouble();
                    if (PR_unk != 0) weight++;

                    switch (i)
                    {
                    case 1:
                        role = "O";
                        break;
                    case 2:
                        role = "P";
                        break;
                    case 3:
                        role = "L";
                        break;
                    case 4:
                        role = "P";
                        break;
                    case 5:
                        role = "L";
                        break;
                    case 6:
                        role = "P";
                        break;
                    case 7:
                        role = "L";
                        break;
                    default:
                        qDebug() << "ERROR: Unknown case in the first switch in dotFileReader";
                    }

                    //testing:
    //                qDebug() << "actualWord: " << actualWord << "\n";
    //                qDebug() << "assignedWord: " << assignedWord << "\n";
    //                qDebug() << "Prob: " << Prob << "\n";
    //                qDebug() << "PR_lex: " << PR_lex << "\n";
    //                qDebug() << "PR_color: " << PR_color << "\n";
    //                qDebug() << "PR_size: " << PR_size << "\n";
    //                qDebug() << "PR_unk: " << PR_unk << "\n";

                    returnNode->addItem ( actualWord, assignedWord, role, Prob, PR_lex, PR_color, PR_size, PR_unk);
                }

                //adding one weight for location
                if(targetLineSplitted.size() <= 2)
                {
                    locationProbability = 0;
                }
                else
                {
                    //setting the location probability also based on the last entrt:
                    locationProbability = EPSILON;
                    weight++;
                }

                //handling the last entry and getting ML:
                mlSplitted = itemSplitted[15].split("\\");
                ml = mlSplitted[0].toDouble();

                returnNode->setML(ml);
                //returnNode->setLocationProbability(locationProbability);
            }

            returnNode->setWeight(weight);
        }
     } //end of reading each line

    file.close();

    return returnNode;
}

//reading in the image.kb files
String2StringMap IOHandler::readImageFile(QString filePath)
{
    QFile file (filePath);
    QString eachLine;

    String2StringMap returnMap;

    QStringList splitted1;
    QStringList splitted2;

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "ERROR: cannot open " << filePath << " file for reading in readImageFile";
        return returnMap;
    }

    QTextStream in (&file);

    while(!in.atEnd())
    {
        eachLine = in.readLine();

        if (eachLine.contains("define") && !eachLine.contains("#"))
        {
            //first split the line
            splitted1 = eachLine.split(" ");
            splitted2 = splitted1[2].split(":");
            returnMap.insert(splitted2[0], splitted2[1]);
        }

        //qDebug() << firstWord << " " << synonym << " " << score << "\n";

    } //end of reading each line

    file.close();

    return returnMap;
}

//writing to subinterps XML file
bool IOHandler::writeSubInterpsXML (SCUSITree* inputTree, QString filePath)
{
    //XML parts: make the root element and add it to the document
    QDomDocument document;

    QDomElement root = document.createElement("ranks");
    document.appendChild(root);

    QDomElement node;

    //Tree parts
    QList<SCUSITree::Node*> treeNodes = inputTree->getNodes();
    SCUSITree::Node* currentNode;

    //adding elements to the document based on the input tree
    for (int i = 0 ; i < treeNodes.size() ; ++i)
    {
        node = document.createElement("subinterp_node");

        currentNode = treeNodes.at(i);

        node.setAttribute("kind", currentNode->getKind());
        node.setAttribute("filename", currentNode->getFileName());
        node.setAttribute("rank", QString::number(currentNode->getRank()));
        node.setAttribute("children", currentNode->getChildren().join(","));
        node.setAttribute("ml", QString::number(currentNode->getMl()));
        node.setAttribute("id", currentNode->getID());
        node.setAttribute("parents", currentNode->getParents().join(","));
        node.setAttribute("expansion", currentNode->getExpansion());

        root.appendChild(node);
    }

    //write the document (XML tree) to file
    QFile file (filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "ERROR: Failed to open " << filePath << " file for writing in writeSubInterpsXML";
        return false;
    }
    else
    {
        QTextStream stream(&file);

        stream << document.toString();

        file.close();

        return true;
    }
}

//reading in the ICGInfo file
QList<ICGInfoNode*> IOHandler::readICGInfoFile(QString filePath)
{
    QFile file (filePath);
    QString eachLine;

    QList<ICGInfoNode*> returnList;
    String2DoubleMap MLMap;

    QStringList splittedLine;
    QStringList IDs;
    QStringList UCGs;
    QStringList MLs;

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "ERROR: cannot open " << filePath << " file for reading in readICGInfoFile";
        return returnList;
    }

    QTextStream in (&file);

    while(!in.atEnd())
    {
        eachLine = in.readLine();

        splittedLine = eachLine.split("\t");
        IDs = splittedLine[2].split(":");
        UCGs = splittedLine[3].split(":");
        MLs = splittedLine[4].split(":");

        for (int i = 0 ; i < UCGs.size() ; ++i)
            MLMap.insert(UCGs[i], MLs[i].toDouble());

        returnList.append(new ICGInfoNode(splittedLine[0].toInt(), splittedLine[1], IDs, MLMap));

        MLMap.clear();

        //qDebug() << splittedLine[0].toInt() << " " << splittedLine[1] << "\n";

    } //end of reading each line

    file.close();

    return returnList;
}

//reading in the TextInfo file
QList<TextInfoNode*> IOHandler::readTextInfoFile(QString filePath)
{
    QFile file (filePath);
    QString eachLine;

    QList<TextInfoNode*> returnList;

    QStringList splittedLine;

    TextInfoNode* currentNode;
    QString textID;
    QStringList altNumbers;
    QString sentence;

    TextInfoNode::NewWord* currentNewWord;
    QString role;
    QString PoS;
    QString word;

    QString lastRole;
    bool firstRun = true;

    QStringList words;
    QStringList EDLabels;
    QStringList probabilities;

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "ERROR: cannot open " << filePath << " file for reading in readTextInfoFile";
        return returnList;
    }

    QTextStream in (&file);

    while(!in.atEnd())
    {
        eachLine = in.readLine();

        splittedLine = eachLine.split("\t");

        if (eachLine.contains("textid"))
        {
            textID = splittedLine[0].split("=").at(1);
            altNumbers = splittedLine[1].remove("*").split("=").at(1).split(":");
            sentence = splittedLine[2].split("=").at(1);

            currentNode = new TextInfoNode (textID, altNumbers, sentence);
        }
        else if (splittedLine.size() == 1)//empty line means the previous one is finished
        {
            currentNode->addNewWord(currentNewWord);
            returnList.append(currentNode);
            firstRun = true; //resetting the last role
        }
        else
        {
            word = splittedLine[0];
            role = splittedLine[1];
            PoS = splittedLine[2];

            if (role.compare(lastRole) != 0 && !firstRun)
            {
                //qDebug() << "last role: " << lastRole << " role: " << role;
                currentNode->addNewWord(currentNewWord);
            }

            currentNewWord = new TextInfoNode::NewWord (role, PoS, word);

            words = splittedLine[3].split(":");
            EDLabels = splittedLine[4].split(":");
            probabilities = splittedLine[5].split(":");

            for (int i = 0 ; i < words.size() ; ++i)
            {
                currentNewWord->addOriginalWord(
                                new TextInfoNode::NewWord::OriginalWord(
                                words[i], EDLabels[i], probabilities[i].toDouble()));
            }

            lastRole = role;
            firstRun = false;

        }
        //qDebug() << splittedLine[0].toInt() << " " << splittedLine[1] << "\n";

    } //end of reading each line

    file.close();

    return returnList;
}
