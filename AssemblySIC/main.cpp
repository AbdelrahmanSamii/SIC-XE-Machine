#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <cstring>
#include <map>
#include <ctype.h>
#include <sstream>
#include <strings.h>
#include <cstdio>
#include <cstdlib>
#include <stdlib.h>

#define max_fixed_size 100
using namespace std;

char *tocharArray(string x);

//int getValue (string x, string y[max_fixed_size][max_fixed_size]);
int getSize(string x);

int getValue(string x);

int getLength(string x);

struct rr {
    std::map<int, std::string[3]> lines;
    std::map<int, std::string> commentLine;
};
struct operation {
public:
    operation(string n, int b, bool r, bool storageD, int parametersNum, bool isRegparameters) {
        name = n;
        bytes = b;
        R = r;
        storageDirective = storageD;
        parametersNumber = parametersNum;
        isregisterparameters = isRegparameters;


    }

    operation() {
        name = "";
        bytes = 0;
        R = false;
        storageDirective = false;
        parametersNumber = 0;
        isregisterparameters = false;
    }


public:
    string name;
    int bytes;
    bool R = false;
    bool storageDirective = false;
    int parametersNumber = 0;
    bool isregisterparameters = false;
    bool jumpOperation = false;
};

struct label {
    string name;
    bool isStorage;
    int index;
};

class MyClass {
public:
    MyClass();

    ~MyClass();

    void operate(map<int, string[3]> *lines, map<int, string> *error_messages);

    void checkparameters(int index, string *err, int operationIndex);

    void checkLabel(int index, string *err);

    void checkOperation(int index, string *err);

    void setKeyWords();

    const static int opsSize = 50;
    const static int regSize = 9;
    string registers[regSize] = {"A", "X", "L", "PC", "SW", "B", "S", "T", "F"};
    operation o[opsSize];
    map<int, string[3]> *mlines;
    map<int, string> *merror_messages;
    map<int, label> labels;
    int endIndex;

    void toUpperStringP(string *str);

    string toUpperStringV(string str);
};

void MyClass::toUpperStringP(string *str) {
    int l = str->length();
    for (int i = 0; i < l; i++) {
        str->at(i) = toupper(str->at(i));
    }
}

string MyClass::toUpperStringV(string str) {
    int l = str.length();
    for (int i = 0; i < l; i++) {
        str[i] = toupper(str.at(i));
    }
    return str;
}

void MyClass::checkparameters(int index, string *err, int i) {
    string parameters[2];
    int numOfparameters = 2;
    int commaIndex = -1;
    commaIndex = mlines->at(index)[2].find(",");
    if (mlines->at(index)[2] == "") { //no parameters
        numOfparameters = 0;
    } else if (commaIndex == std::string::npos) { //no comma --> user provides one parameter
        parameters[0] = mlines->at(index)[2];
        numOfparameters = 1;
    } else {
        int s = mlines->at(index)[2].length();
        parameters[0] = mlines->at(index)[2].substr(0, commaIndex);
        parameters[1] = mlines->at(index)[2].substr(commaIndex + 1, s - commaIndex - 1);
    }


    if (o[i].parametersNumber == numOfparameters) {
        if (o[i].isregisterparameters) {  // parameters must be registers
            for (int p = 0; p < numOfparameters; p++) {
                for (int r = 0; r < regSize; r++) {

                    if (registers[r].compare(parameters[p]) == 0) { // register
                        break;
                    } else if (r == regSize - 1) {
                        err->append("\"" + mlines->at(index)[1] + "\" requires register as parameter rand found \"" +
                                    parameters[p] + "\" instead\n");

                    }
                }
            }
        } else { // parameters must be non registers and defiend  in labels or jumb
            bool nonReg = false;
            for (int p = 0; p < numOfparameters; p++) {

                if (parameters[p] != "") {
                    for (int l = 0; l < labels.size(); l++) {
                        if (parameters[p] == labels[l].name) {
                            if (o[i].jumpOperation) {      // if jumb operation
                                if (labels[l].isStorage) {
                                    err->append("the operation requires non storage directive parameter found:  \"" +
                                                parameters[p] + "\" \n");
                                } else {
                                    return;
                                }
                            } else if (labels[l].isStorage) {//not jumb and the param. equals storage
                                return;
                            }

                        }
                    }
                    if (!nonReg) {
                        err->append("the operation\"" + mlines->at(index)[1] +
                                    "\" requires storage directive parameter found:  \"" + parameters[p] + "\" \n");
                    }
                }
            }
        }
    } else { // if wrong number of parameters
        err->append("wrong number of parameters for operation  \"" + mlines->at(index)[1] + "\" \n");
    }
}

