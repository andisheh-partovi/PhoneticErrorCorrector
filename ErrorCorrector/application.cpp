#include "Application.h"

#include "auxiliaryoriginalwordcontainer.h"
#include <limits>

//constructor
Application::Application()
{
    ioHandler = new IOHandler();
    editDistanceHandle = new EditDistance();
}

/*
 * This method loads all the knowledge base files
 * Including:
 * 1. image files (4) to grab canonical word from,
 * 2. lexical.db to get the synonyms from
 * 3. phonetic dictionary
 *
 * */
void Application::openDataBaseFiles()
{
    //1. Loading synonyms dictionary
    synonymDictionary = ioHandler->readSynonymFile("knowledge_files/lexical.db");

    //1.1 Convert scores to probabilities (formula provided in method comments)
    QVector<double> minMax = getMinMaxSynonymScores(synonymDictionary); //returns the global min and max scores in the dic
    //qDebug() << "max: " << QString::number(minMax[0]) << " , min: " << QString::number(minMax[1]);
    synonymProbability = createSynonymProbabilities(synonymDictionary, 0.5, minMax[0], minMax[1]);

    //Hackingly adding the speaker
    String2DoubleMap speaker;
    speaker.insert("speaker", 1);
    synonymProbability.append(speaker);

    //testing:
    //displaySynonymDictionary(synonymProbability);

    //2. Loading phonetic dictionary
    phoneticDictionary = ioHandler->readDictionaryFile("knowledge_files/CMU_phoneticDictionary.txt");

    //3. Loading all 4 image files
    QString imageFileName;

    for (int i = 1 ; i <= 4 ; ++i)
    {
        imageFileName = "knowledge_files/image" + QString::number(i) + ".kb";
        imageMap.unite(ioHandler->readImageFile(imageFileName));
    }

    //handcrafring the excluded synonyms:
    excludedSynonyms.append("thing");
    excludedSynonyms.append("item");
    excludedSynonyms.append("object");
    excludedSynonyms.append("here");
    excludedSynonyms.append("there");

}

