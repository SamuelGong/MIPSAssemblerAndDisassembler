#include "mainwindow.h"

Disassemble::Disassemble(){

}

Disassemble::~Disassemble(){

}

void Disassemble::coeMain(MainWindow & w){


    getTextEdit1(w);

    labelLib.clear();
    displayAsm.remove(0, displayAsm.length());

    preprocess();

    QString temp;
    unsigned int programCounter = 0;
    insIter = instructions.constBegin();

    while(insIter != instructions.constEnd()){

        temp = *insIter;
        if(temp.length() == 0)
            continue;

        displayAsm.append(labelLib.checkLabel(programCounter));
        displayAsm.append(main(temp, programCounter));
        programCounter += 4;
        ++insIter;
    }

    changeTextEdit2(w);
}

void Disassemble::preprocess(){
    bool ok;
    unsigned int programCounter = 0, labelAddr;
    QString opcode, func, tReg, addr;
    QString temp;
    insIter = instructions.constBegin();

    while(insIter != instructions.constEnd()){
        temp = *insIter;
        if(temp.length() == 0)
            continue;
        opcode = temp.left(6);
        func = temp.right(6);
        tReg = temp.left(16).right(5);
        if( opcode == "000100" || //beq
            opcode == "000101" || //bne
            opcode == "000110" || //blez
            opcode == "000111" || //bgtz
            (opcode == "000001" && tReg == "00000") || // bltz
            (opcode == "000001" && tReg == "00001") // bgez
            ){
            addr = temp.right(16);
            labelAddr = addr.toInt(&ok, 2);
            if(addr.left(1) == "1")
                labelAddr = -(65536 - labelAddr);
            labelAddr *= 4;
            labelAddr += (programCounter + 4);

            if(labelLib.getName(labelAddr).length() == 0)
                labelLib.addLabel(labelLib.autoLabelName(), labelAddr);
        }
        else if(opcode == "000010" || //j
                opcode == "000011"//jal
                ){
            addr = temp.right(26);
            addr.append("00");
            labelAddr = addr.toInt(&ok, 2);
            if(labelLib.getName(labelAddr).length() == 0)
                labelLib.addLabel(labelLib.autoLabelName(), labelAddr);
        }
        ++insIter;
        programCounter += 4;
    }
}


void Disassemble::getTextEdit1(MainWindow & w){

    int flag;
    QString temp;
    QStringList coeInst;
    QStringList::ConstIterator coeIter;
    instructions.clear();

   {
        inString = w.textEdit1->toPlainText();
        coeInst = inString.split(QRegExp("[ ,=;\n\t]"));
        coeIter = coeInst.constBegin();
        flag = -1;
        while(coeIter != coeInst.constEnd()){
            temp = *coeIter;
            if(temp.length() == 0){
                ++coeIter;
                continue;
            }

            if(flag == -1 && (temp.left(1) == "m" || temp.left(1) == "M"))
                flag = 1;
            else if(flag == -1 &&
                    (temp.left(1) == "0" || temp.left(1) == "1" || temp.left(1) == "2" || temp.left(1) == "3" ||
                     temp.left(1) == "4" || temp.left(1) == "5" || temp.left(1) == "6" || temp.left(1) == "7" ||
                     temp.left(1) == "8" || temp.left(1) == "9" || temp.left(1) == "a" || temp.left(1) == "b" ||
                     temp.left(1) == "c" || temp.left(1) == "d" || temp.left(1) == "e" || temp.left(1) == "f" ||
                     temp.left(1) == "A" || temp.left(1) == "B" ||
                     temp.left(1) == "C" || temp.left(1) == "D" ||
                     temp.left(1) == "E" || temp.left(1) == "F" )){
                    flag = 0;
                    instructions << getBinString(temp);
                    ++coeIter;
                    continue;
                }

            else if(flag == 1 && (temp.left(1) == "m" || temp.left(1) == "M")){
                    flag = 0;
                    ++coeIter;
                    continue;
                }
            else if(flag == 0)
                instructions << getBinString(temp);

            ++coeIter;
        }
    }
}

