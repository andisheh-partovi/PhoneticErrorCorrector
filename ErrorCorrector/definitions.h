#ifndef DEFINITIONS
#define DEFINITIONS

#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QHash>

//This class holds constants and definitions used throughout the project

//helper data types
typedef QHash < QString, QStringList> StringDictionary;
typedef QMap < QString, QStringList> String2StringListMap;
typedef QMap <QString, double> String2DoubleMap;
typedef QMap <QString, QString> String2StringMap;
typedef QList <String2DoubleMap>  SynonymDictionary;

//system constants
#define EPSILON 0.0001
#define pr_class_replaceword_obj 0.436
#define pr_class_replaceword_land 0.173


#endif // DEFINITIONS

