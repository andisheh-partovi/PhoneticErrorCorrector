#include <QCoreApplication>

#include "application.h"

void readBatchFile(QString filePath)
{
    QFile file (filePath);
    QString eachLine;
    QStringList splitted;

    Application* app = new Application();

    QString currentDescription = "asrprob-NPLR.RE+I-1";
    int topN = 10;
    QString outputFilePath = "dotxmlfiles/output-ssp-asrprob-NPLR/Reza/ASR_WIN7/description-RE-AW7-I-1";
    double constantDivider = 77;
    int option = 2;
    bool onlyObj = true;


    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "ERROR: cannot open " << filePath << " file for reading";
    }

    QTextStream in (&file);

    while(!in.atEnd())
    {
        eachLine = in.readLine();
        splitted = eachLine.split(" ");

        outputFilePath = splitted[1].remove(".");
        currentDescription = splitted[2];
        topN = splitted[3].toInt();
        constantDivider = splitted[4].toDouble();
        option = splitted[5].toInt();

        option = 2;

        app->run(currentDescription, topN, outputFilePath, constantDivider, option, onlyObj);

    } //end of reading each line

    file.close();

    app->writeFinalLog(app->getEpsilonLog(), option);
}

//void readNewBatchFile(QString filePath)
//{
//    QFile file (filePath);
//    QString eachLine;
//    QStringList splitted;

//    Application* app = new Application();

//    QString currentDescription = "asrprob-NPLR.RE+I-1";
//    int topN = 10;
//    QString outputFilePath = "dotxmlfiles/output-ssp-asrprob-NPLR/Reza/ASR_WIN7/description-RE-AW7-I-1";
//    double constantDivider = 77;
//    int option = 1;
//    bool onlyObj = true;


//    if (!file.open(QFile::ReadOnly | QFile::Text))
//    {
//        qDebug() << "ERROR: cannot open " << filePath << " file for reading";
//    }

//    QTextStream in (&file);

//    while(!in.atEnd())
//    {
//        eachLine = in.readLine();
//        splitted = eachLine.split(" ");

//        outputFilePath = splitted[1].remove(".");
//        currentDescription = splitted[2];
//        topN = splitted[3].toInt();
//        constantDivider = splitted[4].toDouble();
//        option = splitted[5].toInt();

//        option = 2;

//        app->run(currentDescription, topN, outputFilePath, constantDivider, option);

//    } //end of reading each line

//    file.close();

//    app->writeFinalLog(app->getEpsilonLog(), option);
//}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Application* app = new Application();


    //-------------   handling command line argument
//    QStringList args = a.arguments();

//    if (args.count() != 6)
//    {
//        qDebug() << "ERROR: 5 arguments required" << endl;
//        return 1;
//    }

//    QString currentDescription = args[2];
//    int topN = args[3].toInt();
//    QString outputFilePath = args[1];
//    double constantDivider = args[4].toDouble();
//    int option = args[5].toInt();
    //-----------------------------------------------------------



    //-------------   hardcoded commandline arguments for testing
    QString currentDescription = "asrprob-NPR.CO+F-1";
    int topN = 10;
    QString outputFilePath = "dotxmlfiles/output-ssp-asrprob-NPR/Cora/ASR_WIN7/description-CO-AW7-F-1";
    double constantDivider = 77;
    int option = 1;
    bool onlyObj = true;
    //-----------------------------------------------------------

    //validating option:
    if (option != 1 && option != 2)
    {
        qDebug() << "ERROR: option must be 1 or 2. Running for 1";
        option = 2;
    }

    //app->run(currentDescription, topN, outputFilePath, constantDivider, option, onlyObj);

    //readBatchFile("batch.ecrun.asrprob-PLR");
    //readBatchFile("batch.ecrun.asrprob-NPR.wrong");
    //readNewBatchFile("batch.ecrun.asrprob-NPR.wrong");
    readBatchFile("batch.ecrun.new");

    return a.exec();
}
