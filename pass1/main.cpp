#include <iostream>
#include <map>

#include <string>
#include <fstream>
#include <list>
#include <cstring>
#include <iostream>
//std::string *splitter(std::string line) {
//    std::string splitted_line[3];
//    int pos;
//    for (int i = 0; i < 3; ++i) {
//        pos = line.find_last_not_of(" \t");
//        line.erase(pos + 1, line.size());
//        pos = line.find_last_of(" \t");
//        splitted_line[i]=(line.substr(pos + 1, line.size()));
//        if (pos !=-1) {
//            line.erase(pos, line.size());
//        } else{
//            line.erase(pos+1, line.size());
//        }
//    }
//    return splitted_line;
//}

#include <map>

std::map<int, std::string> check_variables(int endIndex ,std::map<int,std::string[3]> lines,std::map<int, std::string> error_messages){
    for (endIndex= endIndex+1 ; endIndex < lines.size(); endIndex++) {
        for (int i = endIndex+1; i < lines.size(); ++i) {
            if (lines[endIndex][0]==lines[i][0]){
                if (lines[endIndex][1]!=lines[i][1] || lines[endIndex][2]!=lines[i][2]){
                    error_messages[i]="**** ERROR \""+lines[i][0]+"\" already defined";
                }
            }
        }
    }
    std::cout << "Hello, World!" << std::endl;

};

void fileLoader(std::string path,std::map<int,std::string[3]> lines) {
    std::map<int, std::string> error_messages;
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
    std::cout << "Hello, World!" << std::endl;
check_variables(11,lines,error_messages);

}




int main() {
    std::map<int,std::string[3]> lines;
    std::map<int, std::string> error_messages;
    fileLoader("/home/belal/CLionProjects/pass1/keys.txt",lines);
    //check_variables(11,lines,error_messages);
    std::cout << "Hello, World!" << std::endl;

    return 0;
}