//runs all the calculations. All the workflow happens here. Most lines are commented
bool Application::run(QString currentDescription, int topN, QString inputFilePath, double constantDivider, int option, bool onlyObj)
{
    QString genericPath = currentDescription.split(".").at(0);

    bool noCanonicalFound;

    QString currentICGInfoFilePath;
    QList<ICGInfoNode*> currentICGInfos;
    int currentICGRank;
    QStringList currentTextIDs;
    QString currentICGID;
    int minNumberOfUCGs;

    String2DoubleMap UCGProbabilities;

    QMap<int , DotFileNode*> dotFileNodesPerRank;
    QMap<QString, double> currentPr_lex_PerRole;
    QMap<QString,int> Ks;

    QString currentTextInfoFilePath;
    QList<TextInfoNode*> currentTextInfo;
    QList<TextInfoNode*> currentApplicableTextInfoNode;
    QStringList rolesOfThing;
    QStringList rolesOfEpsilon;

    QString currentDotFileMainPath;
    QString currentDotFilePath;
    DotFileNode* currentDotFileNode;
    QString currentCanonicalWord;
    QString currentAssignedWord;
    String2DoubleMap currentSynonyms;

    double currentEditDistanceScore;
    QString currentSynonym;
    QList<TextInfoNode::NewWord*> currentNewWords;
    QList<AuxiliaryOriginalWordContainer*> currentOriginalWords;
    QList<TextInfoNode::NewWord::OriginalWord*> heardWords;
    bool isThing;
    QString currentHeardWord;
    String2DoubleMap editDistanceScorePerSynonym;

    String2DoubleMap currentEditDistanceProbabilitiesPerSynonym;
    String2DoubleMap argmaxPerHeardWord;
    QMap<int, String2DoubleMap> argmaxPerHeardWordPerRank;
    QMap<QString, QMap<int, String2DoubleMap> > argmaxPerHeardWordPerRankPerRole;
    QMap<QString, String2DoubleMap> argmaxPerRole;
    double currentArgmax;
    double currentEditDistanceProbability;
    String2DoubleMap currentMultipliedProbsPerSynonym;
    double currentMultiply;

    double PASRSum;
    double constantMultiplier;
    double adjustedProbability;
    String2DoubleMap Pri;
    QMap <QString, String2DoubleMap> PriPerWord;
    QString currentMainWord;
    String2DoubleMap PrASRPerHeardWord;
    QMap<QString, String2DoubleMap> PrASRPerHeardWordPerMainWord;
    QList<double> PASRValues;
    QMap<QString, double> PASRSumPerMainWord;
    QList<double> UCGMLs;

    double Pr_Ui_and_lex_given_Ki;
    double New_Pr_lex_O;
    String2DoubleMap Pr_Ui_and_lex_given_Ki_PerHeardWord;
    QMap<QString, String2DoubleMap> Pr_Ui_and_lex_given_Ki_PerHeardWordPerMainWord;

    double oldMultiply;
    double newMultiply;
    int currentWeight;
    double Pr_I_given_U;
    QStringList usedHeardWords;
    String2DoubleMap Pr_I_given_U_PerHeardWord;

//    double constantFactorProbability;
//    String2DoubleMap constantFactorProbabilitiesPerHeardWord;

    double currentPrU;
    double finalPr;
    double currentFinalPrSum;
    double currentUCGSum;

    double currentFinalML;
    String2DoubleMap finalMLPerICGID;

    qDebug() << "started running for description " << currentDescription;
    log = "";
    log += "\t\t\t*********** Description: " + currentDescription + "\t\tTop: " + QString::number(topN) + " ***********\n\n";

    openDataBaseFiles();    //Loading Database Files



    //-------------------------- Loading Input Files ----------------------

    //"ICGinfo-" + genericPath +
    //currentICGInfoFilePath = "ICGinfo/icginfo." + currentDescription;
    QStringList descriptionSplitted = currentDescription.split(".");
    currentICGInfoFilePath = "ICGinfo/icginfo.output-ms-ssp-asrprob-NPR-new-specifierFixed2-noiseFixed." + descriptionSplitted[1];


    //1. openning the icginfo file
    currentICGInfos = ioHandler->readICGInfoFile(currentICGInfoFilePath);
    minNumberOfUCGs = getMinNumberOfUCGs(currentICGInfos);
    //qDebug() << "min= " << QString::number(minNumberOfUCGs);

    //2. openning the corresponding textinfo file
    currentTextInfoFilePath =  currentICGInfoFilePath.replace("ICGinfo" , "Textinfo");
    currentTextInfoFilePath =  currentTextInfoFilePath.replace("icginfo" , "textinfo");
    currentTextInfoFilePath = "Textinfo/textinfo." + descriptionSplitted[1];//overwrites the 2 previous lines
    currentTextInfo = ioHandler->readTextInfoFile(currentTextInfoFilePath);

    //3. Opening the dot files:
    //        currentDotFilePath = "dot_files/output-ssp-asrprob-NPLR/"
    //                + "ArunKo" +"/ASR_WIN7/description-" + "AR-AW7-F-2" + "/dot/subinterp_known_cg_c_by_ML_"
    //                + QString::number(currentICGRank) + ".dot";
            //dot_files/output-ssp-asrprob-NPLR/ArunKo/ASR_WIN7/description-AR-AW7-F-2/dot/
    //inputFilePath
    //"dotfiles-" + genericPath + "/" +
    currentDotFileMainPath = inputFilePath + "/dot/subinterp_known_cg_c_by_ML_";
    for (int i = 0 ; i < topN ; ++i)
    {
        currentDotFilePath =  currentDotFileMainPath + QString::number(i) + ".dot";
        currentDotFileNode = ioHandler->readDotFile(currentDotFilePath);
        dotFileNodesPerRank.insert(i , currentDotFileNode);

        //qDebug() << currentDotFileNode->toString(); //testing
    }


    //-------------------------- Working out the options ----------------------

    //find out which semantic role(s) is replaced with "thing"
    //this part is common amongst both options
    rolesOfThing = getThingRoles(currentTextInfo);

    //removing the "S" role
    if (rolesOfThing.contains("S"))
    {
        qDebug() << "WARNING: Role \"S\" was replaced by \"thing\" and therefore removed.\n";
        log += "WARNING: Role \"S\" was replaced by \"thing\" and therefore removed.\n";
        rolesOfThing.removeAll("S");
    }

    switch (option)
    {
    case 1:

        if (rolesOfThing.isEmpty())
        {
            qDebug() << "WARNING: There is nothing to calculate. No word was replaced with \"thing\"";
            return false;
        }
        break;

    case 2: //option2

        if (rolesOfThing.isEmpty())
            log += "-No word was replaced by \"thing\". Checking Pr_lex = 0 now.";
        else
        {
            log += "-Word(s) were replaced by \"thing\". Roles:";
            foreach (QString role, rolesOfThing)
                log += role + "\t";
            log += "\n-Checking Pr_lex = 0 now.";
        }

        //find out which semantic role(s) has pr_lex = epsilon and add it to the things
        rolesOfEpsilon = getEpsilonProbRolesComplete(dotFileNodesPerRank);
        allWordsEpsilon = false;
        if (!rolesOfEpsilon.isEmpty())
        {
            allWordsEpsilon = true;
            qDebug() << "all words were epsilon!";
            log += "\n-There was a case where all words were epsilon.";
        }
        rolesOfThing.append(rolesOfEpsilon);

        if (rolesOfThing.isEmpty())
        {
            qDebug() << "WARNING: There is nothing to calculate. No word was replaced with \"thing\" or no word had pr_lex = epsilon";
            return false;
        }
        break;

    default:

        qDebug() << "ERROR: Option must have the value 1 or 2";
        break;
    }


    //removing duplicate rules:
    rolesOfThing.removeDuplicates();

    //removing non-existant roles:
    //1. finding them
    bool foundRole = false;
    QStringList removeList;
    foreach (QString role, rolesOfThing)
    {
        foundRole = false;

        foreach (TextInfoNode* currentTextInfoNode, currentTextInfo)
        {
            foreach (TextInfoNode::NewWord* word, currentTextInfoNode->getWords())
            {
                if (word->getRole().compare(role) == 0)
                {
                    foundRole = true;
                    break;
                }
            }
        }

        if (!foundRole)
           removeList.append(role);
    }
    //2.removing them
    foreach (QString removeRole, removeList) {
        rolesOfThing.removeAll(removeRole);
    }

    //remove the L role if onlyObj is chosen
    if (onlyObj)
    {
        if (rolesOfThing.size() == 2 && rolesOfThing.contains("L"))
            rolesOfThing.removeAll("L");
    }

    log += "\n-The following roles will be calculated: ";
    foreach (QString role, rolesOfThing)
    {
        log += role + "\t";
        qDebug() << role;
    }

    if (rolesOfThing.size() == 2 && allWordsEpsilon)
    {
        epsilonLog += currentDescription;
    }


    //-------------------------- All the calculations ----------------------


    //2. Calculate K (number of unique objects for each role)
    foreach (QString role, rolesOfThing)
    {
        Ks.insert(role, getNumberOfUniqueObjects(dotFileNodesPerRank, role) );
    }

    //BIG LOOP : for each role
    foreach (QString currentRole, rolesOfThing)
    {
        //BIG LOOP : for each rank
        for (int i = 0 ; i < topN ; ++i)
        {
            currentICGRank = currentICGInfos[i]->getICGRank();
            currentICGID = currentICGInfos[i]->getICGID();


            log += "\n\n-------------------------------\nrank: "
                    + QString::number(currentICGRank) + "\n-------------------------------\n";

            //1. Getting parent UCGs and calculating Pr(Ui)
            log += "\n-Calculating Pr(Ui):\n\n";
            UCGProbabilities = currentICGInfos[i]->getParentUCGMLs();

            QMapIterator <QString, double> iter0(UCGProbabilities);
            while (iter0.hasNext())
            {
                iter0.next();
                UCGProbabilities.insert(iter0.key() , ML2Prob(UCGProbabilities[iter0.key()]));
            }

            //logging
            QMapIterator <QString, double> iter01(UCGProbabilities);
            while (iter01.hasNext())
            {
                iter01.next();
                log += iter01.key() + " : " + QString::number(iter01.value()) + "\n";
            }

            //2. processing current dot file
            currentDotFileNode = dotFileNodesPerRank[currentICGRank];

            //2.0 getting the weight
            currentWeight = currentDotFileNode->getWeight();

            //2.1. getting the canonical word (CW)
            noCanonicalFound = false;
            foreach (DotFileNode::DotFileItem* item, currentDotFileNode->getItems()) {
                if (item->getRole().compare(currentRole) == 0) //if the roles match
                {
                    //first grabbing the Pr_lex for later use
                    currentPr_lex_PerRole.insert(currentRole, item->getPR_lex());

                    //getting the canonical word (CW)
                    currentAssignedWord = item->getAssignedWord();
                    QMap<QString, QString>::const_iterator iter = imageMap.find(currentAssignedWord);//find the canonical word in the imageMap

                    if (iter == imageMap.end())
                    {
                        qDebug() << "WARNING: Canonical word not found for" << currentAssignedWord;
                        noCanonicalFound = true;
                        currentCanonicalWord = "NOCANONICALWORD";
                    }
                    else
                    {
                        currentCanonicalWord = iter.value().toLower();
                        //qDebug() << "currentCanonicalWord: " << iter.value().toLower();
                        log += "\n-current word from scusi: " + currentAssignedWord + "\n";
                    }
                }
            }

            //2.2. looking up the canonical word in the lexicon.db and get the synonyms with their probabilities
            if (currentCanonicalWord.compare("NOCANONICALWORD") != 0)
                currentSynonyms = findSynonymsOfAWord(currentCanonicalWord);
                //currentSynonymsPerRole.insert(iter.key(), currentSynonyms);
            else
            {
                currentSynonyms.clear();
                currentSynonyms.insert("NOCANONICALWORDSYNONYM", EPSILON);
            }


            log += "\n-Canonical word: " + currentCanonicalWord + "\n\n";

            //logging:
            log += "-Synonyms:\n";
            QMapIterator <QString, double> iter2(currentSynonyms);
            while (iter2.hasNext())
            {
                iter2.next();
                //qDebug() << iter2.key() << ", " << iter2.value();
                log += iter2.key() + ", " + QString::number(iter2.value()) + "\n";
            }

            //3. grab the text IDs from the texfileinfo that are included in this rank
            currentTextIDs = currentICGInfos[i]->getTextIDs();
            foreach (TextInfoNode* currentTextInfoNode, currentTextInfo)
                if ( currentTextIDs.contains(currentTextInfoNode->getTextID()) )
                    currentApplicableTextInfoNode.append(currentTextInfoNode);

            //4. do the edit distance calculations on the applicable textinfo node
            log += "\n-Calculating edit distance score:";


            //4.1 grabbing the heard words
            foreach (TextInfoNode* node, currentApplicableTextInfoNode)    //iterating thro all the textinfo nodes
            {
                currentNewWords = node->getWords();

                foreach (TextInfoNode::NewWord* word, currentNewWords) //iterating thro all the original words
                    if (word->getRole().compare(currentRole) == 0)
                    {
                        //grabbing the heard words for this role

                        if (word->getWord().compare("thing") == 0)
                            currentOriginalWords.append( new AuxiliaryOriginalWordContainer(word->getWord(), word->getOriginalWords(), true) );
                        else
                            currentOriginalWords.append( new AuxiliaryOriginalWordContainer(word->getWord(), word->getOriginalWords(), false) );
                    }
            }

            //4.2 calculating edit distance between heardwords and synonyms
            foreach (AuxiliaryOriginalWordContainer* element, currentOriginalWords)
            {
                heardWords = element->getHeardWords();
                isThing = element->getIsThing();

                if (isThing)
                {
                    log += "\nHeardWord , Synonym : EditDistanceScore : maxLength : EditDistanceProbability : Synonym Prob : Multiplied by Synonym Prob\n\n";
                    foreach (TextInfoNode::NewWord::OriginalWord* heardWord, heardWords) //iterating thro heard words
                    {
                        log += "\n";
                        currentHeardWord = heardWord->getWord();

                        //-----------------------  6.4.2.1 getting edit distance score
                        QMapIterator <QString, double> iter4(currentSynonyms);
                        while (iter4.hasNext()) //iterating thro all the synonyms
                        {
                            iter4.next();
                            currentSynonym = iter4.key();

                            if (excludedSynonyms.contains(currentSynonym))
                                ;//pass and don't do edit distance if it's an excluded synonym
                            else if (currentSynonym.compare("NOCANONICALWORDSYNONYM") != 0)
                            {
                                currentEditDistanceScore = getEditDistance( currentHeardWord , currentSynonym);
                                editDistanceScorePerSynonym.insert(currentSynonym, currentEditDistanceScore);
                            }
                            else
                            {
                               currentEditDistanceScore = EPSILON;
                               editDistanceScorePerSynonym.insert(currentSynonym, currentEditDistanceScore);
                            }


                        }

                        //-----------------------  4.2.2 getting edit distance probability and multiply by synonym prob

                        QMapIterator <QString, double> iter41(editDistanceScorePerSynonym);
                        while (iter41.hasNext()) //iterating thro all the synonyms
                        {
                            iter41.next();
                            currentSynonym = iter41.key();
                            currentEditDistanceScore = iter41.value();

                            if (!noCanonicalFound)
                            {
                                //converting scores to probabilities
                                //currentEditDistanceProbability = 1- (currentEditDistanceScore / maximumEditScore); //approach 1: using maxScore
                                currentEditDistanceProbability = 1- (currentEditDistanceScore / getMaxPhoneticSize(currentHeardWord, currentSynonym)); //approach 2: using maxLength
                                if (currentEditDistanceProbability <= 0)
                                    currentEditDistanceProbability = EPSILON;

                                currentEditDistanceProbabilitiesPerSynonym.insert(currentSynonym, currentEditDistanceProbability);

                                //multiplying editDistance probability and synonym probability
                                currentMultiply = currentEditDistanceProbability * currentSynonyms[currentSynonym];
                            }
                            else
                               currentMultiply = EPSILON;


                            currentMultipliedProbsPerSynonym.insert (currentSynonym, currentMultiply);

                            //logging
                            if (!noCanonicalFound)
                            {
                                log +=  currentHeardWord + " , " + currentSynonym + " : 1 - " + QString::number(currentEditDistanceScore)
                                         + " / " + QString::number(getMaxPhoneticSize(currentHeardWord, currentSynonym))
                                         + " = " + QString::number(currentEditDistanceProbability)
                                         + " : " + QString::number(currentSynonyms[currentSynonym])
                                         + " : " + QString::number(currentMultiply) +"\n";
                            }
                            else
                                log +=  currentHeardWord + " , " + currentSynonym + " : " + QString::number(currentMultiply) +"\n";

                        }

                        //saving the maximum and logging
                        currentArgmax = getMax(currentMultipliedProbsPerSynonym.values());
                        argmaxPerHeardWord.insert(currentHeardWord, currentArgmax);
                        argmaxPerHeardWordPerRank.insert(currentICGRank , argmaxPerHeardWord);
                        log +=  "\n-For " + currentHeardWord + ", argmax probability is: " + QString::number(currentArgmax) + "\n\n";

                        //----------------------  4.2.3 calculating individual Pri for PrASRPerHeardWord
                        //pr_class_replaceword_obj
                        //pr_class_replaceword_land
                        /*
                         * case1) EDlabel = wrong
                         *  EDprobability * pr_class_replaceword_obj
                         *
                         *  case2) EDlabel = correct
                         *  (1-EDprobability) * pr_class_replaceword_obj
                         *
                         * Also note that if EDprob =1, we decided to use 0.999999 instead of 1.
                         */

                        //choosing between the two constants (see the notes above)
                        if (currentRole.compare("O") == 0)
                            constantMultiplier = pr_class_replaceword_obj;
                        else if (currentRole.compare("L") == 0)
                            constantMultiplier = pr_class_replaceword_land;
                        else
                            qDebug() << "ERROR: a lable other than L and O is being processed";

                        //adjusting the probability if it's 1 (see the notes above)
                        adjustedProbability = heardWord->getProbability() == 1 ? 0.999999 : heardWord->getProbability();

                        //calculating pri's and saving them to a map
                        if (heardWord->getEDLabel().compare("yes") == 0)
                            Pri.insertMulti(currentHeardWord, ( (1 - adjustedProbability) * constantMultiplier ));
                        else
                            Pri.insertMulti(currentHeardWord, (adjustedProbability * constantMultiplier));

                    }//iterating thro heard wrods


                }// if "thing"
                else if (option == 2) //if in option 2
                {
                    foreach (TextInfoNode::NewWord::OriginalWord* heardWord, heardWords) //iterating thro heard words
                    {
                        log += "\n";
                        currentHeardWord = heardWord->getWord();

                        if (currentSynonyms.contains(currentHeardWord))
                        {
                            element->setIsLexEpsilon(false);
                            currentMultiply = currentSynonyms[currentHeardWord];
                            log += "\nHeardWord : Synonym Prob\n\n";
                            log +=  currentHeardWord + " : " + QString::number(currentMultiply) +"\n";

                            currentArgmax = currentMultiply;
                        }
                        else //if not in the sysnonyms, must do the edit distance
                        {
                            element->setIsLexEpsilon(true);
                            //-----------------------  6.4.2.1 getting edit distance score
                            QMapIterator <QString, double> iter42(currentSynonyms);
                            while (iter42.hasNext()) //iterating thro all the synonyms
                            {
                                iter42.next();
                                currentSynonym = iter42.key();

                                if (excludedSynonyms.contains(currentSynonym))
                                    ;//pass and don't do edit distance if it's an excluded synonym
                                else if (currentSynonym.compare("NOCANONICALWORDSYNONYM") != 0)
                                {
                                    currentEditDistanceScore = getEditDistance( currentHeardWord , currentSynonym);
                                    editDistanceScorePerSynonym.insert(currentSynonym, currentEditDistanceScore);
                                }
                                else
                                {
                                   currentEditDistanceScore = EPSILON;
                                   editDistanceScorePerSynonym.insert(currentSynonym, currentEditDistanceScore);
                                }
                            }

                            //-----------------------  4.2.2 getting edit distance probability and multiply by synonym prob

                            QMapIterator <QString, double> iter412(editDistanceScorePerSynonym);
                            while (iter412.hasNext()) //iterating thro all the synonyms
                            {
                                iter412.next();
                                currentSynonym = iter412.key();
                                currentEditDistanceScore = iter412.value();

                                if (!noCanonicalFound)
                                {
                                    //converting scores to probabilities
                                    //currentEditDistanceProbability = 1- (currentEditDistanceScore / maximumEditScore); //approach 1: using maxScore
                                    currentEditDistanceProbability = 1- (currentEditDistanceScore / getMaxPhoneticSize(currentHeardWord, currentSynonym)); //approach 2: using maxLength
                                    if (currentEditDistanceProbability <= 0)
                                        currentEditDistanceProbability = EPSILON;

                                    currentEditDistanceProbabilitiesPerSynonym.insert(currentSynonym, currentEditDistanceProbability);

                                    //multiplying editDistance probability and synonym probability
                                    currentMultiply = currentEditDistanceProbability * currentSynonyms[currentSynonym];
                                }
                                else
                                   currentMultiply = EPSILON;


                                currentMultipliedProbsPerSynonym.insert (currentSynonym, currentMultiply);

                                //logging
                                if (!noCanonicalFound)
                                {
                                    log +=  currentHeardWord + " , " + currentSynonym + " : 1 - " + QString::number(currentEditDistanceScore)
                                             + " / " + QString::number(getMaxPhoneticSize(currentHeardWord, currentSynonym))
                                             + " = " + QString::number(currentEditDistanceProbability)
                                             + " : " + QString::number(currentSynonyms[currentSynonym])
                                             + " : " + QString::number(currentMultiply) +"\n";
                                }
                                else
                                    log +=  currentHeardWord + " , " + currentSynonym + " : " + QString::number(currentMultiply) +"\n";

                            }

                            currentArgmax = getMax(currentMultipliedProbsPerSynonym.values());
                        }

                        //dealing with Pri if pr_lex is epsilon
                        if (element->getIsLexEpsilon())
                        {
                            //choosing between the two constants (see the notes above)
                            if (currentRole.compare("O") == 0)
                                constantMultiplier = pr_class_replaceword_obj;
                            else if (currentRole.compare("L") == 0)
                                constantMultiplier = pr_class_replaceword_land;
                            else
                                qDebug() << "ERROR: a lable other than L and O is being processed";

                            //adjusting the probability if it's 1 (see the notes above)
                            adjustedProbability = heardWord->getProbability() == 1 ? 0.999999 : heardWord->getProbability();

                            //calculating pri's and saving them to a map
                            if (heardWord->getEDLabel().compare("yes") == 0)
                                Pri.insertMulti(currentHeardWord, ( (1 - adjustedProbability) * constantMultiplier ));
                            else
                                Pri.insertMulti(currentHeardWord, (adjustedProbability * constantMultiplier));
                        }

                        //saving the maximum and logging
                        element->setCurrentMultiply(currentArgmax);
                        argmaxPerHeardWord.insert(currentHeardWord, currentArgmax);
                        argmaxPerHeardWordPerRank.insert(currentICGRank , argmaxPerHeardWord);
                        log +=  "\n-For " + currentHeardWord + ", argmax probability is: " + QString::number(currentArgmax) + "\n\n";

                    }//iterating thro heard wrods

                }//end of option 2
                else //if not "thing" and in option 1
                {
                    log += "\nHeardWord : Synonym Prob\n\n";
                    foreach (TextInfoNode::NewWord::OriginalWord* heardWord, heardWords) //iterating thro heard words
                    {
                        log += "\n";
                        currentHeardWord = heardWord->getWord();

                        if (currentSynonyms.contains(currentHeardWord))
                            currentMultiply = currentSynonyms[currentHeardWord];
                        else
                            currentMultiply = EPSILON;

                        log +=  currentHeardWord + " : " + QString::number(currentMultiply) +"\n";

                        //saving the maximum and logging
                        currentArgmax = currentMultiply;
                        element->setCurrentMultiply(currentArgmax);
                        argmaxPerHeardWord.insert(currentHeardWord, currentArgmax);
                        argmaxPerHeardWordPerRank.insert(currentICGRank , argmaxPerHeardWord);
                        log +=  "\n-For " + currentHeardWord + ", argmax probability is: " + QString::number(currentArgmax) + "\n\n";

                    }//iterating thro heard wrods

                }//if not "thing"

                PriPerWord.insertMulti(element->getWord(), Pri);
                Pri.clear();

                argmaxPerRole.insert(currentRole, argmaxPerHeardWord);
                argmaxPerHeardWordPerRankPerRole.insert(currentRole, argmaxPerHeardWordPerRank);
                argmaxPerHeardWordPerRank.clear();


                editDistanceScorePerSynonym.clear();
                currentEditDistanceProbabilitiesPerSynonym.clear();
                currentMultipliedProbsPerSynonym.clear();
            }


            //4.3 PrASRPerHeardWord calculation
            log +=  "\n-Pri calculations:\ncase 1) EDlabel = wrong :\nEDprobability * pr_class_replaceword_obj\ncase 2) EDlabel = correct :\n(1-EDprobability) * pr_class_replaceword_obj\n\n";

            QMapIterator <QString, String2DoubleMap> priPerWordIterator(PriPerWord);
            while (priPerWordIterator.hasNext()) //iterating thro all the main words
            {
                priPerWordIterator.next();
                currentMainWord = priPerWordIterator.key();
                Pri = priPerWordIterator.value();

                log +=  "For " + currentMainWord + ":\n";

                QMapIterator <QString, double> iter5(Pri);
                int j = 1;
                while (iter5.hasNext()) //iterating thro all the heard words and their Pri
                {
                    iter5.next();

                    //logging:
                    log +=  "Pr" + QString::number(j) + " for " + iter5.key() + " : " + QString::number(iter5.value()) + "\n";
                    j++;

                    if (PrASRPerHeardWord.contains(iter5.key()))
                        PrASRPerHeardWord.insert(iter5.key() , PrASRPerHeardWord[iter5.key()] + iter5.value());
                    else
                        PrASRPerHeardWord.insert(iter5.key(), iter5.value());
                }

                PrASRPerHeardWordPerMainWord.insert(currentMainWord, PrASRPerHeardWord);
                PrASRPerHeardWord.clear();

                PASRValues = Pri.values();
                PASRSum = getSum(PASRValues);
                PASRSumPerMainWord.insert(currentMainWord, PASRSum);
                //qDebug() << "rank: " << currentICGRank << " sum: " << PASRSum;    //testing
            }


            QMapIterator <QString, String2DoubleMap> prASRIterator(PrASRPerHeardWordPerMainWord);
            while (prASRIterator.hasNext()) //iterating thro all the main words
            {
                prASRIterator.next();
                currentMainWord = prASRIterator.key();
                PrASRPerHeardWord = prASRIterator.value();

                QMapIterator <QString, double> iter6(PrASRPerHeardWord);
                while (iter6.hasNext())
                {
                    iter6.next();
                    //qDebug() << PrASRPerHeardWord[iter6.key();    //testing
                    PrASRPerHeardWord.insert(iter6.key() , (PrASRPerHeardWord[iter6.key()] / PASRSumPerMainWord[currentMainWord]));
                }
            }

            //logging:
            log +=  "\n-PrASR calculations (PrASR = (Pr1 + Pr2 + ... Prn)/Total)\n\n";

            QMapIterator <QString, String2DoubleMap> prASRIterator2(PrASRPerHeardWordPerMainWord);
            while (prASRIterator2.hasNext()) //iterating thro all the main words
            {
                prASRIterator2.next();
                currentMainWord = prASRIterator2.key();
                PrASRPerHeardWord = prASRIterator2.value();

                log += "For " + currentMainWord + ":\n";

                QMapIterator <QString, double> iter7(PrASRPerHeardWord);
                while (iter7.hasNext())
                {
                    iter7.next();
                    log += "PrASR for " + iter7.key() + " : " + QString::number(iter7.value()) + "\n";
                }
            }

            //5. Calculating Pr(Ui, lex | Ki) for each heard word
            //Pr(Ui, lex | Ki) = 1/K * PrPhonetic * PrASR
            log +=  "\n-Pr(Ui, lex | Ki) calculations : Pr(Ui, lex | Ki) = PrPhonetic\n";
            log +=  "!!!NOTE: This used to be:PrPhonetic * PrASR * 1/K!!!\n\n";

            QMapIterator <QString, String2DoubleMap> prASRIterator3(PrASRPerHeardWordPerMainWord);
            while (prASRIterator3.hasNext()) //iterating thro all the main words
            {
                prASRIterator3.next();
                currentMainWord = prASRIterator3.key();
                PrASRPerHeardWord = prASRIterator3.value();

                log += "For " + currentMainWord + ":\n";

                QMapIterator <QString, double> iter8(PrASRPerHeardWord);
                while (iter8.hasNext())
                {
                    iter8.next();
                    currentHeardWord = iter8.key();

                    //Pr_Ui_and_lex_given_Ki =
                            //argmaxPerHeardWord[currentHeardWord] * PrASRPerHeardWord[currentHeardWord] / Ks[currentRole];
                    if (argmaxPerHeardWord[currentHeardWord] !=0 )
                        Pr_Ui_and_lex_given_Ki = argmaxPerHeardWord[currentHeardWord];
                    else //solving the inf problem
                        Pr_Ui_and_lex_given_Ki = EPSILON;

                    Pr_Ui_and_lex_given_Ki_PerHeardWord.insert(currentHeardWord, Pr_Ui_and_lex_given_Ki) ;

                    //logging:
                    log += "Pr(Ui, lex | Ki) for " + currentHeardWord + " : "
                            + QString::number(argmaxPerHeardWord[currentHeardWord]) + " = "
                            + QString::number(Pr_Ui_and_lex_given_Ki) + "\n";

//                    log += "Pr(Ui, lex | Ki) for " + currentHeardWord + " : "
//                            + QString::number(argmaxPerHeardWord[currentHeardWord]) + " * "
//                            + QString::number(PrASRPerHeardWord[currentHeardWord]) + " / "
//                            + QString::number(Ks[currentRole]) + " = "
//                            + QString::number(Pr_Ui_and_lex_given_Ki) + "\n";
                }

                Pr_Ui_and_lex_given_Ki_PerHeardWordPerMainWord.insert(currentMainWord, Pr_Ui_and_lex_given_Ki_PerHeardWord);
                //Pr_Ui_and_lex_given_Ki_PerHeardWord.clear();
            }

//            //6. calculating the constants Pr (HW | SCUSI_Obj)and Pr ("thing" |SCUSI_Obj)for each heard word
//            log +=  "\n-calculating: Pr (HW | SCUSI_Obj) and Pr (\"thing\" |SCUSI_Obj)\n\n";
//            QMapIterator <QString, double> iter9(argmaxPerHeardWord);
//            while (iter9.hasNext())
//            {
//                iter9.next();
//                currentHeardWord = iter9.key();

//                constantFactorProbability = iter9.value() / currentSynonyms["thing"];
//                constantFactorProbabilitiesPerHeardWord.insert(currentHeardWord, constantFactorProbability);

//                //logging:
//                log += "For " + currentHeardWord + " : Pr (HW | SCUSI_Obj) = "
//                        + QString::number(iter9.value()) + " , Pr (\"thing\" |SCUSI_Obj) = "
//                        + QString::number(currentSynonyms["thing"]) + " , constant factor = "
//                        + QString::number(constantFactorProbability) + "\n";
//            }


            //7. calculating Pr( I | U )'
            //Pr( I | U )' = (Pr_lex_O * Pr_col_O * Pr_size_O * Pr_uk_O * Pr_lex_L * ... * Pr_loc)/Pr_lex_O
            log +=  "\n-Pr( I | U )\' calculations\nPr( I | U )\' = (Pr_lex_O * Pr_col_O * Pr_size_O * Pr_uk_O * Pr_lex_L * ... * Pr_loc)/Pr_lex_role\n\n";

            foreach (AuxiliaryOriginalWordContainer* element, currentOriginalWords)
            {
                heardWords = element->getHeardWords();
                isThing = element->getIsThing();

                foreach (TextInfoNode::NewWord::OriginalWord* heardWord, heardWords) //iterating thro heard words
                {
                    log += "\n";
                    currentHeardWord = heardWord->getWord();

                    oldMultiply = currentDotFileNode->getAllProbsMultiplied();
                    //the tertiary operator debugs scusi. There is a known bug in scusi that sometimes produces 0 pr_lex
                    //if pr_lex is 0, we replace it by epsilon
                    newMultiply = (oldMultiply / (currentPr_lex_PerRole[currentRole] == 0 ? EPSILON : currentPr_lex_PerRole[currentRole]) );

                    element->setPr_I_given_U_prime(newMultiply);

                    //logging:
                    log += "Pr( I | U )\' for " + currentHeardWord + " : "
                            + QString::number(oldMultiply) + " / "
                            + QString::number(currentPr_lex_PerRole[currentRole]) + " = "
                            + QString::number(newMultiply) + "\n";
                }
            }

            //8. calculating Pr( I | U )
            //Pr( I | U ) = Pr( I | U )' * New_Pr_lex_O
            log +=  "\n-Pr( I | U ) calculations\nPr( I | U ) = Pr( I | U )\' * New_Pr_lex_O\n\n";

            foreach (AuxiliaryOriginalWordContainer* element, currentOriginalWords)
            {
                heardWords = element->getHeardWords();
                isThing = element->getIsThing();

                foreach (TextInfoNode::NewWord::OriginalWord* heardWord, heardWords) //iterating thro heard words
                {
                    log += "\n";
                    currentHeardWord = heardWord->getWord();

                    if (isThing)
                        //Pr_Ui_and_lex_given_Ki_PerHeardWordPerMainWord[element->getWord()][currentHeardWord];
                        New_Pr_lex_O = Pr_Ui_and_lex_given_Ki_PerHeardWord[currentHeardWord];
                    else
                        New_Pr_lex_O = element->getCurrentMultiply();

                    Pr_I_given_U = element->getPr_I_given_U_prime() * New_Pr_lex_O;

                    element->setPr_I_given_U(Pr_I_given_U);
                    Pr_I_given_U_PerHeardWord.insert(currentHeardWord, Pr_I_given_U) ;

                    //logging:
                    log += "Pr( I | U ) for " + currentHeardWord + " : "
                            + QString::number(element->getPr_I_given_U_prime()) + " * "
                            + QString::number(New_Pr_lex_O) + " = "
                            + QString::number(Pr_I_given_U) + "\n";
                }

            }

            //9. calculating the final thing
            // (Pr( I | U )) ^ 1/W * Pr(U)
            log +=  "\n-calculating the final probability:\n(Pr( I | U )) ^ 1/W * Pr(U)\n\n";
            currentFinalPrSum = 0;
            currentUCGSum = 0;
            int k = 1;

            UCGMLs = UCGProbabilities.values();
            qSort(UCGMLs);
            for (int l = 0 ; l < minNumberOfUCGs ; ++l)
            {
                currentPrU = UCGMLs[ UCGMLs.size() - 1 - l];//last elements are the maximums

                log += "For PrU" + QString::number(k) + " :\n";
                k++;

                foreach (AuxiliaryOriginalWordContainer* element, currentOriginalWords)
                {
                    heardWords = element->getHeardWords();
                    isThing = element->getIsThing();

                    foreach (TextInfoNode::NewWord::OriginalWord* heardWord, heardWords) //iterating thro heard words
                    {
                        currentHeardWord = heardWord->getWord();

                        if (isThing)
                        {
                           finalPr = qPow( Pr_I_given_U_PerHeardWord[currentHeardWord] , ( 1.0/currentWeight) ) * currentPrU;

                           //logging:
                           log += "\nFor " + currentHeardWord + " : Pr = ( "
                                   + QString::number(Pr_I_given_U_PerHeardWord[currentHeardWord]) + " ) ^ (1/"
                                   + QString::number(currentWeight) + ") * "
                                   + QString::number(currentPrU) + " = "
                                   + QString::number(finalPr) + "\n";
                        }
                        else
                        {
                           finalPr = qPow( element->getPr_I_given_U() , ( 1.0/currentWeight) ) * currentPrU;

                           //logging:
                           log += "\nFor " + currentHeardWord + " : Pr = ( "
                                   + QString::number(element->getPr_I_given_U()) + " ) ^ (1/"
                                   + QString::number(currentWeight) + ") * "
                                   + QString::number(currentPrU) + " = "
                                   + QString::number(finalPr) + "\n";
                        }

                        if (finalPr == 0)
                            finalPr = EPSILON;

                        currentUCGSum += finalPr;

                    }//for each heard word
                }

                //dealing with the zero prob situation for non-words
                if (currentUCGSum == 0)
                    currentUCGSum = EPSILON;

                log += "sum = " + QString::number(currentUCGSum) + "\n\n";
                currentFinalPrSum += currentUCGSum;
                currentUCGSum = 0;

            }//for each parent UCG


            //10. convertting to ML and outputting
            log += "\nFinal sum = " + QString::number(currentFinalPrSum);
            currentFinalPrSum = currentFinalPrSum / constantDivider;
            log +=  " / " + QString::number(constantDivider)
                    + " = " + QString::number(currentFinalPrSum);

            currentFinalML = Prob2ML(currentFinalPrSum);
            finalMLPerICGID.insert(currentICGID, currentFinalML);
            log += "\n***** For ICG ID " + currentICGID
                    + " : Prob = " + QString::number(currentFinalPrSum)
                    + " : ML = " + QString::number(currentFinalML) + " *****\n";

            //resetting lists and maps:
            PrASRPerHeardWord.clear();
            Pri.clear();
            currentApplicableTextInfoNode.clear();
            currentOriginalWords.clear();
            argmaxPerHeardWord.clear();
            currentOriginalWords.clear();

        }// for each rank

    }//for each role

    //-------------------------  WRITE BACK TO FILE  ------------------------

    //1. creating the output directory
    QString outputFilePath = "output/" + genericPath;
    QDir().mkdir(outputFilePath);

    //2. write to the output file
    QString finalResult;
    QMapIterator <QString, double> iter1(finalMLPerICGID);
    while (iter1.hasNext())
    {
        iter1.next();

        finalResult += iter1.key() + "\t" + QString::number(iter1.value()) + "\n";
    }
    QString finalResultFilePath = outputFilePath + "/ecoutput." + currentDescription + ".op" + QString::number(option);
    ioHandler->write2File(finalResultFilePath, finalResult);

    //3. finally write the log to the log file (e.g. eclog.AL+A-1)
    QString logFilePath = finalResultFilePath.replace("ecoutput" , "eclog");
    ioHandler->write2File(logFilePath, log);

    qDebug() << "finished running";

    return true;

//    argmaxPerRole.clear();
//    currentSynonymsPerRole.clear();
//    currentCanonicalWordsPerRole.clear();

    //-------------------------- Legecy code --------------------------
//    //1. Open the subinterps
//    SCUSITree* currentTree = ioHandler->readSubInterpsXML("subinterps.xml");

//    //2. foreach item in the subinterps that type is "known", grab the root and the dot file
//    QStringList currentRoots;
//    DotFileNode* currentDotFileNode2;
//    QString currentDotFileName;
//    QString currentPlainText;
//    QString currentCanonicalWord;
//    double currentMainML;

//    foreach (SCUSITree::Node* node, currentTree->getNodes())
//    {
//        if (node->getKind().compare("") == 0)//subinterp_known_cg_c
//        {
//            currentRoots = currentTree->getRootsID(node->getID());
//            foreach (QString root, currentRoots)
//            {
//                currentPlainText = ioHandler->readSubinterp_PlaintextXML("subinterp_plaintext_c_" + root + ".xml");
//            }

//            //change folder and open the dot file
//            currentDotFileName = "subinterp_known_cg_c_by_ML_" + QString::number(node->getRank()) + ".dot";
//            currentDotFileNode2 = ioHandler->readDotFile(currentDotFileName);
//        }
//    }



//    QList<SSPOutpputNode*> alternatives = ioHandler->readAW7XML ("description-AL-AW7-A-1.xml");

//------------------------------  WRITE BACK TO FILE  -------------------------------

    //ioHandler->writeSubInterpsXML (currentTree, "subinterps_write.xml");


//------------------------------------  DRAFT  --------------------------------------

//    DotFileNode* currentDotFileNode = ioHandler->readDotFile("subinterp_known_cg_c_by_ML_0.dot");
//    qDebug() << currentDotFileNode->toString();
//    foreach (DotFileNode::DotFileItem* item, currentDotFileNode->getItems()) {
//        qDebug() << item->toString();
//    }

//    foreach (TextInfoNode* node, currentTextInfos) {
//        qDebug() << node->toString();
//    }
//    foreach (ICGInfoNode* ICG, ICGInfos) {
//        qDebug() << ICG->toString();
//    }

    //QString desktopPath = QDesktopServices::DesktopLocation;
    //ioHandler->readTextFile("lexical.db");//"qrc:/resources/knowledgeBase/Things.kb"

//    //testing image reader:
//    QMapIterator <QString, QString> Iter(imageMap);

//    while (Iter.hasNext())
//    {
//        Iter.next();
//        qDebug() << Iter.key() << "\t" << Iter.value() << "\n";
//    }



    //    foreach (SSPOutpputNode* alt, alternatives) {
//        qDebug() << alt->toString();
//    }
    //qDebug() << ioHandler->readSubinterp_PlaintextXML("subinterp_plaintext_c_17.xml");

    //testing edit distance:
//    qDebug() << getEditDistance("fear", "sphere");
//    qDebug() << getEditDistance("fear me", "sphere");
//    qDebug() << getEditDistance("fear me", "sphere me");
//    qDebug() << getEditDistance("fear_me", "sphere");
//    qDebug() << getEditDistance("fear_me", "sphere me");

}

