#include "environment.h"
#include "connection.h"

#include <cstdio>
#include <ctime>
#include <sstream>
#include <string>
#include "unicode_t.h"

#if defined(_unix_)
#    include <pwd.h>
#    include <unistd.h>
#endif


const std::map<std::string, TypeInfo> Environment::types_info = {
    {"UInt8", TypeInfo {"TINYINT", true, SQL_TINYINT, 3, 1}},
    {"UInt16", TypeInfo {"SMALLINT", true, SQL_SMALLINT, 5, 2}},
    {"UInt32",
        TypeInfo {"INT",
            true,
            SQL_BIGINT /* was SQL_INTEGER */,
            10,
            4}}, // With perl, python ODBC drivers INT is uint32 and it cant store values bigger than 2147483647: 2147483648 -> -2147483648 4294967295 -> -1
    {"UInt32", TypeInfo {"INT", true, SQL_INTEGER, 10, 4}},
    {"UInt64", TypeInfo {"BIGINT", true, SQL_BIGINT, 20, 8}},
    {"Int8", TypeInfo {"TINYINT", false, SQL_TINYINT, 1 + 3, 1}}, // one char for sign
    {"Int16", TypeInfo {"SMALLINT", false, SQL_SMALLINT, 1 + 5, 2}},
    {"Int32", TypeInfo {"INT", false, SQL_INTEGER, 1 + 10, 4}},
    {"Int64", TypeInfo {"BIGINT", false, SQL_BIGINT, 1 + 19, 8}},
    {"Float32", TypeInfo {"REAL", false, SQL_REAL, 7, 4}},
    {"Float64", TypeInfo {"DOUBLE", false, SQL_DOUBLE, 15, 8}},
    {"Decimal", TypeInfo {"DECIMAL", false, SQL_DECIMAL, 1 + 2 + 38, 16}}, // -0.
    {"String", TypeInfo {"TEXT", true, SQL_VARCHAR, Environment::string_max_size, Environment::string_max_size}},
    {"FixedString", TypeInfo {"TEXT", true, SQL_VARCHAR, Environment::string_max_size, Environment::string_max_size}},
    {"Date", TypeInfo {"DATE", true, SQL_TYPE_DATE, 10, 6}},
    {"DateTime", TypeInfo {"TIMESTAMP", true, SQL_TYPE_TIMESTAMP, 19, 16}},
    {"Array", TypeInfo {"TEXT", true, SQL_VARCHAR, Environment::string_max_size, Environment::string_max_size}},

    {"LowCardinality(String)",
        TypeInfo {"TEXT", true, SQL_VARCHAR, Environment::string_max_size, Environment::string_max_size}}, // todo: remove
    {"LowCardinality(FixedString)",
        TypeInfo {"TEXT", true, SQL_VARCHAR, Environment::string_max_size, Environment::string_max_size}}, // todo: remove
};

Environment::Environment(Driver & driver)
    : ChildType(driver)
{
}

const TypeInfo & Environment::getTypeInfo(const std::string & type_name, const std::string & type_name_without_parametrs) const {
    if (types_info.find(type_name) != types_info.end())
        return types_info.at(type_name);
    if (types_info.find(type_name_without_parametrs) != types_info.end())
        return types_info.at(type_name_without_parametrs);
    LOG("Unsupported type " << type_name << " : " << type_name_without_parametrs);
    throw SqlException("Unsupported type = " + type_name, "HY004");
}

template <>
Connection& Environment::allocateChild<Connection>() {
    auto child_sptr = std::make_shared<Connection>(*this);
    auto& child = *child_sptr;
    auto handle = child.getHandle();
    connections.emplace(handle, std::move(child_sptr));
    return child;
}

template <>
void Environment::deallocateChild<Connection>(SQLHANDLE handle) noexcept {
    connections.erase(handle);
}
