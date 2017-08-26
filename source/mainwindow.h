#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QTextEdit>
#include <QFontComboBox>
#include <QComboBox>
#include <QDebug>
#include "assemble.h"
#include "disassemble.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    friend void Assemble::getTextEdit1(MainWindow & w);
    friend void Assemble::changeTextEdit2(MainWindow & w, int cardinal);
    friend void Disassemble::getTextEdit1(MainWindow & w);
    friend void Disassemble::changeTextEdit2(MainWindow & w);
    friend void Assemble::generateHexString(MainWindow & w);

private slots:
    void showFontComboBox(QString comboStr);
    void showFontComboBox2(QString comboStr);
    void showSizeSpinBox(QString spinValue);
    void showSizeSpinBox2(QString spinValue);
    void showCurrentFormatChanged(const QTextCharFormat &fmt);
    void showCurrentFormatChanged2(const QTextCharFormat &fmt);
    void showColorBtn();
    void showColorBtn2();

private:
    void open();
    void openBin();
    void save();
    void toBin();
    void toCoe();
    void coeToAsm();
    void mergeFormat(QTextCharFormat format);
    void mergeFormat2(QTextCharFormat format);
    QString toHexString(QString instruction);
    Assemble as;
    Disassemble disas;
    QTextEdit *textEdit1;
    QTextEdit *textEdit2;
    QFontComboBox *fontComboBox;
    QComboBox *sizeComboBox;
    QFontComboBox *fontComboBox2;
    QComboBox *sizeComboBox2;
    QStringList hexStringList;
    QString displayHexString;

};
#endif // MAINWINDOW_H