//Returns the minmum number of UCGs in any rank. This is used to reduce the number of UCGs
int Application::getMinNumberOfUCGs(QList<ICGInfoNode*> ICGInfoNodes)
{
    String2DoubleMap parents;
    int minSize = 1000;

    foreach (ICGInfoNode* node, ICGInfoNodes)
    {
       parents = node->getParentUCGMLs();

       if (parents.size() < minSize)
           minSize = parents.size();
    }

    return minSize;
}

/*
 * converting input scores to probabilities
 *
 * @Param: a list of edit distance scores (double)
 * @Return: a list of edit distance probabilities (double)
*/
QList<double> Application::getProbsFromScores(QList<double> inputScores)
{
    QList<double> probabilities;
    double max = getMax(inputScores);

    foreach (double entry, inputScores)
    {
        probabilities.append( 1 - (entry/max) );
    }

    return probabilities;
}

/*
 * returns the maximum of an input list
 *
 * @Param: a list of doubles
 * @Return: their maximum
*/
double Application::getMax(QList<double> inputList)
{
    double max = inputList[0];

    foreach (double entry, inputList)
    {
        if (entry > max)
            max = entry;
    }

    return max;
}

/*
 * converts ML to probability
 *
 * prob = 2 ^ (-ML)
 *
 * @Param: message length
 * @Return: probability
*/
double Application::ML2Prob(double ML)
{
    return qPow( 2 ,(-ML) );
}

