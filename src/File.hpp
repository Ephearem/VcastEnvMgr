#pragma once
#include <string>
#include <vector>


class File
{
public:
    File(std::string const& path, std::string const& name);
    inline std::string getFullName() const { return this->fullName_; }
    inline std::string getPath() const { return this->path_; }
    inline std::string getFullPath() const { return this->path_ + this->fullName_; }
    inline std::string getName() const { return this->fullName_.substr(0, this->fullName_.find_last_of(".")); }
    inline std::string getExtension() const { return this->fullName_.substr(this->fullName_.find_last_of(".") + 1); }
    std::vector<std::string> getDataAsStringsVector() const;
    void writeDataFromStringVector(std::vector<std::string> const& newData) const; // TODO: add bool rewrite arg
    void clear() const;
    File copyTo(std::string const& path) const;

private:
    std::string path_;
    std::string fullName_;
};