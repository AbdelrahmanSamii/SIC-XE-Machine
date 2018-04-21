//
// Created by belal on 20/04/18.
//

#ifndef PASS1_FILE_H
#define PASS1_FILE_H


#include <string>
#include <fstream>
#include <list>
#include <cstring>
#include <iostream>
#include <map>

class File {
public:
    std::map<int,std::string[3]> lines;
    std::map<int, std::string> error_messages;
    int endIndex;

public:
    void fileLoader(std::string path) {
        std::ifstream file(path);
        std::string line;
        int index = 0;
        while (std::getline(file, line)) {
            if (line.size() == 0 || line.find(".") != -1) {
                continue;
            } else {
                int pos, j =2;
                for (int i = 0; i < 3; ++i) {
                    pos = line.find_last_not_of(" \t");
                    line.erase(pos + 1, line.size());
                    pos = line.find_last_of(" \t");
                    lines[index][j]=(line.substr(pos + 1, line.size()));
                    if (lines[index][1]=="END"){
                        endIndex=index;
                    }
                    if (pos !=-1) {
                        line.erase(pos, line.size());
                    } else{
                        line.erase(pos+1, line.size());
                    }
                    j--;
                }
            }
            index++;
        }
    }

    std::map<int,std::string[3]> getList(){
        return lines;
    }

    std::map<int, std::string> check_variables(){
        for (endIndex= endIndex+1 ; endIndex < lines.size(); endIndex++) {
            for (int i = endIndex+1; i < lines.size(); ++i) {
                if (lines[endIndex][0]==lines[i][0]){
                    if (lines[endIndex][1]!=lines[i][1] || lines[endIndex][2]!=lines[i][2]){
                        error_messages[i]="**** ERROR \""+lines[i][0]+"\" already defined";
                    }
                }
            }
        }
    };


};


#endif //PASS1_FILE_H
