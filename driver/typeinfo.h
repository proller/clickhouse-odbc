#pragma once

#include <map>
#include <string>
#include <sqlext.h>

struct TypeInfo
{
    std::string sql_type_name;
    bool is_unsigned;
    SQLSMALLINT sql_type;
    size_t column_size;
    size_t octet_length;
    bool convertible;

    inline bool IsIntegerType() const
    {
        return
            sql_type == SQL_TINYINT || sql_type == SQL_SMALLINT ||
            sql_type == SQL_INTEGER || sql_type == SQL_BIGINT;
    }

    inline bool IsStringType() const
    {
        return sql_type == SQL_VARCHAR;
    }
};


namespace Typeinfo
{
    extern const std::map<std::string, TypeInfo> types_info;
    extern const std::map<std::string, std::string> convert_sql_odbc_to_clickhouse;
};
