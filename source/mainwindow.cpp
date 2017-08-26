#include <fstream>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QDataStream>
#include <QToolButton>
#include <QAction>
#include <QMenu>
#include <QLabel>
#include <QMenuBar>
#include <QToolBar>
#include <QByteArray>
#include <QFontComboBox>
#include <QComboBox>
#include <QColorDialog>
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle(tr("JZF MIPS Assembler"));

    QAction *openAction = new QAction(QIcon(":/images/doc-open"), tr("&Open asm or coe"), this);
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip(tr("Open an existing asm or coe file"));

    QAction *binOpenAction = new QAction(QIcon(":/images/bin-open"), tr("Open &bin"), this);
    binOpenAction->setShortcut(QKeySequence::NativeText);
    binOpenAction->setStatusTip(tr("Open an existing bin file and change it into visible form"));

    QAction *saveAction = new QAction(QIcon(":/images/doc-save"), tr("&Save..."), this);
    saveAction->setShortcut(QKeySequence::Save);;
    saveAction->setStatusTip(tr("Save the modified file in the left column"));

    QAction *toBinAction = new QAction(QIcon(":/images/to-bin"), tr("To Bin"), this);
    toBinAction->setShortcut(QKeySequence::Forward);
    toBinAction->setStatusTip(tr("assemble and save bin file"));

    QAction *toCoeAction = new QAction(QIcon(":/images/to-coe"), tr("To Coe"), this);
    toCoeAction->setShortcut((QKeySequence::MoveToNextWord));
    toCoeAction->setStatusTip(tr("assemble and save coe file"));

    QAction *coeToAsmAction = new QAction(QIcon(":/images/coe-to-asm"), tr("Coe To Asm"), this);
    coeToAsmAction->setShortcut((QKeySequence::MoveToPreviousWord));
    coeToAsmAction->setStatusTip(tr("disassemble from coe or bin file"));

    fontComboBox = new QFontComboBox;
    fontComboBox->setFontFilters(QFontComboBox::AllFonts);
    sizeComboBox = new QComboBox;

    fontComboBox2 = new QFontComboBox;
    fontComboBox2->setFontFilters(QFontComboBox::AllFonts);
    sizeComboBox2 = new QComboBox;

    QLabel *fontLabel1 = new QLabel(tr("Font:"));
    QLabel *fontLabel2 = new QLabel(tr(" Size:"));
    QFontDatabase db;
    foreach (int size, db.standardSizes())
        sizeComboBox->addItem (QString::number (size));

    QLabel *fontLabel3 = new QLabel(tr("Font:"));
    QLabel *fontLabel4 = new QLabel(tr(" Size:"));
    QFontDatabase db2;
    foreach (int size2, db2.standardSizes())
        sizeComboBox2->addItem (QString::number (size2));

    QToolButton * colorBtn = new QToolButton;
    colorBtn->setIcon(QIcon(":/images/color"));
    colorBtn->setCheckable(true);

    QToolButton * colorBtn2 = new QToolButton;
    colorBtn2->setIcon(QIcon(":/images/color"));
    colorBtn2->setCheckable(true);

    textEdit1 = new QTextEdit(this);
    textEdit2 = new QTextEdit(this);

    connect (openAction, &QAction::triggered, this, &MainWindow::open);
    connect (binOpenAction, &QAction::triggered, this, &MainWindow::openBin);
    connect (saveAction, &QAction::triggered, this, &MainWindow::save);
    connect (toBinAction, &QAction::triggered, this, &MainWindow::toBin);
    connect (toCoeAction, &QAction::triggered, this, &MainWindow::toCoe);
    connect (coeToAsmAction, &QAction::triggered, this, &MainWindow::coeToAsm);

    connect (fontComboBox, SIGNAL(activated(QString)), this, SLOT(showFontComboBox(QString)));
    connect (sizeComboBox, SIGNAL(activated(QString)), this, SLOT(showSizeSpinBox(QString)));
    connect (textEdit1, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
             this, SLOT(showCurrentFormatChanged(QTextCharFormat)));
    connect (colorBtn, SIGNAL(clicked(bool)), this , SLOT(showColorBtn()));

    connect (fontComboBox2, SIGNAL(activated(QString)), this, SLOT(showFontComboBox2(QString)));
    connect (sizeComboBox2, SIGNAL(activated(QString)), this, SLOT(showSizeSpinBox2(QString)));
    connect (textEdit2, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
             this, SLOT(showCurrentFormatChanged2(QTextCharFormat)));
    connect (colorBtn2, SIGNAL(clicked(bool)), this , SLOT(showColorBtn2()));

    QMenu *file = menuBar()->addMenu(tr("&File"));
    file->addAction(openAction);
    file->addAction(binOpenAction);
    file->addAction(saveAction);

    QMenu *assembleMenu = menuBar()->addMenu(tr("&Assemble"));
    assembleMenu->addAction(toBinAction);
    assembleMenu->addAction(toCoeAction);

    QMenu *disassembleMenu = menuBar()->addMenu(tr("&Disassemble"));
    disassembleMenu->addAction(coeToAsmAction);

    QToolBar *toolBar1 = addToolBar(tr("&File"));
    toolBar1->addAction(openAction);
    toolBar1->addAction(binOpenAction);
    toolBar1->addAction(saveAction);

    QToolBar *toolBar2 = addToolBar(tr("&Assemble"));
    toolBar2->addAction(toBinAction);
    toolBar2->addAction(toCoeAction);

    QToolBar *toolBar3 = addToolBar(tr("&Disassemble"));
    toolBar3->addAction(coeToAsmAction);

    QToolBar *toolBar4 = addToolBar(tr("&Font"));
    toolBar4->addWidget(fontLabel1);
    toolBar4->addWidget(fontComboBox);
    toolBar4->addWidget(fontLabel2);
    toolBar4->addWidget(sizeComboBox);
    toolBar4->addWidget(colorBtn);

    QToolBar *toolBar5 = addToolBar(tr("&Font2"));
    toolBar5->addWidget(fontLabel3);
    toolBar5->addWidget(fontComboBox2);
    toolBar5->addWidget(fontLabel4);
    toolBar5->addWidget(sizeComboBox2);
    toolBar5->addWidget(colorBtn2);

    QWidget *central = new QWidget(this);
    QHBoxLayout *middleLayout = new QHBoxLayout;
    middleLayout->addWidget(textEdit1);
    middleLayout->addWidget(textEdit2);

    QHBoxLayout *middleUpLayout = new QHBoxLayout;
    middleUpLayout->addWidget(toolBar4);
    middleUpLayout->addWidget(toolBar5);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addLayout(middleUpLayout);
    centralLayout->addLayout(middleLayout);
    central->setLayout(centralLayout);
    setCentralWidget(central);

    QTextCharFormat fmt;
    fmt.setForeground(Qt::blue);
    fmt.setFontPointSize(10.0);
    fmt.setFontFamily("Courier New");
    textEdit1->mergeCurrentCharFormat(fmt);

    fmt.setForeground(Qt::red);
    textEdit2->mergeCurrentCharFormat(fmt);

    statusBar();
}

