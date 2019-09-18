#include "descriptor.h"

void DescriptorRecord::onAttrChange(int attr) {
    switch (attr) {
        case SQL_DESC_TYPE: {
            const auto type = getAttrAs<SQLSMALLINT>(SQL_DESC_TYPE);
            if (isConciseDateTimeIntervalType(type)) {
                throw SqlException("Inconsistent descriptor information", "HY021");
            }
            else if (isVerboseType(type)) {
                const auto code = getAttrAs<SQLSMALLINT>(SQL_DESC_DATETIME_INTERVAL_CODE, 0);
                if (code) {
                    setAttrSilent(SQL_DESC_CONCISE_TYPE, convertDateTimeIntervalCodeToSQLType(code, type));
                    if (getAttrAs<SQLPOINTER>(SQL_DESC_DATA_PTR, 0))
                        setAttrSilent(SQL_DESC_DATA_PTR, 0);
                }
            }
            else {
                setAttrSilent(SQL_DESC_CONCISE_TYPE, type);
                setAttrSilent(SQL_DESC_DATETIME_INTERVAL_CODE, 0);
                if (getAttrAs<SQLPOINTER>(SQL_DESC_DATA_PTR, 0))
                    setAttrSilent(SQL_DESC_DATA_PTR, 0);
            }


            // TODO: reset all non-type attributes?


            switch (type) {
                case SQL_CHAR:
                case SQL_VARCHAR:
                    setAttrSilent(SQL_DESC_LENGTH, 1);
                    setAttrSilent(SQL_DESC_PRECISION, 0);
                    break;

                case SQL_DECIMAL:
                case SQL_NUMERIC:
                    setAttrSilent(SQL_DESC_SCALE, 0);
                    setAttrSilent(SQL_DESC_PRECISION, 6); // TODO: fix this.
                    break;

                case SQL_FLOAT:
                    setAttrSilent(SQL_DESC_PRECISION, 6); // TODO: fix this.
                    break;

                case SQL_DATETIME: {
                    const auto code = getAttrAs<SQLSMALLINT>(SQL_DESC_DATETIME_INTERVAL_CODE, 0);
                    switch (code) {
                        case SQL_CODE_DATE:
                        case SQL_CODE_TIME:
                            setAttrSilent(SQL_DESC_PRECISION, 0);
                            break;

                        case SQL_CODE_TIMESTAMP:
                            setAttrSilent(SQL_DESC_PRECISION, 6);
                            break;
                    }
                    break;
                }

                case SQL_INTERVAL: {
                    const auto code = getAttrAs<SQLSMALLINT>(SQL_DESC_DATETIME_INTERVAL_CODE, 0);
                    if (isIntervalCode(code)) {
                        setAttrSilent(SQL_DESC_DATETIME_INTERVAL_PRECISION, 2);
                    }
                    if (intervalCodeHasSecondComponent(code)) {
                        setAttrSilent(SQL_DESC_PRECISION, 6);
                    }
                    break;
                }
            }

            break;
        }
        case SQL_DESC_CONCISE_TYPE: {
            const auto type = getAttrAs<SQLSMALLINT>(SQL_DESC_CONCISE_TYPE);
            if (isConciseDateTimeIntervalType(type)) {
                setAttrSilent(SQL_DESC_DATETIME_INTERVAL_CODE, convertSQLTypeToDateTimeIntervalCode(type));
                setAttr(SQL_DESC_TYPE, tryConvertSQLTypeToVerboseType(type));
                if (getAttrAs<SQLPOINTER>(SQL_DESC_DATA_PTR, 0))
                    setAttrSilent(SQL_DESC_DATA_PTR, 0);
            }
            else if (!isVerboseType(type)) {
                setAttrSilent(SQL_DESC_DATETIME_INTERVAL_CODE, 0);
                setAttr(SQL_DESC_TYPE, type);
                if (getAttrAs<SQLPOINTER>(SQL_DESC_DATA_PTR, 0))
                    setAttrSilent(SQL_DESC_DATA_PTR, 0);
            }
            break;
        }
        case SQL_DESC_DATETIME_INTERVAL_CODE: {
            const auto code = getAttrAs<SQLSMALLINT>(SQL_DESC_DATETIME_INTERVAL_CODE);
            const auto type = getAttrAs<SQLSMALLINT>(SQL_DESC_TYPE, SQL_UNKNOWN_TYPE);
            if (code && isVerboseType(type)) {
                setAttrSilent(SQL_DESC_CONCISE_TYPE, convertDateTimeIntervalCodeToSQLType(code, type));
                if (getAttrAs<SQLPOINTER>(SQL_DESC_DATA_PTR, 0))
                    setAttrSilent(SQL_DESC_DATA_PTR, 0);
            }
            break;
        }
        case SQL_DESC_DATA_PTR: {
            if (getAttrAs<SQLPOINTER>(SQL_DESC_DATA_PTR, nullptr))
                consistencyCheck();
        }
    }
}

bool DescriptorRecord::hasColumnSize() const {

    // TODO

    return false;
}

SQLULEN DescriptorRecord::getColumnSize() const {

    // TODO

    return 0;
}

bool DescriptorRecord::hasDecimalDigits() const {

    // TODO

    return false;
}

SQLSMALLINT DescriptorRecord::getDecimalDigits() const {

    // TODO

    return 0;
}

void DescriptorRecord::consistencyCheck() {

    // TODO

}

Descriptor::Descriptor(Connection & connection)
    : ChildType(connection)
{
    setAttr(SQL_DESC_COUNT, 0);
}

std::size_t Descriptor::getRecordCount() const {
    return getAttrAs<SQLSMALLINT>(SQL_DESC_COUNT, 0);
}

DescriptorRecord & Descriptor::getRecord(std::size_t num, SQLINTEGER current_role) {
    const auto curr_rec_count = getAttrAs<SQLSMALLINT>(SQL_DESC_COUNT, 0) + 1; // +1 for 0th BOOKMARK record

    for (std::size_t i = curr_rec_count; i <= num && i < records.size(); ++i) {
        getParent().initAsDescRec(records[i], current_role);
    }

    while (records.size() < curr_rec_count || records.size() <= num) {
        records.emplace_back();
        getParent().initAsDescRec(records.back(), current_role);
    }

    if (curr_rec_count <= num) {
        setAttr(SQL_DESC_COUNT, num);
    }

    return records[num];
}
