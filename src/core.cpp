#pragma once
#include "core.hpp"
#include <Windows.h>
#include <vector>
#include <ctime>
#include <iostream>
#include <conio.h>

/**-----------------------------------------------------------------------------
Function: showHelp

\brief Displays a help message.
------------------------------------------------------------------------------*/
void showHelp()
{
    system("cls");
    std::wcout << "VcastEnvMgr v0.01" << std::endl;
    std::wcout << std::endl;
    std::wcout << "Brief:" << std::endl;
    std::wcout << "This program recursively finds and copies all regression scripts to a temporary directory, deploys them (by running .bat files) and, if specified in the launch options, generates full reports." << std::endl;
    std::wcout << std::endl;
    std::wcout << "Some info:" << std::endl;
    std::wcout << "The name of the temporary directory is in the format \"tmp_yyyymmdd_hhmmss\"." << std::endl;
    std::wcout << "By default, a temporary directory is created where this .exe file is located." << std::endl;
    std::wcout << "But this may not be safe, since in regression scripts the relative paths to the source code are most often used. Actually, the success of regression script deployment depends on their location relative to the source code." << std::endl;
    std::wcout << "Most often, in the PSA repositories, the \"regression scripts\" directory is located in the same place as the source code. Therefore, it is recommended that you run the .exe file from there." << std::endl;
    std::wcout << "If the distance from the temporary folder to the source code is different from the default (2), the program will warn you that the paths in the copies will be changed." << std::endl;
    std::wcout << std::endl;
    std::wcout << "Allowable command templates:" << std::endl;
    std::wcout << "\t-deploy rs=[regression scripts path] src=[source code path] [flags]" << std::endl;
    std::wcout << "example: -deploy rs=F:\\VPR\\trunk\\regression_scripts\\Voting\\ src=F:\\VPR\\trunk\\src\\ -safe -full" << std::endl;
    std::wcout << std::endl;
    std::wcout << "Allowable flags:" << std::endl;
    std::wcout << "\t-safe - Creates a temporary directory in a place that does not require path changes in .bat and .env files (most often this is the 'trunk' folder)." << std::endl;
    std::wcout << "\t-full - Changes management report generation instruction to full report generation instruction." << std::endl;
    std::wcout << std::endl;
    std::wcout << "Commands and flags should be specified in the launch parameters." << std::endl;
    std::wcout << std::endl;
    std::wcout << " evgeny.gancharik@psa-software.com" << std::endl;
    std::wcout << " 04/22/2020" << std::endl;
    std::cout << std::endl;
    system("pause");
}


/**-----------------------------------------------------------------------------
Function: showMessage

\brief Displays a message and waits for any key to be pressed.
------------------------------------------------------------------------------*/
void showMessage(std::string const& text)
{
    std::cout << text << std::endl;
    std::cout << std::endl << "Press any key to continue..." << std::endl << std::endl;
    getch();
}


/**-----------------------------------------------------------------------------
Function: throwError

\brief Displays a message and waits for any key to be pressed.
Finishes the program with 'errorCode' code.
------------------------------------------------------------------------------*/
void throwError(size_t errorCode, std::string const& text)
{
    std::cout << "Error! " << text << std::endl;
    std::cout << "Press any key to show HELP." << std::endl;
    getch();   
    ::showHelp();
    exit(errorCode); // TODO: is it safe (?)
}


/**-----------------------------------------------------------------------------
Function: normalizePath

\brief // TODO:
------------------------------------------------------------------------------*/
void normalizePath(std::string& path)
{
    for(auto &c : path)
    {
        if(c == '/')
            c = '\\';
    }
    if(path.back() != '\\')
        path.push_back('\\');
}

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
\brief Returns path of this program
------------------------------------------------------------------------------*/
std::string calcExecutablePath()
{
    LPSTR programPath = new CHAR[MAX_PATH];
    GetModuleFileNameA(GetModuleHandle(NULL), programPath, MAX_PATH); // TODO: use argv[0] istead of this shit (?)
    std::string ret(programPath);
     do // TODO: it can be implemented faster (?)
    {
        ret.pop_back();
    } while (ret.at(ret.size() - 1) != '\\');
    return ret;
}

