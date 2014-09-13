#ifndef _session_hxx_
#define _session_hxx_

#include <string>
#include <map>
#include <vector>

class Session
{
public:
    Session();
    bool processCommand(const std::vector<std::string>& argv);

    std::vector<std::string> getEnvironmentVariableNames();
    std::string getEnvironmentVariable(const std::string& name);
    void setEnvironmentVariable(const std::string& name, const std::string& value);

private:
    std::map<std::string, std::string> m_environment;
};

#endif
