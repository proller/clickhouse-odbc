#include "driver.h"
#include "environment.h"
#include "connection.h"
#include "descriptor.h"
#include "statement.h"
#include "utils.h"

#include <Poco/Net/HTTPClientSession.h>

#include <type_traits>

namespace {

SQLRETURN allocEnv(SQLHENV * out_environment_handle) noexcept {
    return CALL([&] () {
        if (nullptr == out_environment_handle)
            return SQL_INVALID_HANDLE;

        *out_environment_handle = Driver::getInstance().allocateChild<Environment>().getHandle();
        return SQL_SUCCESS;
    });
}

SQLRETURN allocConnect(SQLHENV environment_handle, SQLHDBC * out_connection_handle) noexcept {
    return CALL_WITH_HANDLE(environment_handle, [&] (Environment & environment) {
        if (nullptr == out_connection_handle)
            return SQL_INVALID_HANDLE;

        *out_connection_handle = environment.allocateChild<Connection>().getHandle();
        return SQL_SUCCESS;
    });
}

SQLRETURN allocStmt(SQLHDBC connection_handle, SQLHSTMT * out_statement_handle) noexcept {
    return CALL_WITH_HANDLE(connection_handle, [&] (Connection & connection) {
        if (nullptr == out_statement_handle)
            return SQL_INVALID_HANDLE;

        *out_statement_handle = connection.allocateChild<Statement>().getHandle();
        return SQL_SUCCESS;
    });
}

SQLRETURN allocDesc(SQLHDBC connection_handle, SQLHDESC * out_descriptor_handle) noexcept {
    return CALL_WITH_HANDLE(connection_handle, [&] (Connection & connection) {
        if (nullptr == out_descriptor_handle)
            return SQL_INVALID_HANDLE;

        auto & descriptor = connection.allocateChild<Descriptor>();
        connection.initAsAD(descriptor, true);
        *out_descriptor_handle = descriptor.getHandle();
        return SQL_SUCCESS;
    });
}

SQLRETURN freeHandle(SQLHANDLE handle) noexcept {
    return CALL_WITH_HANDLE_SKIP_DIAG(handle, [&] (auto & object) {
        if ( // Refuse to manually deallocate an automatically allocated descriptor.
            std::is_convertible<std::decay<decltype(object)> *, Descriptor *>::value &&
            object.template getAttrAs<SQLSMALLINT>(SQL_DESC_ALLOC_TYPE) != SQL_DESC_ALLOC_USER
        ) {
            return SQL_ERROR;
        }

        object.deallocateSelf();
        return SQL_SUCCESS;
    });
}

} // namespace


extern "C" {

SQLRETURN SQL_API SQLAllocHandle(SQLSMALLINT handle_type, SQLHANDLE input_handle, SQLHANDLE * output_handle) {
    LOG(__FUNCTION__ << " handle_type=" << handle_type << " input_handle=" << input_handle);

    switch (handle_type) {
        case SQL_HANDLE_ENV:
            return allocEnv((SQLHENV *)output_handle);
        case SQL_HANDLE_DBC:
            return allocConnect((SQLHENV)input_handle, (SQLHDBC *)output_handle);
        case SQL_HANDLE_STMT:
            return allocStmt((SQLHDBC)input_handle, (SQLHSTMT *)output_handle);
        case SQL_HANDLE_DESC:
            return allocDesc((SQLHDBC)input_handle, (SQLHDESC *)output_handle);
        default:
            LOG("AllocHandle: Unknown handleType=" << handle_type);
            return SQL_ERROR;
    }
}

SQLRETURN SQL_API SQLAllocEnv(SQLHDBC * output_handle) {
    LOG(__FUNCTION__);
    return allocEnv(output_handle);
}

SQLRETURN SQL_API SQLAllocConnect(SQLHENV input_handle, SQLHDBC * output_handle) {
    LOG(__FUNCTION__ << " input_handle=" << input_handle);
    return allocConnect(input_handle, output_handle);
}

SQLRETURN SQL_API SQLAllocStmt(SQLHDBC input_handle, SQLHSTMT * output_handle) {
    LOG(__FUNCTION__ << " input_handle=" << input_handle);
    return allocStmt(input_handle, output_handle);
}

SQLRETURN SQL_API SQLFreeHandle(SQLSMALLINT handleType, SQLHANDLE handle) {
    LOG(__FUNCTION__ << " handleType=" << handleType << " handle=" << handle);

    switch (handleType) {
        case SQL_HANDLE_ENV:
        case SQL_HANDLE_DBC:
        case SQL_HANDLE_STMT:
        case SQL_HANDLE_DESC:
            return freeHandle(handle);
        default:
            LOG("FreeHandle: Unknown handleType=" << handleType);
            return SQL_ERROR;
    }
}

SQLRETURN SQL_API SQLFreeEnv(HENV handle) {
    LOG(__FUNCTION__);
    return freeHandle(handle);
}

SQLRETURN SQL_API SQLFreeConnect(HDBC handle) {
    LOG(__FUNCTION__);
    return freeHandle(handle);
}

SQLRETURN SQL_API SQLFreeStmt(HSTMT statement_handle, SQLUSMALLINT option) {
    LOG(__FUNCTION__ << " option=" << option);

    return CALL_WITH_HANDLE(statement_handle, [&] (Statement & statement) -> SQLRETURN {
        switch (option) {
            case SQL_CLOSE: /// Close the cursor, ignore the remaining results. If there is no cursor, then noop.
                statement.closeCursor();
                return SQL_SUCCESS;

            case SQL_DROP:
                return freeHandle(statement_handle);

            case SQL_UNBIND:
                statement.resetColBindings();
                return SQL_SUCCESS;

            case SQL_RESET_PARAMS:
                statement.resetParamBindings();
                return SQL_SUCCESS;
        }

        return SQL_ERROR;
    });
}

} // extern "C"
