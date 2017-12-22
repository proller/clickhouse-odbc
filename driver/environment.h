#pragma once

#include "diagnostics.h"

#include <stdexcept>

struct Environment
{
    Environment();
    ~Environment();

    SQLUINTEGER metadata_id = SQL_FALSE;
    int odbc_version = SQL_OV_ODBC3_80;
    DiagnosticRecord diagnostic_record;
};