/*
 * converts probability to ML
 *
 * ml = -log(prob)/log(2)
 *
 * @Param: probability
 * @Return: message length
*/
double Application::Prob2ML(double probability)
{
    return ( qLn(probability) / qLn(2) ) * -1;
}

/*
 * This method returns all the synonyms of an input canonical word along with their probabilities
 *
 * @Param: canonical word we want to find the synonyms of
 * @Return: a list of synonyms along with their prob
*/
String2DoubleMap Application::findSynonymsOfAWord(QString canonicalWord)
{
    foreach(String2DoubleMap map, synonymProbability)
        if (map.contains(canonicalWord))
            return map;

    qDebug() << "ERROR: canonical word " << canonicalWord << "doesn't have synonyms!";
}

/*
 * This method searches the dot file and if one or more of the words in it are "thing" (replaced by SSP),
 *      returns the roles of those words.
 *
 * NOTE: it is robust and checks whether "thing" is what the speaker said or is what SSP replaced something with
 *       only if "thing" is a replacement, its role is added and not otherwise
 *
 * @Param: data read from the textinfo.ID file
 * @Return: a list containing the roles of the word "thing"
*/
QStringList Application::getThingRoles(QList<TextInfoNode*> textInfoNodes)
{
    QStringList returnList;
    bool falseAlarm;

    foreach (TextInfoNode* node, textInfoNodes)
        foreach (TextInfoNode::NewWord* word, node->getWords())
        {
            falseAlarm = false;
            if (word->getWord().compare("thing") == 0)
            {
                foreach (TextInfoNode::NewWord::OriginalWord* ow, word->getOriginalWords())
                   if (ow->getWord().compare("thing") == 0)
                   {
                       falseAlarm = true;
                       break;
                   }

                if ( !falseAlarm && !returnList.contains(word->getRole()) )
                    returnList.append(word->getRole());
            }
        }

    return returnList;
}

