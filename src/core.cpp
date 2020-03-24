#pragma once
#include "core.hpp"
#include <Windows.h>
#include <vector>
#include <ctime>


/**-----------------------------------------------------------------------------
Function: findFiles

\brief Stores files with a name that matches the pattern into a vector (as File class objects);
If isRecursively equals to true - calls this functions for each directory located in the current.
------------------------------------------------------------------------------*/
std::vector<File> findFiles(std::string const& path, std::string const& pattern, bool isRecursively)
{
    std::vector<File> files;
    WIN32_FIND_DATAA findFileData;
    HANDLE hFile;
    hFile = FindFirstFileA((path + pattern).c_str(), &findFileData);
    while (hFile != INVALID_HANDLE_VALUE && hFile > 0)
    {
        // TODO: is the next 2 conditions are rly needs for FILES (?)
        if (std::string(findFileData.cFileName) != "." &&   /* if not current dir */
            std::string(findFileData.cFileName) != "..")    /* and not previous dir */
        {
            /* if this is a file */
            if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
            {
                files.push_back(File(path, findFileData.cFileName));
            }
        }
        if (FindNextFileA(hFile, &findFileData) == 0)
            break;
    }
    FindClose(hFile);

    if (isRecursively)
    {
        std::vector<std::string> dirs;

        hFile = FindFirstFileA((path + "*").c_str(), &findFileData);
        while (hFile != INVALID_HANDLE_VALUE && hFile > 0)
        {
            if (std::string(findFileData.cFileName) != "." &&   /* if not current dir */
                std::string(findFileData.cFileName) != "..")    /* and not previous dir */
            {
                /* if this is a directory */
                if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
                {
                    if (isRecursively)
                    {
                        std::vector<File> tmp = findFiles(path + findFileData.cFileName + "\\", pattern , true);
                        files.insert(files.end(), tmp.begin(), tmp.end());
                    }
                }
            }
            if (FindNextFileA(hFile, &findFileData) == 0)
                break;
        }
        FindClose(hFile);
    }
    return files;
}


/**-----------------------------------------------------------------------------
Function: calcTempDirectoryName
\brief Creates a string in format "tmpMMDDYYYY_HHMMSS"
------------------------------------------------------------------------------*/
std::string calcTempDirectoryName()
{
    time_t tt;
    time( &tt );
    tm TM = *localtime( &tt );

    std::string retValue = "tmp";    
    if(TM.tm_mon < 10)
        retValue.push_back('0');
    retValue += std::to_string(TM.tm_mon);
    if(TM.tm_mday < 10)
        retValue.push_back('0');
    retValue += std::to_string(TM.tm_mday);
    retValue += std::to_string(TM.tm_year + 1900);
    retValue.push_back('_');
    if(TM.tm_hour < 10)
        retValue.push_back('0');
    retValue += std::to_string(TM.tm_hour);
    if(TM.tm_min < 10)
        retValue.push_back('0');
    retValue += std::to_string(TM.tm_min);
    if(TM.tm_sec < 10)
        retValue.push_back('0');
    retValue += std::to_string(TM.tm_sec);
    return retValue;
}


/**-----------------------------------------------------------------------------
Function: createTempDir

\brief Creates a directory for storing copies of environments.
The directory is created in the same directory as the executable file.
------------------------------------------------------------------------------*/
std::string const createTempDirectory(std::string const& name)
{
    std::string tempDirFullPath;

    tempDirFullPath.resize(MAX_PATH);
    /* get full path of this executable */
    LPSTR programPath = new CHAR[MAX_PATH];
    GetModuleFileNameA(GetModuleHandle(NULL), programPath, MAX_PATH); // TODO: use argv[0] istead of this shit (?)
    tempDirFullPath = programPath;

    do // TODO: it can be implemented faster (?)
    {
        tempDirFullPath.pop_back();
    } while (tempDirFullPath.at(tempDirFullPath.size() - 1) != '\\');
    tempDirFullPath += name + "\\";

    CreateDirectoryA(tempDirFullPath.c_str(), NULL);
    return tempDirFullPath;
}