MainWindow::~MainWindow()
{
}

void MainWindow::open(){
    QString path = QFileDialog::getOpenFileName(this,
                                                tr("Open File"),
                                                ".",
                                                tr("relative Files(*.asm *.coe)"));
    if(!path.isEmpty()){
        QFile file(path);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QMessageBox::warning(this, tr("Read File"),
                                  tr("Cannot open file:\n%1").arg(path));
            return;
        }
        QTextStream in(&file);

        textEdit1->setText(in.readAll());

        file.close();
    }
    else{
        QMessageBox::warning(this, tr("Path"),
                             tr("You did not select any file."));
    }
}

void MainWindow::openBin(){
    QString path = QFileDialog::getOpenFileName(this,
                                                tr("Open File"),
                                                ".",
                                                tr("binary Files(*.bin)"));
    if(!path.isEmpty()){
        QString temp, hexInst;
        char* pathInCpp;
        QByteArray ba = path.toLatin1();
        pathInCpp = ba.data();
        std::ifstream fin(pathInCpp, std::ios::binary);

        char num;
        int counter = 0;
        while(!fin.eof()){
            fin.read((char*)&num, sizeof(char));
            temp = temp.setNum(num, 2);
            if(temp.length() > 8)
                temp = temp.right(8);
            else
                while(temp.length() < 8)
                    temp.prepend("0");
            hexInst.append(temp);
            if(counter % 4 == 3){
                displayHexString.append(toHexString(hexInst)).append(";\n");
                hexInst.remove(0, hexInst.length());
            }

            counter++;
        }
        textEdit1->setText(displayHexString);

        fin.close();
    }
    else{
        QMessageBox::warning(this, tr("Path"),
                             tr("You did not select any file."));
    }
}

void MainWindow::save(){
    QString path = QFileDialog::getSaveFileName(this,
                                                tr("Save File"),
                                                ".",
                                                tr("Text Files(*.asm)"));
    if(!path.isEmpty()){
        QFile file(path);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
            QMessageBox::warning(this, tr("Write File"),
                                  tr("Cannot open file:\n%1").arg(path));
            return;
        }
        QTextStream out(&file);
        out << textEdit1->toPlainText();
        file.close();
    }
    else{
        QMessageBox::warning(this, tr("Path"),
                             tr("You did not select any file."));
    }
}

