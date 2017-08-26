#include <label.h>
#include <QDebug>

label::label(){
    labelCounter = 0;
}

label::~label(){

}

void label::addLabel(const QString & name, unsigned int addr){
    nameList.append(name);
    addrList.push_back(addr);
}

unsigned int label::getAddr(const QString & name){
    nameIter = nameList.constBegin();
    addrIter = addrList.begin();
    while(nameIter != nameList.constEnd()){
        if(name == *nameIter)
            break;
        ++nameIter;
        ++addrIter;
    }
    if(nameIter == nameList.constEnd())
        return 0;
    else
        return *addrIter;
}


QString label::getName(const unsigned int pc){
    QString result;
    nameIter = nameList.constBegin();
    addrIter = addrList.begin();
    while(addrIter != addrList.end()){
        if(pc == *addrIter)
            break;
        ++nameIter;
        ++addrIter;
    }
    if(addrIter != addrList.end())
        result.append(*nameIter);

    return result;
}

void label::clear(){
    int sizeName = nameList.size();
    int sizeAddr = addrList.size();
    for(int i = 0; i < sizeName; i++)
        nameList.removeAt(0);
    nameIter = nameList.constBegin();
    for(int i = 0; i < sizeAddr ; i++)
        addrList.pop_back();
    addrIter = addrList.begin();
    labelCounter = 0;
}

 QString label::autoLabelName(){
    QString result, temp;
    result.append("L");
    temp = temp.setNum(labelCounter, 10);
    result.append(temp);
    labelCounter ++;
    return result;
 }

QString label::checkLabel(unsigned int pc){
    QString result;
    qDebug()<<labelCounter;
    for(int i=0; i<labelCounter; i++){

        if(addrList.at(i) == pc)
            result.append(nameList.at(i)).append(":\n");
    }

    return result;
}
