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
#include <regex>

#define max_fixed_size 100
#define underline "\033[4m"
#define noUnderline "\033[0m"
using namespace std;


int getSize(string x);

string getHexadeciaml(int AddressCode);

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

    if(mlines->at(index)[2][0] == '#'||mlines->at(index)[2][0] == '@'){
        if(mlines->at(index)[2][1]-48>=0&&mlines->at(index)[2][1]-48<=9){
            for(int i=1;i<mlines->at(index)[2].size();i++){
                if(!(mlines->at(index)[2][i]-48>=0&&mlines->at(index)[2][i]-48<=9)){
                    err->append("\ invalid parameter\n");
                    return;
                }
            }

            return ;
        }
        string sss = mlines->at(index)[2].substr(1,mlines->at(index)[2].size());
        bool xx = false;
        for(int i =0 ; i<labels.size();i++){
            if(labels[i].name.compare(sss)==0&&labels[i].isStorage){
                xx=true;
            }

        }
        if(!xx){
            err->append("\ invalid label\n");

        }
        return;
    }
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


    if (o[i].name.compare("LDCH") == 0 || o[i].name.compare("STCH") == 0|| o[i].name.compare("ST")==0|| o[i].name.compare("LD")==0) {
        if (numOfparameters == 2) {
            string ds ="X";
            if (ds.compare(parameters[1]) != 0) {
                err->append("\"" + mlines->at(index)[1] + "\" requires register X as second parameter and found \"" + parameters[1] + "\" instead\n");
            }
            numOfparameters = 1;
            parameters[1] = "";
        }
    }
	if (o[i].name.compare("SHIFTL") == 0 || o[i].name.compare("SHIFTR") == 0) {
		if (numOfparameters != 2) {
			err->append("wrong number of parameters for operation  \"" + mlines->at(index)[1] + "\" \n");
			return;
		}
		numOfparameters = 1;
		for (int n = 0; n < parameters[1].length(); n++) {
			if (parameters[1][n] > 9 || parameters[1][n] < 0) {
				err->append("second parameter of shift must be +ve integer \n");
			}
		}

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
            for (int p = 0; p < numOfparameters; p++) {

                if (parameters[p].compare("")!=0) {
                    for (int l = 0; l < labels.size(); l++) {
                        if (parameters[p].compare( labels[l].name)==0) {
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

                        err->append("the operation\"" + mlines->at(index)[1] +
                                    "\" requires storage directive parameter found:  \"" + parameters[p] + "\" \n");

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
    bool f4 = false;
    if (mlines->at(index)[1][0] == '+') {
        f4 = true;
        mlines->at(index)[1] = mlines->at(index)[1].substr(1, mlines->at(index)[1].size());
    }


    if ((mlines->at(index)[1]).compare("START") == 0 || (mlines->at(index)[1]).compare("END") == 0) {
        if (f4) {
            mlines->at(index)[1] = '+' + mlines->at(index)[1];
        }
        return;
    }

    for (int i = 0; i < opsSize; i++) {

        // op op
        if (o[i].R) {                   // if must got registerappended
            for (int j = 0; j < regSize; j++) {
                if ((o[i].name + registers[j]).compare(mlines->at(index)[1]) == 0) {
                    //check parameters with true R
                    checkparameters(index, err, i);
                    if (f4) {
                        mlines->at(index)[1] = '+' + mlines->at(index)[1];
                    }
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
                            if (f4) {
                                mlines->at(index)[1] = '+' + mlines->at(index)[1];
                            }
                            return;
                        }
                    } else if (mlines->at(index)[1] == "WORD") {
                        int temp = 0;
                        std::istringstream(mlines->at(index)[2]) >> temp;
                        if (temp == (int) temp) {
                            if (f4) {
                                mlines->at(index)[1] = '+' + mlines->at(index)[1];
                            }
                            return;
                        }

                    }
                    //TODO storage parameters check and label
                }
            } else { //non storage
                checkparameters(index, err, i);
            }
            if (f4) {
                mlines->at(index)[1] = '+' + mlines->at(index)[1];
            }
            return;
        }
        if (i == opsSize - 1) {
            err->append("undefiend or unsupported operation \"" + mlines->at(index)[1] + "\" \n");
        }

    }
    if (f4) {
        mlines->at(index)[1] = '+' + mlines->at(index)[1];
    }
}


void MyClass::checkLabel(int index, string *err) {

    if (mlines->at(index)[0] != "") {
        label lbl;
        lbl.name = mlines->at(index)[0];
        if (!(lbl.name[0] <= 'Z' && lbl.name[0] >= 'A')) {
            err->append("The name of the label must start with character  \n");
        }
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
                err->append("Label \"" + mlines->at(index)[0] + "\" is defined in some line before\n");
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
    operation gtOperation = operation("J", 2, false, false, 1, false);
    gtOperation.jumpOperation = true;
    o[i] = gtOperation;
    i++;
    operation gtOperation1 = operation("JGT", 2, false, false, 1, false);
    gtOperation1.jumpOperation = true;
    o[i] = gtOperation1;
    i++;

    operation ltOperation = operation("JLT", 2, false, false, 1, false);
    ltOperation.jumpOperation = true;
    o[i] = ltOperation;
    i++;

    operation t1 = operation("JSUB", 2, false, false, 1, false);
    t1.jumpOperation=true;
    o[i]=t1;
    i++;

     operation t2 = operation("RSUB", 2, false, false, 0, false);
    t2.jumpOperation=true;
    o[i]=t2;
    i++;

    o[i] = operation("LD", 2, true, false, 1, false);
    i++;

    o[i] = operation("ST", 2, true, false, 1, false);
    i++;


    o[i] = operation("LDCH", 1, false, false, 1, false);
    i++;

    o[i] = operation("STCH", 1, false, false, 1, false);
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
	i++;
	o[i] = operation("CLEAR", 2, false, false, 1, true);
	i++;
	o[i] = operation("BASE", 2, false, false, 1, false);
	i++;
	o[i] = operation("NOBASE", 2, false, false, 1, false);
	i++;
	o[i] = operation("SHIFTL", 2, false, false, 1, true);
	i++;
	o[i] = operation("SHIFTR", 2, false, false, 1, true);
	i++;
	o[i] = operation("OR", 2, false, false, 1, false);
	i++;
	o[i] = operation("AND", 2, false, false, 1, false);
	i++;
	o[i] = operation("NORM", 2, false, false, 0, false);

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
    int g = 0;
    string noOperand = "LTORGFIXFLOADHIONORMSIOTIORSUB";
    while (std::getline(file, line)) {
        if (line.size() == 0 || line.find(".") != -1) {
            CommentLine[index + g] = line;
            g++;
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
            if (lines[index][1] == "") {
                lines[index][1] = lines[index][2];
                lines[index][2] = "";
            } else if (lines[index][0] == "") {
                if (lines[index][2] == "START" || lines[index][2] == "END"
                    || (noOperand.find(lines[index][2]) != -1 && lines[index][2].size() > 1)) {
                    lines[index][0] = lines[index][1];
                    lines[index][1] = lines[index][2];
                    lines[index][2] = "";
                }
            }
        }
        index++;
    }

    rr maps;
    maps.lines = lines;
    maps.commentLine = CommentLine;
    return maps;
}


int main() {

    string pathOfFile ;
      cout << "Please Enter filePath: ";
     getline(cin, pathOfFile);
    cout << endl;

    std::map<int, std::string[3]> lines;
    std::map<int, std::string> error_messages;
    std::map<int, std::string> CommentLine;
    rr maps = fileLoader(pathOfFile, lines, CommentLine);

    lines = maps.lines;
    CommentLine = maps.commentLine;

    string s = "";
    map<int, string> errs;

    MyClass cls;
    cls.operate(&lines, &error_messages);

    int sizeX = lines.size();
    int sizeY = CommentLine.size();
   // cout << "size" << sizeX << endl;
    int AddressCode = 0;
    std::map<int, std::string> addressCode;
    for (int j = 0; j < sizeX; j++) {
        if (lines[j][1] == "START") {
            string temp = lines[j][2];
            AddressCode = strtoul(temp.c_str(), NULL, 16);
        }
    }
    int commentflag = 0;
    for (int i = 0; i < sizeX + sizeY; i++) {
        if (CommentLine[i] == "") {
            string op = lines[i - commentflag][1];
            if (op != "END") {
                addressCode[i].append(getHexadeciaml(AddressCode));

                //  cout<<"Not a comment and op is " <<op<<endl;
                if (op == "BYTE" || op == "WORD") {
                    if (op == "BYTE") {
                        AddressCode += getLength(lines[i - commentflag][2]);
                    } else {
                        AddressCode += 3;
                    }

                } else if (op == "RESW" || op == "RESB") {
                    int number = getSize(lines[i - commentflag][2]);
                    if (op == "RESB") {
                        AddressCode += number;
                    } else {
                        AddressCode += (3 * number);
                    }

                } else if (op == "RSUB" || lines[i][0] == "RSUB" || lines[i][2] == "RSUB") {
                    AddressCode += 3;

                } else if (op == "ADD" || op == "MUL" || op == "DIV" || op == "SUB" || op == "COMP") {

                    AddressCode += 3;

                } else if (op == "ADDF" || op == "MULF" || op == "DIVF" || op == "SUBF" || op == "COMPF") {

                    AddressCode += 3;

                } else if (op == "TD" || op == "WD" || op == "RD" || op == "OR" || op == "AND") {
                    AddressCode += 3;

                } else if (op == "ADDR" || op == "MULR" || op == "DIVR" || op == "SUBR" || op == "COMPR" ||
                           op == "CLEAR" || op == "TIXR") {
                    AddressCode += 2;
                } else if (op == "J" || op == "JSUB" || op == "JEQ" || op == "JGT" || op == "JLT" || op == "RSUB") {
                    AddressCode += 3;


                } else if (op == "LDA" || op == "LDB" || op == "LDCH" || op == "LDF" || op == "LDS" || op == "LDL" ||
                           op == "LDT" || op == "LDX" || op == "LPS") {
                    AddressCode += 3;


                } else if (op == "STB" || op == "STCH" || op == "STF" || op == "STI" || op == "STL" || op == "STA" ||
                           op == "SSK" ||
                           op == "STS" || op == "STSW" || op == "STT" || op == "STX" || op == "TIX") {

                    AddressCode += 3;

                }
                    //////////////////////////
                else if (op == "+ADD" || op == "+MUL" || op == "+DIV" || op == "+SUB" || op == "+COMP") {

                    AddressCode += 4;

                } else if (op == "+ADDF" || op == "+MULF" || op == "+DIVF" || op == "+SUBF" || op == "+COMPF") {

                    AddressCode += 4;

                } else if (op == "+ADDR" || op == "+MULR" || op == "+DIVR" || op == "+SUBR" || op == "+COMPR" ||
                           op == "+TIXR" || op == "+CLEAR") {
                    AddressCode += 2;
                } else if (op == "+RSUB" || lines[i][0] == "+RSUB" || lines[i][2] == "+RSUB") {
                    AddressCode += 4;

                } else if (op == "+J" || op == "+JSUB" || op == "+JEQ" || op == "+JGT" || op == "+JLT" ||
                           op == "+RSUB") {
                    AddressCode += 4;


                } else if (op == "+LDA" || op == "+LDB" || op == "+LDCH" || op == "+LDF" || op == "+LDS" ||
                           op == "+LDL" || op == "+LDT" || op == "+LDX" || op == "+LPS") {
                    AddressCode += 4;


                } else if (op == "+STB" || op == "+STCH" || op == "+STF" || op == "+STI" || op == "+STL" ||
                           op == "+STS" || op == "+SSK" ||
                           op == "+STA" || op == "+STSW" || op == "+STT" || op == "+STX" || op == "+TIX") {

                    AddressCode += 4;

                } else if (op == "+TD" || op == "+WD" || op == "+RD" || op == "+OR" || op == "+AND") {
                    AddressCode += 4;

                } else if (op == "+TIO" || op == "TIO" || op == "+SIO" || op == "SIO" || op == "+NORM" ||
                           op == "NORM") {
                    AddressCode += 1;
                } else if (op == "+SVC" || op == "SVC" || op == "+SHIFTR" || op == "SHIFTR" || op == "+SHIFTL" ||
                           op == "SHIFTL") {
                    AddressCode += 2;
                } else if (op == "+FIX" || op == "FLOAT" || op == "+HIO" || op == "FIX" || op == "+FLOAT" ||
                           op == "HIO") {
                    AddressCode += 1;
                }

            } else {

                addressCode[i].append(getHexadeciaml(AddressCode));
                //    cout<<"Not a comment and opEND is " <<op<<endl;
            }
        } else {
            addressCode[i].append(getHexadeciaml(AddressCode));
            commentflag++;
        }


    }
  /* Nasser Printing ADDRESS CODE Map
   * cout << "addresscode size" << addressCode.size() << endl;
    int counter = 0;
    for (int i = 0; i < addressCode.size(); i++) {

        if (CommentLine[i] != "") {
            cout << CommentLine[i] << endl;
            cout << "address " << (i + 1) << "  " << addressCode[i] << endl;
            counter++;

        } else {
            AddressCode = strtoul(addressCode[i].c_str(), NULL, 16);
            cout << AddressCode << "  " << lines[i - counter][1] << " " << lines[i - counter][2]
                 << lines[i - counter][2].size() << endl;
            cout << "address " << (i + 1) << "  " << addressCode[i] << endl;
            // cout<<i<< " "<<lines[i][0]<<" "<<lines[i][1]<<"  "<<lines[i][2]<<endl;
        }
    }*/

//v
    std::map<int, std::string>::iterator it;
    /* std::cout << "Line no.            Adress              Label               Mnemonic            Operands"<< std::endl;
     std::cout << "                                                            Op-code"<< std::endl ;
     int commentCounter = 0;
     int index = 0 ;
     int symbleTable[addressCode.size()] ;

     for (int i = 0; i < addressCode.size() ; i++) {
         if ((lines.at(i)[1] == "WORD" ||lines.at(i)[1] == "BYTE" || lines.at(i)[1] == "RESB" || lines.at(i)[1] == "RESW") && lines.at(i)[1] != "+SUB"){
             symbleTable[i] = 10 ;
         }

         it = CommentLine.find(i + commentCounter);
         if (it == CommentLine.end()) {
             std::cout.width(20);
             std::cout << std::left << i + 1 + index;
             std::cout.width(20);
             std::cout << std::left << addressCode.at(i);
             for (int j = 0; j < 3; j++) {
                 std::cout.width(20);
                 std::cout << std::left << lines.at(i)[j];
             }
             std::cout << '\n';
         }
         else {
             std::cout.width(20);
             std::cout << std::left << i + 1 + index ;
             std::cout.width(20);
             if (i == 0){
                 std::cout << std::left << addressCode.at(i);
                 std::cout << CommentLine.at(i + commentCounter);
             }
             else{
                std::cout << std::left << addressCode.at(i);
                 std::cout << CommentLine.at(i + commentCounter);

             }

             std::cout << '\n';

             std::cout.width(20);
             std::cout << std::left << i + 2 + index;
             std::cout.width(20);
             std::cout << std::left << addressCode.at(i);
             for (int j = 0; j < 3; j++) {
                 std::cout.width(20);
                 std::cout << std::left << lines.at(i)[j];
             }
             commentCounter++;
             index++;
             std::cout << '\n';
         }


         it = error_messages.find(i );
         if (it != error_messages.end()) {
             if (error_messages.at(i) != "" ){
                 std::cout.width(70);
                 std::cout << std::right << error_messages.at(i );
             }
         }
     }

     std::cout << "symbol table" <<endl;
     for (int i = 1 ; i < addressCode.size() ; i++ ){
         if (symbleTable[i] == 10 )
         {
             if (lines.at(i)[1] == "WORD" ||lines.at(i)[1] == "BYTE" || lines.at(i)[1] == "RESB" || lines.at(i)[1] == "RESW")
             {
                 std::cout.width(20);
                 std::cout << std::left << addressCode.at(i);
                 std::cout << std::left << lines.at(i)[0];
                 std::cout << '\n';

             }

         }
     }
 */
    return 0;
}

int getLength(string x) {
    //cout<<"internal"<<x.size()<<x<<endl;
    x = std::regex_replace(x, std::regex("\\s"), "");
    string y = x.substr(2, x.size() - 3);
    // cout<<"internal"<<x.size()<<x.at(0)<<endl;
    if (x.at(0) == 'C') {
        return y.size();

    } else if (x.at(0) == 'X') {
        return 1;

    } else {
        return 1;

    }

}

int getSize(string x) {
    int temp = 0;
    std::istringstream(x) >> temp;
    return temp;
}


string getHexadeciaml(int AddressCode) {
    std::stringstream stream;
    stream << std::hex << AddressCode;
    std::string result(stream.str());
    std::string red = "";
    std::string finalCode = "00000000";
    for (int i = 0; i < (8 - result.size()); i++) {
        red.append("0");
    }
    red.append(result);
    int size = red.size();
    for (int i = 0; i < size; i++) {
        finalCode[i] = toupper(red[i]);
    }

    return finalCode;
}