/**-----------------------------------------------------------------------------
Function: copyEnvironments

\brief Copies environments from the directory specified in the 'from' argument to the
directory specified in the 'to' argument.
The environment is considered to be the presence when:
there are two files with the same name and with the extensions .env and .bat, respectively in the 'from' dir.
The presence of a .tst file is not checked, since it may not always be in a registered script.
However, if it exists, it will also be copied.
------------------------------------------------------------------------------*/
std::vector<File> copyEnvironments(std::string const& from, std::string const& to, bool isRecursively)
{
    std::vector<File> copies;
    std::vector<File> envFiles = ::findFiles(from, "*.env", true);
    /* for each .env file */
    for(auto const& envFile: envFiles)
    {
        /* find a .bat pair file */
        std::vector<File> batFiles = ::findFiles(envFile.getPath(), envFile.getName() + ".bat", false);
        if(batFiles.size() != 0)
        {
            /* create environment directory in the temp directory */
            CreateDirectoryA((to + envFile.getName()).c_str(), NULL);
            /* copy .env and .bat files there */
            copies.push_back(envFile.copyTo(to + envFile.getName() + "\\"));
            copies.push_back(batFiles[0].copyTo(to +  envFile.getName() + "\\")); // TODO: I hope it will be only 1 .bat file :)

            //CopyFileA(envFile.getFullPath().c_str(), (to + envFile.getName() + "/" + envFile.getFullName()).c_str(), 0);
            //CopyFileA(envFile.getFullPath().c_str(), (to + envFile.getName() + "/" + batFiles[0].getFullName()).c_str(), 0);
            /* and copy .tst if exists */
            std::vector<File> tstFiles = ::findFiles(envFile.getPath(), envFile.getName() + ".tst", false);
            if(tstFiles.size() != 0)
            {
                copies.push_back(tstFiles[0].copyTo(to + envFile.getName() + "\\"));
                //CopyFileA(envFile.getFullPath().c_str(), (to + envFile.getName() + "/" + tstFiles[0].getFullName()).c_str(), 0);
            }

        }
        else
        {
            // TODO: throw an error
            continue;
        }
    }
    return copies;
}


/**-----------------------------------------------------------------------------
Function: editEnvFile

\brief Since the distance from the current directory to the source code directory can be any -
for the successful environment deployment, need to specify the correct path to the source code files,
relative to the location of the current copy of the environment.
------------------------------------------------------------------------------*/
void editEnvFile(File & file, size_t backstepsCount)
{
    std::vector<std::string> fileData = file.getDataAsStringsVector();
    std::vector<std::string> newFfileData;
    for(auto &str : fileData)
    {
        if (str.find("ENVIRO.SEARCH_LIST: ") != std::string::npos) // TODO: replace token with a const
        {
            std::string dir = str.substr(20, str.size() - 20); // TODO: replace 20 with a const
            while (dir.find("..\\") != std::string::npos)
            {
                dir.erase(dir.begin(),  dir.begin() + 3);
            }
            for (size_t i = 0; i < backstepsCount; i++)
            {
                dir.insert(0,  "..\\");
            }
            str.resize(20);
            str += dir;
        }
        newFfileData.push_back(str);
    }
    file.writeDataFromStringVector(newFfileData);   
}


