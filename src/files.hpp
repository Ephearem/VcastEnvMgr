#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
std::vector<std::string> getFileDataAsStringsVector(std::filesystem::path const& path);
void writeFileDataFromStringVector(std::filesystem::path const& path, std::vector<std::string> const& newData);