/*
 * This method searches the dot file and if one or more of the words in it are "thing",
 *      returns the roles of those words.
 *
 * NOTE: it is the old (unrobust) version of the previous method
 *
 * @Param: data read from the textinfo.ID file
 * @Return: a list containing the roles of the word "thing"
*/
QStringList Application::getThingRolesUnrobust(QList<TextInfoNode*> textInfoNodes)
{
    QStringList returnList;

    foreach (TextInfoNode* node, textInfoNodes)
        foreach (TextInfoNode::NewWord* word, node->getWords())
            if (word->getWord().compare("thing") == 0)
                if (!returnList.contains(word->getRole()))
                    returnList.append(word->getRole());

    return returnList;
}

/*
 * This method returns the roles of the words if a word has pr_lex = EPSILON
 *
 * @Param: data read from the dot files
 * @Return: a list containing the roles of the words that have Pr_lex = EPSILON (0.0001)
*/
QStringList Application::getEpsilonProbRoles(QMap<int , DotFileNode*> dotFileNodesPerRank)
{
    QStringList returnList;
    DotFileNode* currentDotFileNode;

    QMapIterator<int , DotFileNode*> iter(dotFileNodesPerRank);

    while (iter.hasNext())
    {
        iter.next();
        currentDotFileNode = iter.value();

        foreach (DotFileNode::DotFileItem* item, currentDotFileNode->getItems())
        {
            if (item->getPR_lex() == EPSILON)
            {
                if (!returnList.contains(item->getRole()))
                    returnList.append(item->getRole());
            }
        }
    }

    returnList.removeAll("P");

    return returnList;
}