void MyClass::operate(map<int, string[3]> *lines, map<int, string> *error_messages) {

    mlines = lines;
    merror_messages = error_messages;
    for (int i = 0; i < mlines->size(); i++) { //iterating in lines one by one
        toUpperStringP(&mlines->at(i)[0]);
        toUpperStringP(&mlines->at(i)[1]);
        toUpperStringP(&mlines->at(i)[2]);

    }
    for (int i = 0; i < mlines->size(); i++) { //iterating in lines one by one

        string errorOfLine = "";
        checkLabel(i, &errorOfLine);
        merror_messages->insert(std::pair<int, string>(i, errorOfLine));
    }
    for (int i = 0; i < mlines->size(); i++) { //iterating in lines one by one
        string errorOfLine = "";
        checkOperation(i, &errorOfLine);
        merror_messages->at(i).append(errorOfLine);
    }
}


void MyClass::checkOperation(int index, string *err) {
    if ((mlines->at(index)[1]).compare("START") == 0 || (mlines->at(index)[1]).compare("END") == 0) {
        return;
    }

    for (int i = 0; i < opsSize; i++) {
//nasser
        // op op
        if (o[i].R) {                   // if must got registerappended
            for (int j = 0; j < regSize; j++) {
                if ((o[i].name + registers[j]).compare(mlines->at(index)[1]) == 0) {
                    //check parameters with true R
                    checkparameters(index, err, i);
                    return;
                }
            }

        } else if ((o[i].name).compare(mlines->at(index)[1]) == 0) { // if found it with no r added to op
            if (o[i].storageDirective) { //if storage
                if (mlines->at(index)[0] == "") { //it must has label
                    err->append(
                            "\"" + mlines->at(index)[1] + "\" is defiend as storage directive , it must has a label\n");
                } else {
                    if (mlines->at(index)[1] == "BYTE") {
                        string storage = mlines->at(index)[2];
                        string flag = storage.substr(0, 1);

                        if (flag == "C" || flag == "X") {
                            return;
                        }
                    } else if (mlines->at(index)[1] == "WORD") {
                        int temp = 0;
                        std::istringstream(mlines->at(index)[2]) >> temp;
                        if (temp == (int) temp) {

                            return;
                        }

                    }
                    //TODO storage parameters check and label
                }
            } else { //non storage
                checkparameters(index, err, i);
            }
            return;
        }
        if (i == opsSize - 1) {
            err->append("undefiend or unsupported operation \"" + mlines->at(index)[1] + "\" \n");
        }
    }
}


void MyClass::checkLabel(int index, string *err) {

    if (mlines->at(index)[0] != "") {
        label lbl;
        lbl.name = mlines->at(index)[0];
        lbl.index = index;
        if (mlines->at(index)[1] == "RESW" || mlines->at(index)[1] == "RESB" || mlines->at(index)[1] == "WORD" ||
            mlines->at(index)[1] == "BYTE") {
            lbl.isStorage = true;
        } else {
            lbl.isStorage = false;
        }
        labels[labels.size()] = lbl;
        for (int i = index - 1; i >= 0; i--) {
            if (mlines->at(i)[0] == mlines->at(index)[0]) {
                err->append("Label \"" + mlines->at(index)[0] + "\" is defined in line \n");
            }
        }

        for (int r = 0; r < regSize; r++) {
            if (mlines->at(index)[0] == registers[r]) {
                err->append("Label \"" + mlines->at(index)[0] + "\" has the same name of defined register " + "\n");
            }
        }
        for (int p = 0; p < opsSize; p++) {
            if (o[p].name == mlines->at(index)[0]) {
                err->append("Label \"" + mlines->at(index)[0] + "\" has the same name of defined operation " + "\n");
            }
        }
    }
}

MyClass::MyClass() {
    setKeyWords();
}

MyClass::~MyClass() {
}


