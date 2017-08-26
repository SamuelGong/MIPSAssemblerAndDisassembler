#ifndef DISASSEMBLE_H
#define DISASSEMBLE_H
#include <QString>
#include "label.h"

class MainWindow;

class Disassemble{
public:
    Disassemble();
    ~Disassemble();
    void getTextEdit1(MainWindow & w);
    void changeTextEdit2(MainWindow & w);
    void coeMain(MainWindow & w);
private:
    label labelLib;
    QString getBinString(QString hexString);
    QString inString;
    QStringList instructions;
    QStringList::const_iterator insIter;
    QString displayAsm;
    QString main(QString inst, unsigned int pc);
    void displayResult(MainWindow & w);
    void preprocess();
    QString getRegName(int n);
    QString signExtent(QString num, int target);
    QString zeroExtent(QString num, int target);

};

#endif // DISASSEMBLE_H
