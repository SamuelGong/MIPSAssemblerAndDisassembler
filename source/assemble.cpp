#include "mainwindow.h"
#include <QRegExp>

Assemble::Assemble(){
}

Assemble::~Assemble(){

}

void Assemble::changeTextEdit2(MainWindow & w, int cardinal){
    if(cardinal == 2){
        w.textEdit2->setText(displayBinString);
    }

    else
        w.textEdit2->setText(displayHexString);
}

void Assemble::getTextEdit1(MainWindow & w){
    inString = w.textEdit1->toPlainText();
}

void Assemble::generateHexString(MainWindow & w){
    QString temp;
    QStringList sourceBin;
    QStringList::const_iterator iter_b;
    QStringList::const_iterator iter_h;

    w.hexStringList.clear();
    sourceBin = displayBinString.split('\n');
    for(iter_b = sourceBin.constBegin(); iter_b != sourceBin.constEnd(); ++iter_b){
        temp = *iter_b;
        if(temp.length() == 0)
            continue;
        temp = toHexString(temp);
        w.hexStringList << temp;
    }

    displayHexString.remove(0, displayHexString.length());
    for(iter_h =w.hexStringList.constBegin(); iter_h != w.hexStringList.constEnd(); ++iter_h){
        if(iter_h != w.hexStringList.constBegin())
            displayHexString.append(",\n");
        displayHexString.append(*iter_h);
    }
    displayHexString.append(";");
}

QString Assemble::preprocess(){
    QStringList after;
    QStringList::const_iterator iter_a;

    after = inString.split(QRegExp("#[^\n]*"));                               //delete commment
    inString = inString.remove(0, inString.length());
    for(iter_a = after.constBegin(); iter_a != after.constEnd(); ++iter_a)
        inString.append(*iter_a);

    int instCount = 0;
    int flag = 0;
    QString op;
    QStringList segment;
    QStringList::const_iterator seg_Iter;
    inString.prepend("prefix ");
    segment = inString.split(QRegExp("[ ,;\n\t]"));
    seg_Iter = segment.constBegin();

    while(++seg_Iter != segment.constEnd()){                  // process text label
        if(*seg_Iter == ".data")
            flag = 1;
        else if(*seg_Iter == ".text")
            flag = 0;
        else if(flag == 1)
            continue;
        else{
            op = *seg_Iter;
            if(op == "eret" || op == "syscall" || op == "break" || op == "nop" ||
               op == "add" || op == "addu"  || op == "sub"   || op == "subu"  ||
               op == "and" || op == "or"    || op == "xor"   || op == "nor"   ||
               op == "slt" || op == "sltu"  || op == "sll"   || op == "srl"   ||
               op == "sra" || op == "sllv"  || op == "srlv"  || op == "srav"  ||
               op == "jr"  || op == "div"   || op == "divu"  || op == "mult"  ||
               op == "multu"|| op == "jalr" || op == "mtlo"  || op == "mthi"  ||
               op == "mfhi" || op == "mflo" || op == "mfc0"  || op == "mtc0"  ||
               op == "addi" || op == "addiu" || op == "andi" || op == "ori"   ||
               op == "xori" || op == "lui"   || op == "lw"   || op == "sw"    ||
               op == "beq"  || op == "bne"   || op == "slti" || op == "sltiu" ||
               op == "lb"   || op == "lbu"   || op == "lh"   || op == "lhu"   ||
               op == "sb"   || op == "sh"    || op == "blez" || op == "bltz"  ||
               op == "bgez" || op == "bgtz"  || op == "j"    || op == "jal"   ||
               op == "move" || op == "nop")
                instCount ++;
            else if(op == "blt" || op == "ble" || op == "bgt" || op == "bge" || op == "la" || op == "li"    )
                instCount += 2;
            else if(op.right(1) == ":")
                labelLib.addLabel(op.left(op.length() - 1),instCount*4);

        }
        qDebug() << *seg_Iter << flag << instCount << endl;
    }


    QString dataString;                                      // process data label
    QString psuedoOp;
    QString temp;
    int *wordNum;
    wordNum = new int;
    flag = 0;
    seg_Iter = segment.constBegin();

    while(++seg_Iter != segment.constEnd()){
        if(*seg_Iter == ".text")
            flag = 0;
        else if(*seg_Iter == ".data")
            flag = 1;
        else if(flag == 0)
            continue;
        else{
            psuedoOp = *seg_Iter;
            if(psuedoOp.right(1) == ":"){
                labelLib.addLabel(psuedoOp.left(psuedoOp.length() - 1), instCount*4);
                seg_Iter++;
                if(*seg_Iter == ".asciiz"){
                    ++seg_Iter;
                    temp = *seg_Iter;
                    *wordNum = 0;
                    dataString.append(asciiData(temp, wordNum));
                    instCount += *wordNum;
                }
                else if(*seg_Iter == ".word"){
                    *wordNum = 0;
                    do{
                        temp = *(seg_Iter+1);
                        if(temp.length() == 0 || (temp.left(1) <= "9" && temp.left(1) >= "0") )
                            dataString.append(wordData(temp, wordNum)).append("\n");
                        else
                            break;
                        ++seg_Iter;
                    }while((seg_Iter + 1) != segment.constEnd());
                    instCount += *wordNum;
                }
                else
                    ;

            }
        }
    }

    delete wordNum;
    return dataString;
}

