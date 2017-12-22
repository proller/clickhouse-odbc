#include "typeinfo.h"
#include <iostream>

namespace Typeinfo {
const std::map<std::string, TypeInfo> types_info =
{
    { "UInt8",       TypeInfo{ "TINYINT",   true,    SQL_TINYINT,         3,  1, false } },
    { "UInt16",      TypeInfo{ "SMALLINT",  true,    SQL_SMALLINT,        5,  2, false  } },
    { "UInt32",      TypeInfo{ "INT",       true,    SQL_INTEGER,         10, 4, false  } },
    { "UInt64",      TypeInfo{ "BIGINT",    true,    SQL_BIGINT,          19, 8, false  } },
    { "Int8",        TypeInfo{ "TINYINT",   false,   SQL_TINYINT,         3,  1, true  } },
    { "Int16",       TypeInfo{ "SMALLINT",  false,   SQL_SMALLINT,        5,  2, true  } },
    { "Int32",       TypeInfo{ "INT",       false,   SQL_INTEGER,         10, 4, true  } },
    { "Int64",       TypeInfo{ "BIGINT",    false,   SQL_BIGINT,          20, 8, true  } },
    { "Float32",     TypeInfo{ "REAL",      false,   SQL_REAL,            7,  4, true  } },
    { "Float64",     TypeInfo{ "DOUBLE",    false,   SQL_DOUBLE,          15, 8, true  } },
    { "String",      TypeInfo{ "TEXT",      true,    SQL_VARCHAR,         0xFFFFFF, (1 << 20), true  } },
    { "FixedString", TypeInfo{ "TEXT",      true,    SQL_VARCHAR,         0xFFFFFF, (1 << 20), false  } },
    { "Date",        TypeInfo{ "DATE",      true,    SQL_TYPE_DATE,       10, 6, true } },
    { "DateTime",    TypeInfo{ "TIMESTAMP", true,    SQL_TYPE_TIMESTAMP,  19, 16, true } },
    { "Array",       TypeInfo{ "TEXT",      true,    SQL_VARCHAR,         0xFFFFFF, (1 << 20), false } },
};

const std::map<std::string, std::string> convert_sql_odbc_to_clickhouse = [] {
    std::map<std::string, std::string> r;
        for (const auto type: types_info) {
            if (!type.second.convertible) continue;
            std::cerr << type.second.sql_type_name << " = "<<type.first << "\n";
            r["SQL_" + type.second.sql_type_name]=type.first;
        }
        return r;
}();

}
