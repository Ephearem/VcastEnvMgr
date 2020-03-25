#pragma once
#include <string>
#include <vector>

#include "File.hpp"

void showHelp();
void showMessage(std::string const& text);
void throwError(size_t errorCode, std::string const& text);
void normalizePath(std::string& path);
std::vector<File> findFiles(std::string const& path, std::string const& pattern, bool isRecursively);
std::string calcExecutablePath();
std::string calcTempDirectoryName(std::string const& prefix);
std::string calcSafeTempDirectoryPath(std::string const& sourceCodePath);
std::vector<File> copyEnvironments(std::string const& from, std::string const& to, bool isRecursively);
void editEnvFile(File & file, size_t backstepsCount);
void editBatFile(File & file, size_t backstepsCount, bool isFullReport);
size_t calcBackstepsToDriveRoot(std::string const& path);

void deploy(std::string const& regressionScriptsPath, std::string const& sourceCodePath, bool isSafeMode, bool isFullReport);
void fixpaths(std::string const& regressionScriptsPath, size_t backstepsCount); // TODO: 