/**-----------------------------------------------------------------------------
Function: editBatFile

\brief The same stuff as in editEnvFile, and:
changes management report generation to full report generation, if isFullReqport = true
------------------------------------------------------------------------------*/
void editBatFile(File & file, size_t backstepsCount, bool isFullReport)
{
    std::vector<std::string> fileData = file.getDataAsStringsVector();
    std::vector<std::string> newFfileData;
    for(auto &str : fileData)
    {
        size_t offset = str.find("TESTABLE_SOURCE_DIR "); // TODO: replace token with a const
        if (offset != std::string::npos)
        {
            std::string dir = str.substr(offset + 20, str.size() - 20); // TODO: replace 20 with a const
            while (dir.find("..\\") != std::string::npos)
            {
                dir.erase(dir.begin(), dir.begin() + 3);
            }
            for (size_t i = 0; i < backstepsCount; i++)
            {
                dir.insert(0, "..\\");
            }
            str.resize(offset + 20);
            str += dir;
        }
        if(isFullReport)
        {          
            /* replace management reports to full reports */
            offset = str.find("reports custom management");
            if (offset != std::string::npos)
            {
                size_t managementWordPos = str.find(" management ");
                str.replace(managementWordPos, 6, " full ");
                str.erase(managementWordPos + 6, 6);

                size_t reportNamePos = str.find("_management_report.html");
                str.replace(reportNamePos, 17, "_FULL_REPORT.html");
                str.erase(reportNamePos + 17, 6);
            }
        }
        newFfileData.push_back(str);
    }
    file.writeDataFromStringVector(newFfileData);   
}

size_t calcBackstepsToDriveRoot(std::string const& path)
{
    size_t distance = 0;
    for (auto const c : path)
    {
        if (c == '\\' || c == '/')
            distance++;
    }
    return distance;
}


/**-----------------------------------------------------------------------------
Function: deploy

\brief 
Calculate the name of the temporary directory (based on the current date and time) -> 
Create a temporary directory ->
Calc the backsteps count from this dir to the drive root ->
Copy environments in the temporary dir ->
Process the copies ->
    (edit .env files, edit .bat files)
Execute the copies of .bat files to deploy environments
------------------------------------------------------------------------------*/
void deploy(std::string const& regressionScriptsPath, std::string const& sourceCodePath)
{
    // TODO: 
    // size_t distRs = calcBackstepsToDriveRoot(regressionScriptsPath);
    // size_t distSrc = calcBackstepsToDriveRoot(sourceCodePath);
    

    /* Calculate the name of the temporary directory (based on the current date and time), create a temporary directory */   
    std::string tempDirFullPath = ::createTempDirectory(::calcTempDirectoryName());
    /* Calc the backsteps count from this dir to the drive root */  
    size_t backsteps = ::calcBackstepsToDriveRoot(tempDirFullPath);    
    /* Copy environments in the temporary dir */
    std::vector<File> copies = ::copyEnvironments(regressionScriptsPath, tempDirFullPath, true);
    /* process the copies */
    for(auto &f : copies)
    {
        if(f.getExtension() == "env")
        {
            ::editEnvFile(f, backsteps);
        }
        else if(f.getExtension() == "bat")
        {
            ::editBatFile(f, backsteps, true);
        }
    }
    /* Execute the copies of .bat files to deploy environments */
    for(auto &f : copies)
    {
        if(f.getExtension() == "bat") // TODO: move this in previous cycle
        {
            std::string runCmd = (std::string("start /wait /d ") + f.getPath() + " cmd /c " + f.getFullPath());
            //std::cout << runCmd << std::endl;
            system(runCmd.c_str());
        }
    }   
}


/**-----------------------------------------------------------------------------
Function: fixpaths

\brief TODO: 
------------------------------------------------------------------------------*/
void fixpaths(std::string const& regressionScriptsPath, size_t backstepsCount)
{
    std::vector<File> envFiles = ::findFiles(regressionScriptsPath, "*.env", true);
    /* for each .env file */
    for(auto & envFile: envFiles)
    {
        /* find a .bat pair file */
        std::vector<File> batFiles = ::findFiles(envFile.getPath(), envFile.getName() + ".bat", false);
        if(batFiles.size() != 0)
        {
            ::editBatFile(batFiles[0], backstepsCount, false);
            ::editEnvFile(envFile, backstepsCount);
        }
    }
}