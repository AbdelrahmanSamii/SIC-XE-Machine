#include "stdafx.h"
#include <string>
#include <fstream>
#include <list>
#include <cstring>
#include <iostream>
#include <map>
#include <ctype.h>


using namespace std;
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
	void operate(map<int, string[3]>* lines, map<int, string>* error_messages);
	void checkparameters(int index, string *err, int operationIndex);
	void checkLabel(int index, string * err);
	void checkOperation(int index, string * err);
	void setKeyWords();
	const static int opsSize = 50;
	const static int regSize = 9;
	string registers[regSize] = { "A","X","L","PC","SW","B","S","T","F" };
	operation  o[opsSize];
	map<int, string[3]>* mlines;
	map<int, string>* merror_messages;
	map<int, label> labels;
	int endIndex;
	void toUpperStringP(string *str);
	string toUpperStringV(string str);
};

void MyClass::toUpperStringP(string *str) {
	int l = str->length();
	for (int i = 0; i < l; i++) {
		str->at(i)=toupper(str->at(i));
	}
}
string MyClass::toUpperStringV(string str) {
	int l =str.length();
	for (int i = 0; i < l; i++) {
		str[i]= toupper(str.at(i));
	}
	return str;
}
void MyClass::checkparameters(int index, string *err, int i) {
	string  parameters[2];
	int numOfparameters = 2;
	int commaIndex = -1;
	commaIndex = mlines->at(index)[2].find(",");
	if (mlines->at(index)[2] == "") { //no parameters
		numOfparameters = 0;
	}
	else if (commaIndex == std::string::npos) { //no comma --> user provides one parameter
		parameters[0] = mlines->at(index)[2];
		numOfparameters = 1;
	}
	else {
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
					}
					else if (r == regSize - 1) {
						err->append("\"" + mlines->at(index)[1] + "\" requires register as parameter rand found \"" + parameters[p] + "\" instead\n");

					}
				}
			}
		}

		else { // parameters must be non registers and defiend  in labels or jumb
			bool nonReg = false;
			for (int p = 0; p < numOfparameters; p++) {

				if (parameters[p] != "") {
					for (int l = 0; l < labels.size(); l++) {
						if (parameters[p] == labels[l].name) {
							 if (o[i].jumpOperation) {      // if jumb operation
								 if (labels[l].isStorage) {
									 err->append("the operation requires non storage directive parameter found:  \"" + parameters[p] + "\" \n");
								 }
								 else {
									 return;
								 }
							}else if (labels[l].isStorage) {//not jumb and the param. equals storage
								return;
							}
							
						}
					}
					if (!nonReg) {
						err->append("the operation\""+mlines->at(index)[1]+"\" requires storage directive parameter found:  \"" + parameters[p] + "\" \n");
					}
				}
			}
		}
	}
	else { // if wrong number of parameters
		err->append("wrong number of parameters for operation  \"" + mlines->at(index)[1] + "\" \n");
	}
}
void  MyClass::operate(map<int, string[3]>* lines, map<int, string>* error_messages) {

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


void MyClass::checkOperation(int index, string * err) {
	for (int i = 0; i < opsSize; i++) {  // op op
		if (o[i].R) {                   // if must got registerappended
			for (int j = 0; j < regSize; j++) {
				if ((o[i].name + registers[j]).compare(mlines->at(index)[1]) == 0) {
					//check parameters with true R
					checkparameters(index, err, i);
					return;
				}
			}
		}
		else if ((o[i].name).compare(mlines->at(index)[1]) == 0) { // if found it with no r added to op
			if (o[i].storageDirective) { //if storage
				if (mlines->at(index)[0] == "") { //it must has label
					err->append("\"" + mlines->at(index)[1] + "\" is defiend as storage directive , it must has a label\n");
				}
				else {
					//TODO storage parameters check and label
				}
			}

			else { //non storage 
				checkparameters(index, err, i);
			}
			return;
		}
		if (i == opsSize - 1) {
			err->append("undefiend or unsupported operation \"" + mlines->at(index)[1] + "\" \n");
		}
	}
}


void MyClass::checkLabel(int index, string * err) {

	if (mlines->at(index)[0] != "") {
		label lbl;
		lbl.name = mlines->at(index)[0];
		lbl.index = index;
		if (mlines->at(index)[1] == "RESW" || mlines->at(index)[1] == "RESB" || mlines->at(index)[1] == "WORD" || mlines->at(index)[1] == "BYTE") {
			lbl.isStorage = true;
		}
		else {
			lbl.isStorage = false;
		}
		labels[labels.size()] = lbl;
		for (int i = index - 1; i >= 0; i--) {
			if (mlines->at(i)[0] == mlines->at(index)[0]) {
				err->append("Label \"" + mlines->at(index)[0] + "\" is defined in line " + std::to_string(i) + "\n");
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
MyClass::MyClass()
{
	setKeyWords();
}

MyClass::~MyClass()
{
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

}
int main() {
//	MyClass cll;
//	string ss = "dfed"; cll.toUpperStringP(&ss);
//	cout << ss; 

	string s = "";
	map<int, string[3]> lines;
	map<int, string> errs;
	int i = 0;
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
	}
	MyClass cls;
	
	cls.operate(&lines, &errs);
	for (int e = 0; e < errs.size(); e++) {
		cout << errs.at(e);
	}

	cin >> s;
	return 0;
}

