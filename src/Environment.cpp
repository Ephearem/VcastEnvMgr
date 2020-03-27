#include "Environment.hpp"
#include "files.hpp"

Environment::Environment(std::filesystem::path const& env, std::filesystem::path const& bat, std::filesystem::path const& tst)
    :env_(env), bat_(bat), tst_(tst)
{}


/**-----------------------------------------------------------------------------
Function: Environment::moveCopy

\brief Copies the environment files in the 'to/envName' directory;
changes this instance paths
------------------------------------------------------------------------------*/
void Environment::moveCopy(std::filesystem::path const& to)
{
    /* create environment directory in the temp directory */
    std::filesystem::path newEnvDir = to / this->getName();
    std::filesystem::create_directory(newEnvDir);

    // TODO: process errors
    std::filesystem::copy_file(this->env_, newEnvDir / this->env_.filename());
    std::filesystem::copy_file(this->bat_, newEnvDir / this->bat_.filename());
    std::filesystem::copy_file(this->tst_, newEnvDir / this->tst_.filename());

    this->env_ = newEnvDir / this->env_.filename();
    this->bat_ = newEnvDir / this->bat_.filename();
    this->tst_ = newEnvDir / this->tst_.filename();
    // TODO: call std::filesystem::exists() (?)
}


/**-----------------------------------------------------------------------------
Function: Environment::moveCopy

\brief Replaces management report generation instruction to full report generation instruction.
------------------------------------------------------------------------------*/
void Environment::manageToFull() const
{
    std::vector<std::string> fileData = getFileDataAsStringsVector(this->bat_);
    std::vector<std::string> newFfileData;
    for(auto &str : fileData)
    {        
        size_t offset;
    
        /* replace management report generation instruction to full report generation instruction*/
        offset = str.find("reports custom management");
        if (offset != std::string::npos)
        {
            size_t managementWordPos = str.find(" management ");
            str.replace(managementWordPos, 6, " full ");
            str.erase(managementWordPos + 6, 6);

            size_t reportNamePos = str.find("_management_report.html"); // TODO: create a constant for this token (?)
            str.replace(reportNamePos, 17, "_FULL_REPORT.html"); // TODO: get postfix in .exe start params (?)
            str.erase(reportNamePos + 17, 6);
        }
    
        newFfileData.push_back(str);
    }
    writeFileDataFromStringVector(this->bat_, newFfileData);  
}

 void Environment::deploy() const
 {
    std::string runCmd = (std::string("start /wait /d ") + this->bat_.parent_path().string() + " cmd /c " + this->bat_.string());
    system(runCmd.c_str());
 }