void MyClass::setKeyWords() {


    int i = 0;
    o[i] = operation("ADD", 2, false, false, 1, false);
    i++;

    o[i] = operation("SUB", 2, false, false, 1, false);
    i++;


    o[i] = operation("DIV", 2, false, false, 1, false);
    i++;


    o[i] = operation("MUL", 2, false, false, 1, false);
    i++;
    o[i] = operation("ADDR", 2, false, false, 2, true);
    i++;


    o[i] = operation("SUBR", 2, false, false, 2, true);
    i++;


    o[i] = operation("DIVR", 2, false, false, 2, true);
    i++;


    o[i] = operation("MULR", 2, false, false, 2, true);
    i++;


    o[i] = operation("COMP", 2, false, false, 1, false);
    i++;


    o[i] = operation("COMPR", 2, false, false, 2, true);
    i++;


    operation eqOperation = operation("JEQ", 2, false, false, 1, false);
    eqOperation.jumpOperation = true;

    o[i] = eqOperation;
    i++;
    operation gtOperation = operation("JGT", 2, false, false, 1, false);
    gtOperation.jumpOperation = true;
    o[i] = gtOperation;
    i++;

    operation ltOperation = operation("JLT", 2, false, false, 1, false);
    ltOperation.jumpOperation = true;
    o[i] = ltOperation;
    i++;

    o[i] = operation("JSUB", 2, false, false, 1, false);
    i++;

    o[i] = operation("RSUB", 2, false, false, 0, false);
    i++;

    o[i] = operation("LD", 2, true, false, 1, false);
    i++;

    o[i] = operation("ST", 2, true, false, 1, false);
    i++;


    o[i] = operation("LDCH", 2, false, false, 1, false);
    i++;

    o[i] = operation("STCH", 2, false, false, 1, false);
    i++;


    o[i] = operation("RMO", 2, false, false, 2, true);
    i++;

    o[i] = operation("TIX", 2, false, false, 1, false);
    i++;

    o[i] = operation("TIXR", 2, false, false, 1, true);
    i++;

    o[i] = operation("TD", 2, false, false, 1, false);
    i++;

    o[i] = operation("RD", 2, false, false, 1, false);
    i++;


    o[i] = operation("WD", 2, false, false, 1, false);
    i++;

    o[i] = operation("RESW", 2, false, true, 1, false);
    i++;

    o[i] = operation("RESB", 2, false, true, 1, false);
    i++;

    o[i] = operation("BYTE", 2, false, true, 1, false);
    i++;


    o[i] = operation("WORD", 2, false, true, 1, false);
    i++;
    o[i] = operation("START", 2, false, false, 1, false);
    i++;
    o[i] = operation("END", 2, false, false, 1, false);

}

std::map<int, std::string>
check_variables(int endIndex, std::map<int, std::string[3]> lines, std::map<int, std::string> error_messages) {
    for (endIndex = endIndex + 1; endIndex < lines.size(); endIndex++) {
        for (int i = endIndex + 1; i < lines.size(); ++i) {
            if (lines[endIndex][0] == lines[i][0]) {
                if (lines[endIndex][1] != lines[i][1] || lines[endIndex][2] != lines[i][2]) {
                    error_messages[i] = "**** ERROR \"" + lines[i][0] + "\" already defined";
                }
            }
        }
    }
}

rr fileLoader(std::string path, std::map<int, std::string[3]> lines, std::map<int, std::string> CommentLine) {
    std::map<int, std::string> error_messages;
    std::ifstream file(path.c_str());
    std::string line;
    int index = 0;
    while (std::getline(file, line)) {
        if (line.size() == 0 || line.find(".") != -1) {
            CommentLine[index] = line;
            continue;
        } else {
            int pos, j = 2;
            for (int i = 0; i < 3; ++i) {
                pos = line.find_last_not_of(" \t");
                line.erase(pos + 1, line.size());
                pos = line.find_last_of(" \t");
                lines[index][j] = (line.substr(pos + 1, line.size()));
                if (pos != -1) {
                    line.erase(pos, line.size());
                } else {
                    line.erase(pos + 1, line.size());
                }
                j--;
            }
        }
        index++;
    }

    rr maps;
    maps.lines = lines;
    maps.commentLine = CommentLine;
    cout<<"size before"<<lines.size();
    return maps;

//    check_variables(11, lines, error_messages);
}


