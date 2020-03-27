#include "files.hpp"

/**-----------------------------------------------------------------------------
Function: getFileDataAsStringsVector

\brief Opens a file, stores it line-by-line in the vector of strings and return this vector.
------------------------------------------------------------------------------*/
std::vector<std::string> getFileDataAsStringsVector(std::filesystem::path const& path)
{
    std::ifstream ifs(path.string(), std::ios_base::in); // TODO: is .string() rly need? (?)
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


/**-----------------------------------------------------------------------------
Function: getFileDataAsStringsVector

\brief Opens a file, stores 'newData' there line-by-line.
------------------------------------------------------------------------------*/
void writeFileDataFromStringVector(std::filesystem::path const& path, std::vector<std::string> const& newData)
{
    std::ofstream ofs(path.string(), std::ios::out); // TODO: is .string() rly need? (?)
    if(!ofs.is_open())
        return; // TODO: throw an error (!)

    for (auto const& str : newData)
    {        
        ofs << str; // TODO: fix last empty string stuff (!)
        if(!str.empty())
            ofs << "\n";
    }
    ofs.close();
}