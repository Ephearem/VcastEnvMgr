#include "File.hpp"
#include <fstream>


File::File(std::string const& path, std::string const& name)
	:path_(path), fullName_(name)
{ }


std::vector<std::string> File::getDataAsStringsVector() const
{
    std::ifstream ifs(this->getFullPath(), std::ios_base::in);
    std::vector<std::string> fileStrings;
    if(!ifs.is_open())
        return fileStrings; // TODO: throw an error (!)
    
    while (!ifs.eof())
    {
        std::string currentString;
        std::getline(ifs, currentString);
        fileStrings.push_back(currentString);
    }
    ifs.close();
    return fileStrings;
}

void File::writeDataFromStringVector(std::vector<std::string> const& newData) const
{
    this->clear();
    std::ofstream ofs(this->getFullPath(), std::ios::out);
    if(!ofs.is_open())
        return; // TODO: throw an error (!)

    for (auto const& str : newData)
    {        
        ofs << str; // TODO: delete last empty string (!)
        if(!str.empty())
            ofs << "\n";
    }
    ofs.close();
}

void File::clear() const
{
    std::ofstream ofs(this->getFullPath(), std::ios::out | std::ios::trunc);
    ofs.close();
}

File File::copyTo(std::string const& path) const
{    
    std::ifstream  src(this->getFullPath(), std::ios::binary);
    if(!src.is_open())
    {        
        // TODO: throw an error (!)
    }
    std::ofstream  dst(path + this->getFullName(),   std::ios::binary);
    if(!dst.is_open())
    {        
        // TODO: throw an error (!)
    }            
    dst << src.rdbuf();
    dst.close();
    src.close();
    return File(path, this->getFullName());
}