int main() {
    std::map<int, std::string[3]> lines;
    std::map<int, std::string> error_messages;
    std::map<int, std::string> CommentLine;
    rr maps = fileLoader("E:\\CSED 4\\SystemProgramming\\CLion\\mytest.txt", lines, CommentLine);

    lines = maps.lines;
    CommentLine = maps.commentLine;

    cout<<"size after"<<lines.size();

    //check_variables(11,lines,error_messages);
    string s = "";
    //map<int, string[3]> lines;
    map<int, string> errs;
    /*   int i = 0;
       while (s != "m") {
           cout << i + "             label:";
           cin >> s;
           lines[i][0] = s;
           cout << i + "            operation:";
           cin >> s;
           lines[i][1] = s;
           cout << i + "              parameter:";
           cin >> s;
           lines[i][2] = s;
           cout << "enter \"show\" if show ?";
           cin >> s;
           i++;
       }*/
    MyClass cls;
    cls.operate(&lines, &error_messages);

    //  cin >> s;
    //  string x [8][3]= {{"Prbn01","START","1000"},{"0","LDA","BETA"},{"0","MUL","GAMMA"},{"0","STA","ALPHA"},{"ALPHA","RESW","1"},{"BETA","WORD","25"},{"GAMMA","WORD","4"},{"0","END","Prbn01"}};
    int sizeX = lines.size();
  //  int sizeY = 3;
    int AddressCode = 0;
    std::map<int, std::string> addressCode;
    for (int i = 0; i < sizeX; i++) {
        if (error_messages[i] == "") {
            string op = lines[i][1];
            cout<<"operation"<<op;
            if (op == "START") {
                string temp = lines[i][2];
                if (temp.size() > 4) {
                    char *ctemp = tocharArray(lines[0][2]);
                    AddressCode = strtoul(ctemp, NULL, 16);

                } else {
                    std::istringstream(lines[i][2]) >> AddressCode;
                }
            } else if (op == "BYTE" || op == "WORD") {
                if (op == "BYTE") {
                    AddressCode += getLength(lines[i][2]);
                } else {
                    AddressCode += 3;
                }

            } else if (op == "RESW" || op == "RESB") {
                int number = getSize(lines[i][2]);
                if (op == "RESB") {
                    AddressCode += number;
                } else {
                    AddressCode += (3 * number);
                }

            } else if (op == "END") {

            } else if (op == "ADD" || op == "MUL" || op == "DIV" || op == "SUB" || op == "COMP") {
          //      int flag = 0;
//                for (int j = 0; j < sizeX; j++) {
//                    if (lines[j][0] == lines[i][2]) {
//                        if (lines[j][1] == "WORD") {
//                            flag = 3;
//                        } else if (lines[j][1] == "BYTE") {
//                            flag = getLength(lines[j][2]);
//                        }
//                    } else {
//                        flag = 0;
//                    }
//                }

                AddressCode += 3;

            } else if (op == "ADDR" || op == "MULR" || op == "DIVR" || op == "SUBR" || op == "COMPR") {


                /////
                                AddressCode += 2;
            }else if (op == "J" || op == "JSUB" || op == "JEQ" || op == "JGT" || op == "JLT"){
                                            AddressCode += 2;


            }else if (op == "LDA" || op == "LDB" || op == "LDCH" || op == "LDF" || op == "LDS"|| op == "LDL"|| op == "LDT"|| op == "LDX"){
                                            AddressCode += 3;


            }else if (op == "STB" || op == "STCH" || op == "STF" || op == "STI" || op == "STL"|| op == "STA" || op == "STSW" || op == "STT" || op == "STX  "){

                                            AddressCode += 3;

            }
        }
        // cout<<AddressCode<<endl;
        std::stringstream stream;
        stream << std::hex << AddressCode;
        std::string result(stream.str());
        std::string temp = "";
        std::string finalCode = "00000000";
        for (int i = 0; i < (8 - result.size()); i++) {
            temp.append("0");
        }
        temp.append(result);
        for (int i = 0; i < sizeX; i++) {
            finalCode[i] = toupper(temp[i]);
        }
        addressCode[i].append(finalCode);

    }

    for (int i = 0; i < addressCode.size(); i++){
        cout << addressCode[i] << endl;
    }

//    std::map<int, std::string>::iterator it;
//    int commentCounter = 0;
//    for (int i = 0; i < addressCode.size(); i++) {
//        std::cout.width(20);
//        std::cout << std::left << i + 1;
//        std::cout.width(20);
//        std::cout << std::left << addressCode.at(i);
//        it = CommentLine.find(i);
//        if (it == CommentLine.end()) {
//            for (int j = 0; j < 3; j++) {
//                std::cout.width(20);
//                std::cout << std::left << lines.at(i - commentCounter)[j];
//            }
//        } else {
//            commentCounter++;
//            std::cout << CommentLine.at(i);
//        }
//        it = error_messages.find(i - commentCounter);
//        if (it != error_messages.end()) {
//            std::cout << '\n';
//            std::cout.width(50);
//            std::cout << std::right << error_messages.at(i - commentCounter);
//        }
//        std::cout << '\n';
//    }
//    cout << "sami";

    cin >> s;
    return 0;
}

int getLength(string x) {
    string y = x.substr(2, x.size() - 2);

    return y.size();

}

int getSize(string x) {
    int temp = 0;
    std::istringstream(x) >> temp;
    return temp;
}

//int getValue (string x, string y[max_fixed_size][max_fixed_size]){
//
//for(int i=0;i<sizeof(y)/sizeof(y[0]);i++){
//      if(y[i][0]==x){
//        if(y[i][1]=="WORD"){
//    return 3;
//}else if (y[i][1] =="BYTE"){
// return getLength(y[i][2]);
//}
//      } else {
//      return 0;
//      }
//}
//}

char *tocharArray(string x) {

    int n = x.length() + 1;
    char *c[n];
    for (int i = 0; i < x.length(); ++i) {
        *c[i] = x[i];
    }
    return *c;

}