/*
 * This method returns the roles of the words where ALL the words have pr_lex = EPSILON
 * This is different with getEpsilonProbRoles method in a sense that the former is interested in seeing
 * just ONE case, whereas in this method, ALL words must have probability epsilon before they can play
 *
 * @Param: data read from the dot files
 * @Return: a list containing the roles of the words that have Pr_lex = EPSILON (0.0001)
*/
QStringList Application::getEpsilonProbRolesComplete(QMap<int , DotFileNode*> dotFileNodesPerRank)
{
    QStringList returnList;
    DotFileNode* currentDotFileNode;
    bool allObjectEpsilon = true;
    bool allLandmarkEpsilon = true;

    QMapIterator<int , DotFileNode*> iter(dotFileNodesPerRank);

    while (iter.hasNext())
    {
        iter.next();
        currentDotFileNode = iter.value();

        foreach (DotFileNode::DotFileItem* item, currentDotFileNode->getItems())
        {
            if (item->getRole().compare("O") == 0)
            {
                if (item->getPR_lex() > EPSILON)
                    allObjectEpsilon = false;
            }

            else if (item->getRole().compare("L") == 0)
            {
                if (item->getPR_lex() > EPSILON)
                    allLandmarkEpsilon = false;
            }
        }
    }

    if (allObjectEpsilon)
        returnList.append("O");

    if (allLandmarkEpsilon)
        returnList.append("L");


    return returnList;
}