QString Disassemble::getBinString(QString hexString){
    bool ok;
    QString result;
    QString digit;
    int digit_num;
    for(int i = 1; i <= 8; i++){
        digit = hexString.left(i).right(1);
        digit_num = digit.toInt(&ok, 16);
        digit = digit.setNum(digit_num, 2);
        while(digit.length() < 4)
            digit.prepend("0");
        result.append(digit);
    }
    return result;
}

void Disassemble::changeTextEdit2(MainWindow & w){
     w.textEdit2->setText(displayAsm);
}

QString Disassemble::main(QString inst, unsigned int pc){
    bool ok;
    QString result;
    QString opcode = inst.left(6);

    if(inst == "00000000000000000000000000000000")
        result.append("nop");
    else if(inst.left(6) == "000000" && inst.right(6) == "001101"){
        int code_num;
        bool ok;
        QString code;
        code = inst.left(26).right(20);
        code_num = code.toInt(&ok, 2);
        code = code.setNum(code_num, 16);
        code.prepend("0x");

        result.append("break ").append(code);
    }
    else if(opcode == "000000"){ // R-type
        QString dReg, sReg, tReg, shAmt, func;
        int shAmt_num, dReg_num, sReg_num, tReg_num;

        func = inst.right(6);

        shAmt = inst.right(11).left(5);
        shAmt_num = shAmt.toInt(&ok, 2);
        shAmt = shAmt.setNum(shAmt_num, 10);

        dReg = inst.right(16).left(5);
        dReg_num = dReg.toInt(&ok, 2);
        dReg = getRegName(dReg_num);

        tReg = inst.left(16).right(5);
        tReg_num = tReg.toInt(&ok, 2);
        tReg = getRegName(tReg_num);

        sReg = inst.left(11).right(5);
        sReg_num = sReg.toInt(&ok, 2);
        sReg = getRegName(sReg_num);

        if(func == "000000")
            result.append("sll ").append(dReg).append(", ").append(tReg).append(", ").append(shAmt);
        else if(func == "000010")
            result.append("srl ").append(dReg).append(", ").append(tReg).append(", ").append(shAmt);
        else if(func == "000011")
            result.append("sra ").append(dReg).append(", ").append(tReg).append(", ").append(shAmt);
        else if(func == "000100")
            result.append("sllv ").append(dReg).append(", ").append(tReg).append(", ").append(sReg);
        else if(func == "000110")
            result.append("srlv ").append(dReg).append(", ").append(tReg).append(", ").append(sReg);
        else if(func == "000111")
            result.append("srav ").append(dReg).append(", ").append(tReg).append(", ").append(sReg);
        else if(func == "001000")
            result.append("jr ").append(sReg);
        else if(func == "001001")
            result.append("jalr ").append(sReg).append(", ").append(dReg);
        else if(func == "001100")
            result.append("syscall");
        else if(func == "001101")
            result.append("break");
        else if(func == "010000")
            result.append("mfhi ").append(dReg);
        else if(func == "010001")
            result.append("mthi ").append(sReg);
        else if(func == "010010")
            result.append("mflo ").append(dReg);
        else if(func == "010011")
            result.append("mtlo ").append(sReg);
        else if(func == "011000")
            result.append("mult ").append(sReg).append(", ").append(tReg);
        else if(func == "011001")
            result.append("multu ").append(sReg).append(", ").append(tReg);
        else if(func == "011010")
            result.append("div ").append(sReg).append(", ").append(tReg);
        else if(func == "011011")
            result.append("divu ").append(sReg).append(", ").append(tReg);
        else if(func == "100000")
            result.append("add ").append(dReg).append(", ").append(sReg).append(", ").append(tReg);
        else if(func == "100001")
            result.append("addu ").append(dReg).append(", ").append(sReg).append(", ").append(tReg);
        else if(func == "100010")
            result.append("sub ").append(dReg).append(", ").append(sReg).append(", ").append(tReg);
        else if(func == "100011")
            result.append("subu ").append(dReg).append(", ").append(sReg).append(", ").append(tReg);
        else if(func == "100100")
            result.append("and ").append(dReg).append(", ").append(sReg).append(", ").append(tReg);
        else if(func == "100101")
            result.append("or ").append(dReg).append(", ").append(sReg).append(", ").append(tReg);
        else if(func == "100110")
            result.append("xor ").append(dReg).append(", ").append(sReg).append(", ").append(tReg);
        else if(func == "100111")
            result.append("nor ").append(dReg).append(", ").append(sReg).append(", ").append(tReg);
        else if(func == "101010")
            result.append("slt ").append(dReg).append(", ").append(sReg).append(", ").append(tReg);
        else if(func == "101011")
            result.append("sltu ").append(dReg).append(", ").append(sReg).append(", ").append(tReg);

    }
    /*
    else if(opcode == "000001"){
        bool ok;
        int addr, tReg_num;
        QString offset;
        QString tReg, sReg;
        tReg = inst.left(16).right(5);
        tReg_num = tReg.toInt(&ok, 2);

        offset = inst.right(16);
        addr = offset.toInt(&ok, 2);
        addr += (pc + 4);

        if(tReg_num == 0)
            result.append("bltz ").append(sReg).append(", ").append(labelLib.getName(addr));
        else if(tReg_num == 1)
            result.append("bgez ").append(sReg).append(", ").append(labelLib.getName(addr));
    }*/

    else if(opcode == "000001"){
        int addr, tReg_num, sReg_num;
        QString offset, tReg, sReg;
        offset = inst.right(16);
        addr = offset.toInt(&ok, 2);
        if(offset.left(1) == "1")
            addr = -(65536 - addr);
        addr *= 4;
        addr += (pc + 4);

        tReg = inst.left(16).right(5);
        tReg_num = tReg.toInt(&ok, 2);
        tReg = getRegName(tReg_num);

        sReg = inst.left(11).right(5);
        sReg_num = sReg.toInt(&ok, 2);
        sReg = getRegName(sReg_num);

        if(tReg == "00001")
            result.append("bgez ").append(sReg);
        else
            result.append("bltz ").append(sReg);

        result.append(", ").append(labelLib.getName(addr));
    }
    else if(opcode == "000010" || opcode == "000011"){
        int addr;
        QString offset;
        offset = inst.right(26);
        offset.append("00");
        addr = offset.toInt(&ok, 2);
        if(opcode == "000010")
            result.append("j ");
        else
            result.append("jal ");

        result.append(labelLib.getName(addr));
    }
    else if(opcode == "000100" || opcode == "000101" || opcode == "000110" || opcode == "000111"){
        int addr, tReg_num, sReg_num;
        QString offset, tReg, sReg;
        offset = inst.right(16);
        addr = offset.toInt(&ok, 2);
        if(offset.left(1) == "1")
            addr = -(65536 - addr);
        addr *= 4;
        addr += (pc + 4);

        tReg = inst.left(16).right(5);
        tReg_num = tReg.toInt(&ok, 2);
        tReg = getRegName(tReg_num);

        sReg = inst.left(11).right(5);
        sReg_num = sReg.toInt(&ok, 2);
        sReg = getRegName(sReg_num);

        if(opcode == "000100")
            result.append("beq ").append(sReg).append(", ").append(tReg);
        else if(opcode == "000101")
            result.append("bne ").append(sReg).append(", ").append(tReg);
        else if(opcode == "000110")
            result.append("blez ").append(sReg);
        else
            result.append("bgtz ").append(sReg);

        result.append(", ").append(labelLib.getName(addr));
    }
    else if(opcode == "001000" || opcode == "001001" || opcode == "001010" || opcode == "001011" ||
            opcode == "001100" || opcode == "001101" || opcode == "001110" || opcode == "001111"){
        int tReg_num, sReg_num;
        QString tReg, sReg, imme;
        QString signedImme, unsignedImme;

        int imme_num;
        imme = inst.right(16);
        imme_num = imme.toInt(&ok, 2);

        unsignedImme = unsignedImme.setNum(imme_num, 16);
        if(imme.left(1) == "1"){
           signedImme.append("-");
           imme_num = 65536 - imme_num;
           signedImme.append(imme.setNum(imme_num, 16));
        }
        else
            signedImme = unsignedImme;

        unsignedImme.prepend("0x");

        tReg = inst.left(16).right(5);
        tReg_num = tReg.toInt(&ok, 2);
        tReg = getRegName(tReg_num);

        sReg = inst.left(11).right(5);
        sReg_num = sReg.toInt(&ok, 2);
        sReg = getRegName(sReg_num);

        if(opcode == "001000")
            result.append("addi ").append(tReg).append(", ").append(sReg).append(", ").append(signedImme);
        else if(opcode == "001001")
            result.append("addiu ").append(tReg).append(", ").append(sReg).append(", ").append(unsignedImme);
        else if(opcode == "001010")
            result.append("slti ").append(tReg).append(", ").append(sReg).append(", ").append(signedImme);
        else if(opcode == "001011")
            result.append("sltiu ").append(tReg).append(", ").append(sReg).append(", ").append(unsignedImme);
        else if(opcode == "001100")
            result.append("andi ").append(tReg).append(", ").append(sReg).append(", ").append(unsignedImme);
        else if(opcode == "001101")
            result.append("ori ").append(tReg).append(", ").append(sReg).append(", ").append(unsignedImme);
        else if(opcode == "001110")
            result.append("xori ").append(tReg).append(", ").append(sReg).append(", ").append(unsignedImme);
        else
            result.append("lui ").append(tReg).append(", ").append(unsignedImme);
    }
    else if(opcode == "100000" || opcode == "100001" || opcode == "100011" || opcode == "100100" ||
            opcode == "100101" || opcode == "101000" || opcode == "101001" || opcode == "101011"){

        bool ok;
        int offset_num, tReg_num, sReg_num;
        QString tReg, sReg, offset;
        offset = inst.right(16);
        offset_num = offset.toInt(&ok, 2);
        offset = offset.setNum(offset_num, 10);

        tReg = inst.left(16).right(5);
        tReg_num = tReg.toInt(&ok, 2);
        tReg = getRegName(tReg_num);

        sReg = inst.left(11).right(5);
        sReg_num = sReg.toInt(&ok, 2);
        sReg = getRegName(sReg_num);

        if(opcode == "100000")
            result.append("lb ");
        else if(opcode == "100001")
            result.append("lh ");
        else if(opcode == "100011")
            result.append("lw ");
        else if(opcode == "100100")
            result.append("lbu ");
        else if(opcode == "100101")
            result.append("lhu ");
        else if(opcode == "101000")
            result.append("sb ");
        else if(opcode == "101001")
            result.append("sh ");
        else
            result.append("sw ");

        result.append(tReg).append(", ").append(offset).append("(").append(sReg).append(")");

    }
    else if(inst == "01000010000000000000000000011000")
        result.append("eret");
    else if(inst == "00000000000000000000000000001100")
        result.append("syscall");
    //

    result.append(";\n");
    return result;
}

