#include "Environment.hpp"
#include "files.hpp"

Environment::Environment(std::filesystem::path const& env, std::filesystem::path const& bat, std::filesystem::path const& tst)
    :env_(env), bat_(bat), tst_(tst)
{}


/**-----------------------------------------------------------------------------
Function: Environment::moveCopy

\brief Copies the environment files in the 'to/envName' directory;
changes this instance paths.
------------------------------------------------------------------------------*/
void Environment::moveCopy(std::filesystem::path const& to)
{
    /* create environment directory in the temp directory */
    std::filesystem::path newEnvDir = to / this->getName();
    std::filesystem::create_directory(newEnvDir);

    // TODO: (!) process errors
    std::filesystem::copy_file(this->env_, newEnvDir / this->env_.filename());
    std::filesystem::copy_file(this->bat_, newEnvDir / this->bat_.filename());
    std::filesystem::copy_file(this->tst_, newEnvDir / this->tst_.filename());

    this->env_ = newEnvDir / this->env_.filename();
    this->bat_ = newEnvDir / this->bat_.filename();
    this->tst_ = newEnvDir / this->tst_.filename();
    // TODO: (!) call std::filesystem::exists() (?)
}


/**-----------------------------------------------------------------------------
Function: Environment::manageToFull

\brief Replaces management report generation instruction to full report generation instruction.
------------------------------------------------------------------------------*/
void Environment::manageToFull() const
{
    std::vector<std::string> fileData = getFileDataAsStringsVector(this->bat_);
    std::vector<std::string> newFfileData;
    for(auto &str : fileData)
    {        
        size_t offset;
    
        /* replace management report generation instruction to full report generation instruction */
        offset = str.find("reports custom management");
        if (offset != std::string::npos)
        {
            size_t managementWordPos = str.find(" management ");
            str.replace(managementWordPos, 6, " full ");
            str.erase(managementWordPos + 6, 6);

            size_t reportNamePos = str.find("_management_report.html"); // TODO: (!) create a constant for this token (?)
            str.replace(reportNamePos, 17, "_FULL_REPORT.html"); // TODO: (!) get postfix in .exe start params (?)
            str.erase(reportNamePos + 17, 6);
        }
    
        newFfileData.push_back(str);
    }
    writeFileDataFromStringVector(this->bat_, newFfileData);  
}


/**-----------------------------------------------------------------------------
Function: Environment::disableSbfTemplates

\brief Disables options checkbox 'Enable SBF capability for template functions'.
// TODO: (!) add comments
------------------------------------------------------------------------------*/
void Environment::disableSbfTemplates() const
{
    std::vector<std::string> fileData = getFileDataAsStringsVector(this->bat_);
    std::vector<std::string> newFfileData;
    size_t optionSectionEndIndex = 0;
    bool isSbfTemplatesAlreadyEnabled = false;

    // TODO: (!) Don't create newFileData vector. Just add 1 string in fileData vector.
    for(size_t i = 0; i < fileData.size(); i++)
    {
        size_t offset = std::string::npos; // TODO: (!) delete me, use 'str.find' in the if-cond. block
        if(!isSbfTemplatesAlreadyEnabled)
        {           
            if((fileData[i]).find("echo options") != std::string::npos) /* if current string is options entry */
            {
                offset = (fileData[i]).find("echo options VCAST_SBF_TEMPLATES FALSE >> commands.tmp"); // TODO: (?) std::substring will be faster?
                if (offset != std::string::npos)
                {
                    isSbfTemplatesAlreadyEnabled = true;
                }  
                optionSectionEndIndex = i; 
            }         
        }    
        newFfileData.push_back(fileData[i]); /* write current string in the .bat file */
    }

    if(isSbfTemplatesAlreadyEnabled == false)
    {
        newFfileData.insert(newFfileData.begin() + optionSectionEndIndex, "echo options VCAST_SBF_TEMPLATES FALSE >> commands.tmp");
    }
    writeFileDataFromStringVector(this->bat_, newFfileData);
}


/**-----------------------------------------------------------------------------
Function: Environment::disableSbfTemplates

\brief // TODO: (!) write me
------------------------------------------------------------------------------*/
 void Environment::deploy() const
 {
    std::string runCmd = (std::string("start /wait /d ") + this->bat_.parent_path().string() + " cmd /c " + this->bat_.string());
    system(runCmd.c_str());
 }