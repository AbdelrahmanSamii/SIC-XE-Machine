#include <iostream>
#include <string>
#include <sstream>
#include <strings.h>
#include <cstdio>
#include <cstdlib>
#include <stdlib.h>
#define max_fixed_size 100
using namespace std;
char *tocharArray(string x);
//int getValue (string x, string y[max_fixed_size][max_fixed_size]);
int getSize (string x);
int getValue (string x);
int getLength (string x);
int main()
{
    string x [8][3]= {{"Prbn01","START","1000"},{"0","LDA","BETA"},{"0","MUL","GAMMA"},{"0","STA","ALPHA"},{"ALPHA","RESW","1"},{"BETA","WORD","25"},{"GAMMA","WORD","4"},{"0","END","Prbn01"}};
  int sizeX =sizeof(x)/sizeof(x[0]);
  int sizeY = 3;
    cout << "Hello world!" <<sizeX<<"hdf"<<sizeY<<endl;
  int AddressCode = 0;

  cout <<AddressCode<<endl;
    for(int i=0;i<sizeof(x)/sizeof(x[0]);i++){
            string op = x[i][1];
        if(op=="START"){
                string temp =x[i][2];
            if(temp.size()>4){
                    char * ctemp=tocharArray(x[0][2]);
                    AddressCode = strtoul(ctemp, NULL, 16);

            }else{
                std::istringstream ( x[0][2] ) >> AddressCode;
            }
        }else if (op=="BYTE"|| op == "WORD"){
        if(op=="BYTE"){
            AddressCode+=getLength(x[i][2]);
        }else{
            AddressCode+=3;
        }

        }else if (op=="RESW"|| op == "RESB"){
            int number =getSize(x[i][2]);
        if(op=="RESB"){
            AddressCode+=number;
        }else{
            AddressCode+=(3*number);
        }

        }else if (op=="END"){

        }else if (op=="ADD"||op=="MUL"||op=="DIV"||op=="SUB"||op== "COMP"){
            int flag=0;
        for(int j=0;j<sizeof(x)/sizeof(x[0]);j++){
        if(x[j][0]==x[i][2]){
        if(x[j][1]=="WORD"){
            flag= 3;
        }else if (x[j][1] =="BYTE"){
        flag= getLength(x[j][2]);
            }
      } else {
      flag= 0;
      }
}

            AddressCode+=flag;


        }else if (op=="ADDR"||op=="MULR"||op=="DIVR"||op=="SUBR"||op== "COMPR"){
            int flagtwo=0;
        for(int l=0;l<sizeof(x)/sizeof(x[0]);l++){
        if(x[l][0]==x[i][2]){
        if(x[l][1]=="WORD"){
            flagtwo= 3;
        }else if (x[l][1] =="BYTE"){
        flagtwo= getLength(x[l][2]);
            }
      } else {
      flagtwo= 0;
      }
            AddressCode+= (flagtwo*2);

        }

        /////
    }
    cout<<AddressCode<<endl;
}
    return 0;

}

int getLength (string x){
string y =x.substr(2,x.size()-2);

return y.size();

}
int getSize (string x){
int temp=0;
 std::istringstream (x) >> temp;
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

char *tocharArray(string x){

 int n = x.length() + 1;
 char c[n];
 for(int i=0; i<x.length(); ++i){
   c[i] = x[i];
 }
 return c;

}
