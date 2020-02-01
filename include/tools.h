//
// Created by zhouy on 2019/12/27.
//

#ifndef TEST_TOOLS_H
#define TEST_TOOLS_H
#include <iostream>
#include <fstream>
#include <string>
#include <io.h>
#include <iterator>
#include "algorithm"

namespace Tools
{
    unsigned int  getFileNumbers(const std::string & base_path,const std::string & file_suffix)
    {
        unsigned int  counter{};
        long hFile = 0;
        _finddata_t fileInfo{};
        std::string pathName{};
        if ((hFile = _findfirst(pathName.assign(base_path+"\\*.").append(file_suffix).c_str(), &fileInfo)) == -1)
            return 0;
        do
        {
            counter++;
        } while (_findnext(hFile, &fileInfo) == 0);
        return counter;
    }

    unsigned int getTxtLineNumbers(const std::string & txt_path)
    {
        std::ifstream my_file(txt_path);

        // new lines will be skipped unless we stop it from happening:
        my_file.unsetf(std::ios_base::skipws);

        // count the newlines with an algorithm specialized for counting:
        unsigned line_count = std::count(
                std::istream_iterator<char>(my_file),
                std::istream_iterator<char>(),
                '\n');

        return line_count;
    }
    std::vector<std::string> getFileNames(const std::string & base_path,const std::string & file_suffix)
    {
        std::vector<std::string> file_names{};
        long hFile = 0;
        _finddata_t fileInfo{};
        std::string pathName{};
        if ((hFile = _findfirst(pathName.assign(base_path+"\\*.").append(file_suffix).c_str(), &fileInfo)) == -1)
            return file_names;
        do
        {
            file_names.emplace_back(base_path+"\\"+ fileInfo.name);
        } while (_findnext(hFile, &fileInfo) == 0);
        return file_names;
    }
    std::vector<std::string> getPureFileNames(const std::string & base_path,const std::string & file_suffix)
    {
        std::vector<std::string> file_names{};
        long hFile = 0;
        _finddata_t fileInfo{};
        std::string pathName{};
        if ((hFile = _findfirst(pathName.assign(base_path+"\\*.").append(file_suffix).c_str(), &fileInfo)) == -1)
            return file_names;
        do
        {
            std::string temp_string{fileInfo.name};
            file_names.emplace_back(temp_string.substr(0,temp_string.rfind('.')));
        } while (_findnext(hFile, &fileInfo) == 0);
        return file_names;
    }

}
#endif //TEST_TOOLS_H
