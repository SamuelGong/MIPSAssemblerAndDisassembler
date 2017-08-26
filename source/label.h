#ifndef LABEL_H
#define LABEL_H

#include <vector>
#include <QString>
#include <QStringList>

using namespace std;

class label{
public:

    label();
    ~label();
    void addLabel(const QString & name, unsigned int addr);
    void clear();
    unsigned int getAddr(const QString & name);
    QString getName(const unsigned int pc);
    QString autoLabelName();
    QString checkLabel(unsigned int pc);

private:
    int labelCounter;
    QStringList::const_iterator nameIter;
    QStringList nameList;
    vector<unsigned int> addrList;
    vector<unsigned int>::iterator addrIter;
};


#endif // LABEL_H
