# Todo: test and fix in unicode iodbc
# NOT UNICODE AND 
if (EXISTS "${PROJECT_SOURCE_DIR}/contrib/nanodbc/CMakeLists.txt" )
    set (USE_INTERNAL_NANOODBC_LIBRARY 1)
    set (NANOODBC_LIBRARY nanodbc)
endif ()
message (STATUS "Using nanoodbc=${USE_INTERNAL_NANOODBC_LIBRARY} : ${NANOODBC_LIBRARY}")