QString Assemble::asciiData(QString String, int * wordNum){
    String = String.left(String.length() - 1);
    String = String.right(String.length() - 1);

    QChar character;
    QStringList med;
    QString temp = "temp";
    QString result;

    int iter = 0;
    int end = String.length();
    while(iter != end){
        character = String.at(iter);
        if(character == '\\') {
            iter ++;
            character = String.at(iter);
            if(character == 'n'){
                med << ("00001010");
            }
            else if(character == 't'){
                med << ("00001001");
            }
        }
        else{
            temp = temp.setNum((int)(character.toLatin1()), 10);
            temp = zeroExtent(temp, 8);
            med << temp;
        }

        iter ++;
    }

    // aligning

    int byteNum = 0;
    temp.remove(0,temp.length());
    QStringList::const_iterator iter2;
    for(iter2 = med.constBegin(); iter2 != med.constEnd(); ++iter2){
        temp.prepend(*iter2);
        byteNum++;
        if(byteNum % 4 == 0){
            *wordNum = *wordNum + 1;
            result.append(temp).append('\n');
            temp.remove(0, temp.length());
        }
    }
    if(byteNum % 4 == 0){
        *wordNum = *wordNum + 1;
        result.append("00000000000000000000000000000000\n");
    }

    if(byteNum % 4 != 0){
        *wordNum = *wordNum + 1;
        for(int i = 0; i < 4 - byteNum % 4; i++)
            temp.prepend("00000000");
        result.append(temp).append('\n');
    }

    return result;
}


QString Assemble::wordData(QString String, int * wordNum){
    QString result;
    if(String.length() == 0)
        return result;

    bool ok;
    int med;
    QChar fill;
    *wordNum ++;
    if(String.left(2) == "0x" || String.left(2) == "0X"){
        fill = '0';
        result = result.setNum(String.toInt(&ok, 16), 2);
    }
    else{
        med = String.toInt(&ok, 10);
        result = result.setNum(med, 2);
        if(med >= 0)
            fill = '0';
        else
            fill = '1';
    }

    int length = result.length();
    int diff = 32 - length;
    if(diff >= 0)
        for (int i=0; i<diff; i++)
            result.prepend(fill);
    else{
        result = result.right(32);
    }

    return result;
}

void Assemble::main(MainWindow & w){
    QString op;
    QString dataString;
    programCounter = 0;

    displayBinString.remove(0, displayBinString.length());
    getTextEdit1(w);

    labelLib.clear();
    dataString = preprocess();

    instructions = inString.split(QRegExp("[ ,:;\n\t]"));
    insIter = instructions.constBegin();


    while(++insIter != instructions.constEnd()){

        getOpcode(instructions, insIter, op);

        if(insIter == instructions.constEnd())
            break;

        switch(getOpType(op)){
        case O_Type:
            O_TypeProcess(op);
             programCounter += 4;
            break;
        case R_Type:
            R_TypeProcess(op);
            programCounter += 4;
            break;
        case I_Type:
            I_TypeProcess(op);
            programCounter += 4;
            break;
        case J_Type:
            J_TypeProcess(op);
            programCounter += 4;
            break;
        case P_Type:
            P_TypeProcess(op);
            break;
        default:
            break;
        }
    }
    displayBinString = displayBinString.append(dataString);
    generateHexString(w);
}

