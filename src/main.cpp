#include <iostream>
#include "core.hpp"


void showMessage(std::string const& text)
{
    std::cout << text << std::endl;
    system("pause");
}


void throwError(size_t errorCode, std::string const& text)
{
    ::showMessage(std::string("Error! ") + text);
    exit(errorCode); // TODO: is it safe (?)
}


int main(int argc, char** argv)
{
    /* parse params */
    enum class enMode { NO, DEPLOY, FIXPATH} mode = enMode::NO;
    if(argc >= 2)
    {
        std::string arg1 = argv[1];
        if(arg1 == "-deploy")
        {
            if(argc != 4)
            {
                ::throwError(1, "Wrong number of parameters. Use -deploy [regression scripts path] [source code path]");
            }
            // TODO: check paths and throw an error if they are invalid
            mode = enMode::DEPLOY;
        }
        else if(arg1 == "-fixpaths")
        {
            if(argc != 4)
            {
                ::throwError(1, "Wrong number of parameters. Use -fixpaths [regression scripts path] [number of backsteps (../)]");               
            }
            mode = enMode::FIXPATH;
        }
        else
        {
            ::throwError(1, std::string("Unknown parameter [" + arg1 + "]"));
        } 
    }
    else
    {
        std::string error = "Wrong number of parameters. Allowable templates:\n";
        error += "-deploy [regression scripts path] [source code path]\n";
        error += "-fixpaths [regression scripts path] [number of backsteps (../)]\n";
        ::throwError(1, error);  
    }
    

    /* processing  */
    switch(mode)
    {
        case enMode::DEPLOY:
            ::deploy(argv[2], argv[3]);
            break;       

        case enMode::FIXPATH:        
            ::fixpaths(argv[2], std::stoi(argv[3]));
            std::cout << "Paths has been changed." << std::endl;
            break;        
    }

    system("pause");
    return 0;
}