/*
 * This method returns the number of unique objects of a specific role in all the dot files
 *
 * @Param: data read from the textinfo.ID file
 * @Return: a list containing the roles of the word "thing"
*/
int Application::getNumberOfUniqueObjects(QMap<int , DotFileNode*> dotFileNodesPerRank, QString role)
{
    QStringList countedObjects;
    QString currentObject;
    DotFileNode* currentDotFileNode;

    QMapIterator <int , DotFileNode*> iter(dotFileNodesPerRank);
    while (iter.hasNext())
    {
        iter.next();
        currentDotFileNode = iter.value();

        foreach (DotFileNode::DotFileItem* item, currentDotFileNode->getItems())
        {
            if (item->getRole().compare(role) == 0)
            {
                currentObject = item->getAssignedWord();
                if (!countedObjects.contains(currentObject))
                    countedObjects.append(currentObject);
            }
        }
    }

    return countedObjects.size();
}

//returns the global minimum and maximum values of the scores in the input dictionary of synonyms
QVector<double> Application::getMinMaxSynonymScores (SynonymDictionary inputDictionary)
{
    QVector<double> minMax;
    QList<double> currentValues;
    double currentMaxScore;
    double currentMinScore;



    foreach (String2DoubleMap word, inputDictionary)
    {
        currentValues.append(word.values());                      //return all the values in the map
    }

    qSort(currentValues.begin(), currentValues.end());  //sorting the list to get the max and the min
    currentMaxScore = currentValues.last();             //so last element is the maximum

    //min element is a bit tricky, because maps have elements with the score -1
    //that denotes the canonical word. So must search thro all the entries and
    //ignore the -1's until reaching the first non -1, which is the minimum
    for (int i = 0 ; i < currentValues.size() ; ++i)
    {
        if (currentValues[i] != -1)
        {
            currentMinScore = currentValues[i];
            break;
        }
    }

    minMax.append(currentMaxScore);
    minMax.append(currentMinScore);

    return minMax;
}

