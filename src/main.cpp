#include <iostream>
#include <string>
#include <ctime>
#include <filesystem>
#include "Environment.hpp"




/**-----------------------------------------------------------------------------
Function: showHelp

\brief Prints some useful information to the console.
------------------------------------------------------------------------------*/
void showHelp()
{
    std::wcout << std::endl;
    std::wcout << "VcastEnvMgr v0.01" << std::endl;
    std::wcout << " * * * HELP * * *" << std::endl;    
    std::wcout << std::endl;
    std::wcout << "Brief:" << std::endl;
    std::wcout << "This program recursively finds and copies all regression scripts to a temporary directory, deploys them (by running .bat files) and, if specified in the launch options, generates full reports." << std::endl;
    std::wcout << std::endl;
    std::wcout << "Some info:" << std::endl;
    std::wcout << "The name of the temporary directory is in the format: \"tmp_yyyymmdd_hhmmss\"." << std::endl;    
    std::wcout << "Most often, in the PSA repositories, the \"regression scripts\" directory is located in the same place as the source code. Therefore, The temporary directory will be located at /src/../ too." << std::endl;
    std::wcout << std::endl;
    std::wcout << "Allowable command templates:" << std::endl;
    std::wcout << "\t-deploy rs=[regression scripts path] src=[source code path] [flags]" << std::endl;    
    std::wcout << std::endl;
    std::wcout << "Allowable flags:" << std::endl;
    std::wcout << "\t-full - changes a management report generation instruction to a full report generation instruction." << std::endl;
    std::wcout << std::endl;
    std::wcout << "Commands and flags should be specified in the launch parameters." << std::endl;
    std::wcout << "Example: -deploy rs=F:\\VPR\\trunk\\regression_scripts\\Voting\\ src=F:\\VPR\\trunk\\src\\ -full" << std::endl;
    std::wcout << std::endl;
    std::wcout << " evgeny.gancharik@psa-software.com" << std::endl;
    std::wcout << " 04/22/2020" << std::endl;
    std::cout << std::endl;
}


/**-----------------------------------------------------------------------------
Function: error

\brief Logs an error and terminates the process with error code.
------------------------------------------------------------------------------*/
void error(std::string const& text, int code, bool isShowHelp = false)
{
    std::cout << "Error. " << text << std::endl;
    if(isShowHelp)
        showHelp();
    system("pause");
    exit(code);
}


/**-----------------------------------------------------------------------------
Function: calcTempDirectorySafePath

\brief Returns a "/src/../" path.
------------------------------------------------------------------------------*/
std::filesystem::path calcTempDirectorySafePath(std::filesystem::path const& sourcePath)
{
    std::filesystem::path retValue = sourcePath.parent_path();
    if(!std::filesystem::exists(retValue))
    {
        ::error("calcTempDirectorySafePath", 1);
    }
    return retValue;
}


/**-----------------------------------------------------------------------------
Function: calcTempDirectoryName

\brief Creates a string in format "tmpYYYYMMDD_HHMMSS"
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
Function: searchEnvs

\brief Searches environments in the directory specified in the 'path' argument.
The environment is considered to be the presence when:
there are three(3) files with the same name and with the extensions .env, .bat, .tst respectively in the 'path' dir.
// TODO: The presence of a .tst file is not checked, since it may not always be in a regression script (?)
// TODO: Make this function as a static function of the Environment class (?)
------------------------------------------------------------------------------*/
std::vector<Environment> searchEnvs(std::filesystem::path const& path)
{
    std::vector<Environment> retValue;

    for(auto& p: std::filesystem::recursive_directory_iterator(path))
    {
        if (!std::filesystem::is_regular_file(p.status()))
            continue;
        
        std::string name(p.path().filename().string());
        bool match = !name.compare(name.size() - 4, 4, ".env");
        if (!match)
            continue;   
        /* if .env found */     
        std::filesystem::path envFile = p.path();
        /* try find a .bat pair in the same directory with the same name */
        std::filesystem::path batFile(envFile);
        batFile.replace_extension(".bat");
        if(!std::filesystem::exists(batFile))
        {
            continue;
        }
        /* if .bat pair found  try find a .tst pair */
        std::filesystem::path tstFile(envFile);
        tstFile.replace_extension(".tst");
        if(!std::filesystem::exists(tstFile))
        {
            continue;
        }
        retValue.push_back(Environment(envFile,batFile,tstFile));
    }
    return retValue;
}