QString Disassemble::zeroExtent(QString num, int target){
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

QString Disassemble::signExtent(QString num, int target){
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

QString Disassemble::getRegName(int n){
    QString result;
    switch(n){
        case(0):
            result.append("$zero");
            break;
        case(1):
            result.append("$at");
            break;
        case(2):
            result.append("$v0");
            break;
        case(3):
            result.append("$v1");
            break;
        case(4):
            result.append("$a0");
            break;
        case(5):
            result.append("$a1");
            break;
        case(6):
            result.append("$a2");
            break;
        case(7):
            result.append("$a3");
            break;
        case(8):
            result.append("$t0");
            break;
        case(9):
            result.append("$t1");
            break;
        case(10):
            result.append("$t2");
            break;
        case(11):
            result.append("$t3");
            break;
        case(12):
            result.append("$t4");
            break;
        case(13):
            result.append("$t5");
            break;
        case(14):
            result.append("$t6");
            break;
        case(15):
            result.append("$t7");
            break;
        case(16):
           result.append("$s0");
            break;
        case(17):
            result.append("$s1");
            break;
        case(18):
            result.append("$s2");
            break;
        case(19):
            result.append("$s3");
            break;
        case(20):
            result.append("$s4");
            break;
        case(21):
            result.append("$s5");
            break;
        case(22):
            result.append("$s6");
            break;
        case(23):
            result.append("$s7");
            break;
        case(24):
            result.append("$t8");
            break;
        case(25):
            result.append("$t9");
            break;
        case(26):
            result.append("$k0");
            break;
        case(27):
            result.append("$k1");
            break;
        case(28):
            result.append("$gp");
            break;
        case(29):
            result.append("$sp");
            break;
        case(30):
            result.append("$fp");
            break;
        case(31):
            result.append("$ra");
            break;
    }
    return result;
}

