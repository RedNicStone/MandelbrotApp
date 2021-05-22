#include "ini_file.h"

IniFile::IniFile(const std::string& path) : path{path}, fstream{path} {
    readContents();
}

IniFile::~IniFile() {
    fstream.close();
}

void IniFile::readContents() {
    std::string line;
    while(std::getline(fstream, line)) {
        // TODO
    }
}