int main(int argc, char** argv)
{
    /* .exe full name; -deploy; rs=; src=; */
    if(argc < 4)
    {
        ::error("Invalid number of arguments. Read help for more information.", 1, true);
    }
    if(std::string(argv[1]) != "-deploy")
    {
        ::error("Invalid argument: " + std::string(argv[1]) + ". Read help for more information.", 1, true);
    }    
    
    std::filesystem::path regressionScripts;
    std::filesystem::path sourceCode;
    bool isFullReport = false;


    /* parse input args */
    for(int i = 2; i < argc; i++)
    {
        std::string curStr = argv[i];
        if(curStr.find("rs=") != std::string::npos)
            regressionScripts = curStr.substr(3, curStr.size() - 3);
        else if(curStr.find("src=") != std::string::npos)
            sourceCode = curStr.substr(4, curStr.size() - 4);
        else if(curStr == "-full")
            isFullReport = true;            
    }
   
    /* does regression scripts directory exist ? */
    if(!std::filesystem::exists(regressionScripts))
    {
        ::error("Invalid regression scripts directory (" + regressionScripts.string() + ")", 1);
    }
    /* does source code directory exist ? */
    if(!std::filesystem::exists(sourceCode))
    {
        ::error("Invalid source code directory (" + sourceCode.string() + ")", 1);
    }
  
    /* normalize */
    regressionScripts = std::filesystem::canonical(regressionScripts);
    sourceCode = std::filesystem::canonical(sourceCode);

    /* if can not read regression scripts dir */
    if((std::filesystem::status(regressionScripts).permissions() & std::filesystem::perms::owner_read) != std::filesystem::perms::owner_read)
    {
        ::error("There are no read permissions for the Regression scripts directory.", 1);
    }
    /* calc temp directory path */
    std::filesystem::path tmpDirPath = ::calcTempDirectorySafePath(sourceCode);
   
    /* if can not read write and execute in the temp dir */    
    if((std::filesystem::status(tmpDirPath.parent_path()).permissions() & std::filesystem::perms::owner_all) != std::filesystem::perms::owner_all)
    {
        ::error("There are no r-w-e permissions for the temp directory.", 1);
    }
    tmpDirPath /= ::calcTempDirectoryName("tmp");

    /* search environments */
    std::vector<Environment> envs = searchEnvs(regressionScripts);
    if(envs.empty())
    {
        ::error("There are no environments in directory: " + regressionScripts.string(), 1);
    }    
    /* log some info */
    std::cout << "Programm start with params: " << std::endl;
    std::cout << "Regression scripts directory: " << regressionScripts.string() << std::endl;
    std::cout << "Source code directory: " << sourceCode.string() << std::endl;
    std::cout << "Generate full reports: " << (isFullReport ? "true" : "false") << std::endl;
    std::cout << std::endl;

    std::cout << "There are " << envs.size() << " regression scripts found in the selected directory." << std::endl;
    /* create temp directory */
    std::filesystem::create_directory(tmpDirPath);
    std::cout << "Temporary directory created at: " << tmpDirPath.string() << std::endl;
    /* copy envs in the temp directory */
    for(auto & e : envs)
    {      
        e.moveCopy(tmpDirPath);  
        if(isFullReport) // TODO: it's so bad to check this for every loop iteration
        {
            e.manageToFull();           
        }
        std::cout << "Deploying " << e.getName() << "..." << std::endl;
        e.deploy(); 
    }

    std::cout << envs.size() << " regression scripts has been deployed." << std::endl;
    /* ENVIRONMENTS PROCESSING STUFF */
    
    system("pause");
    return 0;
}