/**-----------------------------------------------------------------------------
Function: calcTempDirectoryName
\brief Creates a string in format "tmpMMDDYYYY_HHMMSS"
------------------------------------------------------------------------------*/
std::string calcTempDirectoryName(std::string const& prefix)
{
    time_t tt;
    time( &tt );
    tm TM = *localtime( &tt );

    std::string retValue = prefix;    
    retValue += std::to_string(TM.tm_year + 1900);
    if(TM.tm_mon < 10)
        retValue.push_back('0');
    retValue += std::to_string(TM.tm_mon);
    if(TM.tm_mday < 10)
        retValue.push_back('0');
    retValue += std::to_string(TM.tm_mday);    
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
Function: calcSafeTempDirectoryPath

\brief Returns a path two directories above the source code.
------------------------------------------------------------------------------*/
std::string calcSafeTempDirectoryPath(std::string const& sourceCodePath)
{
    std::string retValue = sourceCodePath;
    size_t counter = 0;

    while(retValue.size() != 0)
    {
        if(retValue.back() == '/' || retValue.back() == '\\')
            counter++;

        if(counter == 2)
            break;
        retValue.pop_back();
    }

   return retValue;
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
void deploy(std::string const& regressionScriptsPath, std::string const& sourceCodePath, bool isSafeMode, bool isFullReport)
{

    std::string tempDirectoryPath;

    
    std::string warningMessage;
    if(isSafeMode)
    {
        tempDirectoryPath = ::calcSafeTempDirectoryPath(sourceCodePath);
        std::cout << "The program is launched with the -safe option." << std::endl;
    }
    else
    {
        tempDirectoryPath = calcExecutablePath();
        if(::calcSafeTempDirectoryPath(sourceCodePath) != tempDirectoryPath)
        {
            std::string warningMessage;
            warningMessage = "Warning! The program is launched without the -safe option.\n";
            warningMessage += "A temporary environments directory will be created in the:\n";
            warningMessage += tempDirectoryPath;
            warningMessage += "\nTo successful deploying of the environments, the relative paths to the source code will be changed.\n";
            warningMessage += "This can make it difficult to deploy this environments in the future.\n";
            warningMessage += "Ways to solve this:\n";
            warningMessage += " 1) Re-execute this program from safe dir (in this case its: " + ::calcSafeTempDirectoryPath(sourceCodePath) + ");\n";
            warningMessage += " 2) Re-Execute this program with -safe parameter\n";
            warningMessage += " 3) Press any key to ignore this warning and deploy environments in the directory: " + tempDirectoryPath;
            ::showMessage(warningMessage);
            warningMessage.clear();
            warningMessage = "Ok...";
        }
    }    
    
    std::string tempDirFullPath = tempDirectoryPath + ::calcTempDirectoryName("tmp") + "\\";
    warningMessage += "The environments will be copied in the directory: " + tempDirFullPath; 
    ::showMessage(warningMessage);
    /* create tmp dir */
    CreateDirectoryA(tempDirFullPath.c_str(), NULL);

    /* Calc the backsteps count from the temp dir to the drive root */  
    size_t backsteps = ::calcBackstepsToDriveRoot(tempDirFullPath);    
    /* Copy environments in the temporary dir */
    std::vector<File> copies = ::copyEnvironments(regressionScriptsPath, tempDirFullPath, true);
    /* process the copies */
    for(auto &f : copies)
    {
        // TODO: throw an error if backsteps != 2 in the safe mode(!)
        if(!isSafeMode && f.getExtension() == "env")        
            ::editEnvFile(f, backsteps);        
        else if(f.getExtension() == "bat")        
            ::editBatFile(f, isSafeMode? 2 : backsteps, isFullReport);        
    }
    /* Execute the copies of .bat files to deploy environments */
    size_t counter = 0;
    for(auto &f : copies)
    {
        if(f.getExtension() == "bat") // TODO: move this in previous cycle
        {
            std::cout << "Executing " << f.getFullName() << "..." << std::endl;
            std::string runCmd = (std::string("start /wait /d ") + f.getPath() + " cmd /c " + f.getFullPath());
            system(runCmd.c_str());
            counter++;
        }
    }   
    std::cout << counter << " environments has been succcessfully deployed, press any key to exit...";
    getch();
}


/**-----------------------------------------------------------------------------
Function: fixpaths

\brief TODO: 
------------------------------------------------------------------------------*/
/*void fixpaths(std::string const& regressionScriptsPath, size_t backstepsCount)
{
    std::vector<File> envFiles = ::findFiles(regressionScriptsPath, "*.env", true);
   
    for(auto & envFile: envFiles)
    {
        std::vector<File> batFiles = ::findFiles(envFile.getPath(), envFile.getName() + ".bat", false);
        if(batFiles.size() != 0)
        {
            ::editBatFile(batFiles[0], backstepsCount, false);
            ::editEnvFile(envFile, backstepsCount);
        }
    }
}*/