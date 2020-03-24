#pragma once
#include <string>
#include <vector>

#include "File.hpp"
std::vector<File> findFiles(std::string const& path, std::string const& pattern, bool isRecursively);
std::string calcTempDirectoryName();
std::string const createTempDirectory(std::string const& name);
std::vector<File> copyEnvironments(std::string const& from, std::string const& to, bool isRecursively);
void editEnvFile(File & file, size_t backstepsCount);
void editBatFile(File & file, size_t backstepsCount, bool isFullReport);
size_t calcBackstepsToDriveRoot(std::string const& path);

void deploy(std::string const& regressionScriptsPath, std::string const& sourceCodePath);
void fixpaths(std::string const& regressionScriptsPath, size_t backstepsCount);