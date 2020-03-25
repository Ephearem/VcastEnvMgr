#include <iostream>
#include "core.hpp"

int main(int argc, char** argv)
{  

    if(argc < 3)
    {
        std::string error = "Wrong number of parameters. Allowable templates:\n";
        error += "-deploy rs=[regression scripts path] src=[source code path] [flags]\n";
        ::throwError(1, error); 
    }    

    std::string rsPath;
    std::string srcPath;
    bool isSafe = true;
    bool isFullReport = false;
    if(std::string(argv[1]) == "-deploy")
    {
        for(int i = 2; i < argc; i++)
        {
            std::string curStr = argv[i];
            if(curStr.find("rs=") != std::string::npos)
                rsPath = curStr.substr(3, curStr.size() - 3);
            else if(curStr.find("src=") != std::string::npos)
                srcPath = curStr.substr(4, curStr.size() - 4);
            else if(curStr == "-full")
                isFullReport = true;
                
        }
    }
    else
    {
        std::string errorMsg = "Unknown parameter [";
        errorMsg += argv[1];
        errorMsg += "].";
        ::throwError(1, errorMsg);
    }
    if(rsPath.empty())
        ::throwError(1, "Missing parameter [rs=REGRESSION_SCRIPTS_PATH]");
    if(srcPath.empty())
        ::throwError(1, "Missing parameter [rs=SOURCE_PATH]");
    
    ::normalizePath(rsPath);
    ::normalizePath(srcPath);
    ::deploy(rsPath, srcPath, isSafe, isFullReport); 


    return 0;
}
