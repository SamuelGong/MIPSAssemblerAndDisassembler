#ifndef ASSEMBLE_H
#define ASSEMBLE_H

#include <QString>
#include <label.h>

class MainWindow;

class Assemble{

public:
    Assemble();
    ~Assemble();
    void main(MainWindow & w);
    void displayBin(MainWindow & w);
    void displayHex(MainWindow & w);
    void getTextEdit1(MainWindow & w);
    void changeTextEdit2(MainWindow & w, int cardinal);
    void generateHexString(MainWindow & w);
    void labelProcess(const QString & lable);

private:
    label labelLib;

    QString preprocess();
    QString asciiData(QString String, int * asciiLength);
    QString wordData(QString String, int * wordNum);

    int getOpType(const QString & op);
    void O_TypeProcess(const QString & op);
    void R_TypeProcess(const QString & op);
    void I_TypeProcess(const QString & op);
    void J_TypeProcess(const QString & op);
    void P_TypeProcess(const QString & op);

    void getOpcode(QStringList & instructions, QStringList::const_iterator & insIter, QString & op);

    void psuedoCompare(const QString & op);

    QString registerCode(const QString & r);
    QString zeroExtent(QString num, int target);
    QString signExtent(QString num, int target);
    QString toHexString(QString instruction);
    QString getNextString();

    unsigned int programCounter;
    QString inString;
    QString displayBinString;
    QString displayHexString;
    QStringList instructions;
    QStringList::const_iterator insIter;
};

enum OpType_E{
    O_Type,
    R_Type,
    I_Type,
    J_Type,
    P_Type
};


#endif // ASSEMBLE_H