/*
 * This method compute normalised score (starts at min_acceptable_score for worst acceptable match)
 * prob = min_acceptable_score + (1 - min_acceptable_score) * (synlist.value(syn) - lex_acceptable_match) / (max_lex_match -lex_acceptable_match);

 * max_lex_match --> max score from lexical DB
 * lex_acceptable_match --> min score from lexical DB
 * static const double min_acceptable_score = 0.5; --> this is constant
*/
SynonymDictionary Application::createSynonymProbabilities (SynonymDictionary inputDictionary,
                                                           double min_acceptable_score, double max_lex_match
                                                           , double lex_acceptable_match)
{
    SynonymDictionary outputDictionary;
    String2DoubleMap currentWord;
    double currentMaxScore;
    double currentMinScore;
    double probability;


    foreach (String2DoubleMap word, inputDictionary)
    {
        currentMaxScore = max_lex_match;
        currentMinScore = lex_acceptable_match;

        QMapIterator <QString, double> Iter(word);

        while (Iter.hasNext())
        {
            Iter.next();

            if (Iter.value() == -1)     //it is the canonical word itself and the probability is 1
                currentWord.insert(Iter.key() , 1);
            else    //otherwise apply the formula
            {
                probability = min_acceptable_score + (1 - min_acceptable_score) * (Iter.value() - currentMinScore) / (currentMaxScore -currentMinScore);
                //qDebug() << "probability:" << probability;
                currentWord.insert(Iter.key() , probability);
            }
        }

        outputDictionary.append(currentWord);
        currentWord.clear();
    }

    return outputDictionary;
}

//just printing the synonym dictionary for testing purposes
void Application::displaySynonymDictionary(SynonymDictionary inputDictionary)
{
    foreach (String2DoubleMap word, inputDictionary) {

        QMapIterator <QString, double> Iter(word);

        while (Iter.hasNext())
        {
            Iter.next();
            qDebug() << Iter.key() << "\t" << Iter.value() << "\n";
        }

        qDebug() << "\n\n";
    }
}

//returns the phonetics of a given input string
QStringList Application::getPhonetic (QString inputPhrase)
{
    QStringList phrase = inputPhrase.replace("_", " ").toUpper().split(" ");
    QStringList phrase1Phonetic;

    foreach (QString word, phrase) {

        if (phoneticDictionary.contains(word))
            phrase1Phonetic.append(phoneticDictionary.value(word));
        else
            qDebug() << "ERROR: " << word << " not in the dictionary";
    }

    return phrase1Phonetic;
}

/*
 * Gets the edit distance between two prases
 * by converting them to their phonetics and running the edit distance code
 * */
double Application::getEditDistance(QString inputPhrase1, QString inputPhrase2)
{
//    QStringList phrase1 = inputPhrase1.replace("_", " ").toUpper().split(" ");
//    QStringList phrase2 = inputPhrase2.replace("_", " ").toUpper().split(" ");

    //converting user inputs to phontetics
    QStringList phrase1Phonetic = getPhonetic(inputPhrase1);
    QStringList phrase2Phonetic = getPhonetic(inputPhrase2);

//    foreach (QString word, phrase1) {

//        if (phoneticDictionary.contains(word))
//            phrase1Phonetic.append(phoneticDictionary.value(word));
//        else
//            qDebug() << "ERROR: " << word << " not in the dictionary";
//    }

//    foreach (QString word, phrase2) {

//        if (phoneticDictionary.contains(word))
//            phrase2Phonetic.append(phoneticDictionary.value(word));
//        else
//            qDebug() << "ERROR: " << word << " not in the dictionary";
//    }

    return editDistanceHandle->getDistance(phrase1Phonetic, phrase2Phonetic) - qAbs(inputPhrase1.replace("_", " ").count(" ") - inputPhrase2.replace("_", " ").count(" "));
}

double Application::getMaxPhoneticSize (QString inputPhrase1, QString inputPhrase2)
{
//    QStringList phrase1 = inputPhrase1.replace("_", " ").toUpper().split(" ");
//    QStringList phrase2 = inputPhrase2.replace("_", " ").toUpper().split(" ");

    //converting user inputs to phontetics
    QStringList phrase1Phonetic = getPhonetic(inputPhrase1);
    QStringList phrase2Phonetic = getPhonetic(inputPhrase2);

//    foreach (QString word, phrase1) {

//        if (phoneticDictionary.contains(word))
//            phrase1Phonetic.append(phoneticDictionary.value(word));
//        else
//            qDebug() << "ERROR: " << word << " not in the dictionary";
//    }

//    foreach (QString word, phrase2) {

//        if (phoneticDictionary.contains(word))
//            phrase2Phonetic.append(phoneticDictionary.value(word));
//        else
//            qDebug() << "ERROR: " << word << " not in the dictionary";
//    }

    return (double)qMax(phrase1Phonetic.size(), phrase2Phonetic.size()) - 1;
}

//returns the sum of all the elements in the input list
double Application::getSum(QList<double> inputList)
{
    double sum = 0;

    foreach (double entry, inputList) {
        sum += entry;
    }

    return sum;
}

//this method is specifically used to write back logs after all the runs are done
void Application::writeFinalLog(QString log, int optionNumber)
{
    QString logFilePath = "output/epsilonLog.op" + QString::number(optionNumber);

    ioHandler->write2File(logFilePath, log);
}

QString Application::getEpsilonLog()
{
    return this->epsilonLog;
}

Application::~Application()
{
    delete ioHandler;
    delete editDistanceHandle;
}
