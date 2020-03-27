#pragma once
#include <filesystem>
#include <string>
#include <vector>


class Environment
{
public:
    Environment(std::filesystem::path const& env, std::filesystem::path const& bat, std::filesystem::path const& tst);
    inline std::filesystem::path getEnv() const { return this->env_; }
    inline std::filesystem::path getBat() const { return this->bat_; }
    inline std::filesystem::path getTst() const { return this->tst_; }
    inline std::string getName() const { return this->env_.stem().string(); }
    void moveCopy(std::filesystem::path const& to);
    void manageToFull() const; // TODO: mb not const (?) (!)
    void deploy() const;
private:
    std::filesystem::path env_;
    std::filesystem::path bat_;
    std::filesystem::path tst_;
};