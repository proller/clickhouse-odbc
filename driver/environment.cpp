#include "environment.h"

#if defined (_unix_)
#   include <stdio.h>
#   include <unistd.h>
#   include <pwd.h>
#endif

Environment::Environment()
{
#if defined (_unix_)
    struct passwd *pw;
    uid_t uid;
    std::string stderr_path = "/tmp/clickhouse-odbc-stderr";
    uid = geteuid();
    pw = getpwuid(uid);
    if (pw)
    {
        stderr_path += "." + std::string(pw->pw_name);
    }
    if (!freopen(stderr_path.c_str(), "w", stderr))
        throw std::logic_error("Cannot freopen stderr.");
#endif
}

Environment::~Environment()
{ }