void MainWindow::toBin(){
    as.main(*this);
    as.displayBin(*this);
    QString path = QFileDialog::getSaveFileName(this,
                                                tr("Save File"),
                                                "/",
                                                tr("Binary Files(*.bin)"));
    if(!path.isEmpty()){
        QFile file(path);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
            QMessageBox::warning(this, tr("Write File"),
                                  tr("Cannot open file:\n%1").arg(path));
            return;
        }
        QDataStream out(&file);

        bool ok;
        QString temp;
        QStringList::const_iterator iter;

        for(iter = hexStringList.constBegin(); iter != hexStringList.constEnd(); ++iter){
            temp = *iter;
            out << (qint32)temp.toUInt(&ok, 16);
            file.flush();
        }

        file.close();
    }
    else{
        QMessageBox::warning(this, tr("Path"),
                             tr("You did not select any file."));
    }

}

void MainWindow::toCoe(){

    as.main(*this);
    as.displayHex(*this);
    QString path = QFileDialog::getSaveFileName(this,
                                                tr("Save File"),
                                                ".",
                                                tr("Coe Files(*.coe)"));
    if(!path.isEmpty()){
        QFile file(path);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
            QMessageBox::warning(this, tr("Write File"),
                                  tr("Cannot open file:\n%1").arg(path));
            return;
        }
        QTextStream out(&file);
        out << "memory_initialization_radix = 16;\n"
            << "memory_initialization_vector = \n" << textEdit2->toPlainText();
        file.close();
    }
    else{
        QMessageBox::warning(this, tr("Path"),
                             tr("You did not select any file."));
    }
}

void MainWindow::coeToAsm(){
    disas.coeMain(*this);
    QString path = QFileDialog::getSaveFileName(this,
                                                tr("Save File"),
                                                "/",
                                                tr("Assembly Files(*.asm)"));
    if(!path.isEmpty()){
        QFile file(path);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
            QMessageBox::warning(this, tr("Write File"),
                                  tr("Cannot open file:\n%1").arg(path));
            return;
        }
        QDataStream out(&file);
        out << textEdit1->toPlainText();
        file.close();
    }
    else{
        QMessageBox::warning(this, tr("Path"),
                             tr("You did not select any file."));
    }
}

void MainWindow::showFontComboBox(QString comboStr){
    QTextCharFormat fmt;
    fmt.setFontFamily(comboStr);
    mergeFormat(fmt);
}

void MainWindow::showFontComboBox2(QString comboStr){
    QTextCharFormat fmt;
    fmt.setFontFamily(comboStr);
    mergeFormat(fmt);
}

void MainWindow::mergeFormat(QTextCharFormat format){
    QTextCursor cursor = textEdit1->textCursor();
    if(!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    textEdit1->mergeCurrentCharFormat(format);
}

void MainWindow::mergeFormat2(QTextCharFormat format){
    QTextCursor cursor = textEdit2->textCursor();
    if(!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    textEdit2->mergeCurrentCharFormat(format);
}

void MainWindow::showSizeSpinBox(QString spinValue){
    QTextCharFormat fmt;
    fmt.setFontPointSize(spinValue.toFloat());
    textEdit1->mergeCurrentCharFormat(fmt);
}

void MainWindow::showSizeSpinBox2(QString spinValue){
    QTextCharFormat fmt;
    fmt.setFontPointSize(spinValue.toFloat());
    textEdit2->mergeCurrentCharFormat(fmt);
}

void MainWindow::showColorBtn (){
    QColor color = QColorDialog::getColor(Qt::red, this);
    if (color.isValid()){
        QTextCharFormat fmt;
        fmt.setForeground(color);
        textEdit1->mergeCurrentCharFormat(fmt);
    }
}

void MainWindow::showColorBtn2 (){
    QColor color = QColorDialog::getColor(Qt::red, this);
    if (color.isValid()){
        QTextCharFormat fmt;
        fmt.setForeground(color);
        textEdit2->mergeCurrentCharFormat(fmt);
    }
}

void MainWindow::showCurrentFormatChanged (const QTextCharFormat &fmt){
    fontComboBox->setCurrentIndex(fontComboBox->findText (fmt.fontFamily()));
    sizeComboBox->setCurrentIndex(sizeComboBox->findText (QString::number(fmt.fontPointSize ())));
}

void MainWindow::showCurrentFormatChanged2 (const QTextCharFormat &fmt){
    fontComboBox->setCurrentIndex(fontComboBox->findText (fmt.fontFamily()));
    sizeComboBox->setCurrentIndex(sizeComboBox->findText (QString::number(fmt.fontPointSize ())));
}

QString MainWindow::toHexString(QString instruction){
    bool ok;
    int iter;
    QString result;
    QString temp;
    for(iter = 0; iter <= instruction.length() - 4; iter += 4){
        temp = temp.setNum(instruction.mid(iter, 4).toInt(&ok, 2), 16);
        temp = temp.toUpper();
        result.append(temp);
    }
    return result;
}
