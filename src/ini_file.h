#pragma once
#ifndef MANDELBROT_INIFILE_INCLUDED

#include <string>
#include <fstream>
#include <unordered_map>

class IniFile {

protected:
    std::string path;
    std::fstream fstream;
    std::unordered_map<std::string, std::string> contents;

public:
    IniFile(const std::string& path);
    virtual ~IniFile();

protected:
    void readContents();

};

#endif