void Assemble::displayBin(MainWindow & w){
    changeTextEdit2(w, 2);
}

void Assemble::displayHex(MainWindow & w){
    changeTextEdit2(w, 16);
}

QString Assemble::toHexString(QString instruction){
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

void Assemble::O_TypeProcess(const QString & op){
    QString temp;
    if (op == "nop")
        displayBinString.append("00000000000000000000000000000000").append("\n");
    else if(op == "eret")
        displayBinString.append("01000010000000000000000000011000").append("\n");
    else if(op == "syscall")
        displayBinString.append("00000000000000000000000000001100").append("\n");
    else if(op == "break"){
        temp = getNextString();
        temp = zeroExtent(temp, 20);
        displayBinString.append("000000").append(temp).append("001101").append("\n");
    }
}

void Assemble::R_TypeProcess(const QString & op){
    QString opCode, dReg, sReg, tReg, shAmt, func;
    QString temp;
    opCode = "000000";

    // op rd rs rt
    if(op == "add" || op == "addu"  || op == "sub"   || op == "subu"  ||
       op == "and" || op == "or"    || op == "xor"   || op == "nor"   ||
       op == "slt" || op == "sltu"  )
    {
        temp = getNextString();
        dReg = registerCode(temp);

        temp = getNextString();
        sReg = registerCode(temp);

        temp = getNextString();
        tReg = registerCode(temp);

        shAmt = "00000";

        if(op == "add")
            func = "100000";
        else if(op == "addu")
            func = "100001";
        else if(op == "sub")
            func = "100010";
        else if(op == "subu")
            func = "100011";
        else if(op == "and")
            func = "100100";
        else if(op == "or")
            func = "100101";
        else if(op == "xor")
            func = "100110";
        else if(op == "nor")
            func = "100111";
        else if(op == "slt")
            func = "101010";
        else if(op == "sltu")
            func = "101011";
        else
            ;
    }

    // op rd rt rs
    else if(op == "sllv"  || op == "srlv"  ||  op == "srav"){
        temp = getNextString();
        dReg = registerCode(temp);

        temp = getNextString();
        tReg = registerCode(temp);

        temp = getNextString();
        sReg = registerCode(temp);

        shAmt = "00000";

        if(op == "sllv")
            func = "000100";
        else if(op == "srlv")
            func = "000110";
        else if(op == "srav")
            func = "000111";
        else
            ;
    }

    // op rs rt
    else if(op == "mult" || op == "multu" || op == "div" || op == "divu"){
        temp = getNextString();
        sReg = registerCode(temp);

        temp = getNextString();
        tReg = registerCode(temp);

        dReg = "00000";

        shAmt = "00000";

        if(op == "mult")
            func = "011000";
        else if(op == "multu")
            func = "011001";
        else if(op == "div")
            func = "011010";
        else if(op == "divu")
            func = "011011";
        else
            ;

    }

    // op rs rd
    else if(op == "jalr"){
        temp = getNextString();
        sReg = registerCode(temp);

        temp = getNextString();
        dReg = registerCode(temp);

        shAmt = "00000";
        tReg = "00000";

        func = "001001";
    }

    // op rd rt shamt
    else if(op == "sll" || op == "srl" || op == "sra"){
        sReg = "00000";
        temp = getNextString();
        dReg = registerCode(temp);
        temp = getNextString();
        tReg = registerCode(temp);

        shAmt = getNextString();
        shAmt = zeroExtent(shAmt, 5);

        if(op == "sll")
            func = "000000";
        else if(op == "sra")
            func = "000011";
        else if(op == "srl")
            func = "000010";
        else
            ;
    }

    // op rs
    else if(op == "jr" || op == "mthi" || op == "mtlo"){
        temp = getNextString();
        sReg = registerCode(temp);
        tReg = "00000";
        dReg = "00000";
        shAmt = "00000";

        if(op == "mthi")
            func = "010001";
        else if(op == "mtlo")
            func = "010011";
        else if(op == "jr")
            func = "001000";
        else
            ;
    }

    // op rd
    else if(op == "mfhi" || op == "mflo")
    {
        temp = getNextString();
        dReg = registerCode(temp);
        sReg = "00000";
        tReg = "00000";
        shAmt = "00000";

        if(op == "mfhi")
            func = "010000";
        else if(op == "mflo")
            func = "010010";
        else
            ;
    }

    displayBinString.append(opCode).append(sReg).append(tReg).append(dReg).append(shAmt).append(func).append('\n');
}

void Assemble::I_TypeProcess(const QString & op){
    QString opCode, sReg, tReg, imme;
    QString temp;
    QStringList sr_imme;
    int addr;

    // op rs rt imme
    if(op == "beq" || op == "bne"){
        temp = getNextString();
        sReg = registerCode(temp);

        temp = getNextString();
        tReg = registerCode(temp);

        temp = getNextString();
        addr = labelLib.getAddr(temp);
        addr = (addr - (programCounter + 4)) / 4;
        temp = temp.setNum(addr, 10);
        imme = signExtent(temp, 16);

        if(op == "beq")
            opCode = "000100";
        else if(op == "bne")
            opCode = "000101";
        else
            ;
    }

    // op rt ts imme
    else if(op == "addi" || op == "addiu" || op == "andi" || op == "ori" ||
            op == "xori" || op == "slti"  || op == "slti" || op == "sltiu"){
        temp = getNextString();
        tReg = registerCode(temp);

        temp = getNextString();
        sReg = registerCode(temp);

        temp = getNextString();

        if(op == "addi" || op == "slti" || op == "sltiu"|| op == "addiu")
            imme = signExtent(temp, 16);
        else
            imme = zeroExtent(temp, 16);

        if(op == "addi")
            opCode = "001000";
        else if(op == "addiu")
            opCode = "001001";
        else if(op == "andi")
            opCode = "001100";
        else if(op == "ori")
            opCode = "001101";
        else if(op == "xori")
            opCode = "001110";
        else if(op == "slti")
            opCode = "001010";
        else if(op == "sltiu")
            opCode = "001011";
        else
            ;
    }

    // op rt imme(rs)
    else if(op == "lw" || op == "sw" || op == "lb" || op == "lbu" || op == "lh" ||
            op == "lhu" || op == "sb" || op == "sb" || op == "sh"){
        temp = getNextString();
        tReg = registerCode(temp);

        temp = getNextString();
        sr_imme = temp.split(QRegExp("[()]"));

        imme = signExtent(sr_imme.at(0), 16);
        sReg = registerCode(sr_imme.at(1));

        if(op == "lw")
            opCode = "100011";
        else if(op == "sw")
            opCode = "101011";
        else if(op == "lb")
            opCode = "100000";
        else if(op == "lbu")
            opCode = "100100";
        else if(op == "lh")
            opCode = "100001";
        else if(op == "lhu")
            opCode = "100101";
        else if(op == "sb")
            opCode = "101000";
        else if(op == "sh")
            opCode = "101001";
        else
            ;
    }

    // op rt imme
    else if(op == "lui"){
        temp = getNextString();
        tReg = registerCode(temp);

        sReg = "00000";

        temp = getNextString();
        imme = signExtent(temp, 16);

        opCode = "001111";
    }

    // op rs imme
    else if(op == "blez" || op == "bltz" || op == "bgtz" || op == "bgez"){
        temp = getNextString();
        sReg = registerCode(temp);

        temp = getNextString();
        addr = labelLib.getAddr(temp);
        addr = (addr - (programCounter + 4)) / 4;
        temp = temp.setNum(addr, 10);
        imme = signExtent(temp, 16);

        if(op == "bgez"){
            opCode = "000001";
            tReg = "00001";
        }
        else if(op == "bgtz"){
            opCode = "000111";
            tReg = "00000";
        }
        else if(op == "blez"){
            opCode = "000110";
            tReg = "00000";
        }
        else if(op == "bltz"){
            opCode = "000001";
            tReg = "00000";
        }

        else
            ;
    }

    displayBinString.append(opCode).append(sReg).append(tReg).append(imme).append('\n');
}

void Assemble::J_TypeProcess(const QString & op){
    QString opCode, target;
    unsigned int temp;

    if(op == "j")
        opCode = "000010";
    else if(op == "jal")
        opCode = "000011";
    else
        ;

    target = getNextString();
    temp = labelLib.getAddr(target);
    temp = temp / 4;
    target = target.setNum(temp, 10);
    target = signExtent(target, 26);

    displayBinString.append(opCode).append(target).append('\n');
}

QString Assemble::registerCode(const QString & r){
    if(r == "$0" || r == "$zero")
        return "00000";
    if(r == "$1" || r == "$at")
        return "00001";
    if(r == "$2" || r == "$v0")
        return "00010";
    if(r == "$3" || r == "$v1")
        return "00011";
    if(r == "$4" || r == "$a0")
        return "00100";
    if(r == "$5" || r == "$a1")
        return "00101";
    if(r == "$6" || r == "$a2")
        return "00110";
    if(r == "$7" || r == "$a3")
        return "00111";
    if(r == "$8" || r == "$t0")
        return "01000";
    if(r == "$9" || r == "$t1")
        return "01001";
    if(r == "$10" || r == "$t2")
        return "01010";
    if(r == "$11" || r == "$t3")
        return "01011";
    if(r == "$12" || r == "$t4")
        return "01100";
    if(r == "$13" || r == "$t5")
        return "01101";
    if(r == "$14" || r == "$t6")
        return "01110";
    if(r == "$15" || r == "$t7")
        return "01111";

    if(r == "$16" || r == "$s0")
        return "10000";
    if(r == "$17" || r == "$s1")
        return "10001";
    if(r == "$18" || r == "$s2")
        return "10010";
    if(r == "$19" || r == "$s3")
        return "10011";
    if(r == "$20" || r == "$s4")
        return "10100";
    if(r == "$21" || r == "$s5")
        return "10101";
    if(r == "$22" || r == "$s6")
        return "10110";
    if(r == "$23" || r == "$s7")
        return "10111";
    if(r == "$24" || r == "$t8")
        return "11000";
    if(r == "$25" || r == "$t9")
        return "11001";
    if(r == "$26" || r == "$k0")
        return "11010";
    if(r == "$27" || r == "$k1")
        return "11011";
    if(r == "$28" || r == "$gp")
        return "11100";
    if(r == "$29" || r == "$sp")
        return "11101";
    if(r == "$30" || r == "$fp")
        return "11110";
    else
        return "11111";
}

int Assemble::getOpType(const QString & op){
    if(op == "eret" || op == "syscall" || op == "break" || op == "nop" )
        return O_Type;
    else if(op == "add" || op == "addu"  || op == "sub"   || op == "subu"  ||
       op == "and" || op == "or"    || op == "xor"   || op == "nor"   ||
       op == "slt" || op == "sltu"  || op == "sll"   || op == "srl"   ||
       op == "sra" || op == "sllv"  || op == "srlv"  || op == "srav"  ||
       op == "jr"  || op == "div"   || op == "divu"  || op == "mult"  ||
       op == "multu"|| op == "jalr" || op == "mtlo"  || op == "mthi"  ||
       op == "mfhi" || op == "mflo" )
        return R_Type;

    else if(op == "addi" || op == "addiu" || op == "andi" || op == "ori" ||
            op == "xori" || op == "lui"   || op == "lw"   || op == "sw"  ||
            op == "beq"  || op == "bne"   || op == "slti" || op == "sltiu" ||
            op == "lb"   || op == "lbu"   || op == "lh"   || op == "lhu"   ||
            op == "sb"   || op == "sh"    || op == "blez" || op == "bltz"  ||
            op == "bgez" || op == "bgtz")
        return I_Type;
    else if(op == "j" || op == "jal")
        return J_Type;
    else
        return P_Type;
}

QString Assemble::zeroExtent(QString num, int target){
    bool ok;
    QString result;

    if(num.left(2) == "0x" || num.left(2) == "0X"){
        num = num.setNum(num.toInt(&ok, 16), 2);
    }
    else{
        num = num.setNum(num.toInt(&ok, 10), 2);
    }

    int length = num.length();
    int diff = target - length;
    for (int i=0; i<diff; i++){
        result.append('0');
    }
    return result.append(num);
}

QString Assemble::signExtent(QString num, int target){
    bool ok;
    int med;
    char fill;
    QString result;

    if(num.left(2) == "0x" || num.left(2) == "0X"){
        fill = '0';
        num = num.setNum(num.toInt(&ok, 16), 2);
    }
    else{
        med = num.toInt(&ok, 10);
        if(med >= 0){
            fill = '0';
            num = num.setNum(med, 2);
        }
        else{
            fill = '1';
            num = num.setNum(med, 2);
        }
    }

    int length = num.length();
    int diff = target - length;
    if(diff >= 0){
        for (int i=0; i<diff; i++){
            result.append(fill);
        }
        result = result.append(num);
    }
    else{
        result = num.right(target);
    }
    return result;
}

QString Assemble::getNextString(){
    ++insIter;
    QString temp = *insIter;
    while(temp.length() == 0){
        ++insIter;
        temp = *insIter;
    }
    return temp;
}

void Assemble::getOpcode(QStringList & instructions, QStringList::const_iterator & insIter, QString & op){

    op = *insIter;

    while(op.length() == 0){
        insIter++;
        if(insIter == instructions.constEnd())
            break;
        op = *insIter;
    }
}

void Assemble::P_TypeProcess(const QString & op){
    int addr;
    QString opCode, dReg, tReg, sReg, imme, shAmt, func;
    QString temp;
    if(op == "blt" || op == "ble" || op == "bgt" || op == "bge"){
        psuedoCompare(op);
    }
    else if(op == "la"){            // la $at addr = lui $rt addr[high] , ori $rt, $at, addr[low]
        opCode = "001111";
        tReg = "00001";
        sReg = "00000";

        displayBinString.append(opCode).append(sReg).append(tReg);
        temp = getNextString();
        tReg = registerCode(temp);

        temp = getNextString();
        addr = labelLib.getAddr(temp);
        temp = temp.setNum(addr/65536, 10);
        imme = zeroExtent(temp, 16);

        displayBinString.append(imme).append('\n');
        programCounter += 4;

        opCode = "001101";
        sReg = "00001";
        temp = temp.setNum(addr%65536, 10);
        imme = zeroExtent(temp, 16);

        displayBinString.append(opCode).append(sReg).append(tReg).append(imme).append('\n');
        programCounter += 4;
    }
    else if(op == "move"){
        opCode = "000000";
        temp = getNextString();
        dReg = registerCode(temp);
        temp = getNextString();
        sReg = registerCode(temp);
        tReg = "00000";
        shAmt = "00000";
        func = "100101";
        displayBinString.append(opCode).append(sReg).append(tReg).append(dReg).append(shAmt).append(func).append('\n');
        programCounter += 4;
    }
    else if(op == "li"){ // li $rt imme = lui $at uppper;  ori $rt $at lower
        opCode = "001111";
        tReg = "00001";
        sReg = "00000";

        displayBinString.append(opCode).append(sReg).append(tReg);
        temp = getNextString();
        tReg = registerCode(temp);

        temp = getNextString();
        imme = zeroExtent(temp, 32);

        displayBinString.append(imme.left(16)).append('\n');
        programCounter += 4;

        opCode = "001101";
        sReg = "00001";

        displayBinString.append(opCode).append(sReg).append(tReg).append(imme.right(16)).append('\n');
        programCounter += 4;
        /*opCode = "001101";
        temp = getNextString();
        tReg = registerCode(temp);
        sReg = "00000";
        temp = getNextString();
        imme = zeroExtent(temp, 16);

        displayBinString.append(opCode).append(sReg).append(tReg).append(imme).append('\n');
        programCounter += 4;*/
    }
    else
        ;
}

void Assemble::psuedoCompare(const QString & op){
    QString opCode, sReg, tReg, dReg, shAmt, func;               // the first inst is R type
    QString temp;
    opCode = "000000";
    dReg = "00001";
    if(op == "bge" || "blt"){
        temp = getNextString();
        sReg = registerCode(temp);

        temp = getNextString();
        tReg = registerCode(temp);
    }
    else{
        temp = getNextString();
        tReg = registerCode(temp);

        temp = getNextString();
        sReg = registerCode(temp);
    }
    shAmt = "00000";
    func = "101010";
    displayBinString.append(opCode).append(sReg).append(tReg).append(dReg).append(shAmt).append(func).append('\n');

    programCounter += 4;
    QString opCode2, sReg2, tReg2, imme;                     // the second inst is I type
    int addr;
    sReg2 = "00001";
    tReg2 = "00000";
    if(op == "bge" || op == "ble"){
        opCode2 = "000100";
    }
    else opCode2 = "000101";

    temp = getNextString();
    addr = labelLib.getAddr(temp);
    addr = (addr - (programCounter + 4)) / 4;
    temp = temp.setNum(addr, 10);
    imme = signExtent(temp, 16);

    displayBinString.append(opCode2).append(sReg2).append(tReg2).append(imme).append('\n');
    programCounter += 4;
}

void Assemble::labelProcess(const QString & name){
    labelLib.addLabel(name, programCounter);
}
