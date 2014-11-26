#ifndef __DATA_TYPES__
#define __DATA_TYPES__

#include <iostream>

enum DataType {
	TYPE_INT,
	TYPE_LONG,
	TYPE_FLOAT,
	TYPE_DOUBLE
};

std::string DataType_to_string(DataType d_type){
	switch (d_type){
		case TYPE_INT:
		{
			return std::string("TYPE_INT");
		}
		break;
		case TYPE_LONG:
		{
			return std::string("TYPE_LONG");
		}
		break;
		case TYPE_FLOAT:
		{
			return std::string("TYPE_FLOAT");
		}
		break;
		case TYPE_DOUBLE:
		{
			return std::string("TYPE_DOUBLE");
		}
		break;
	}
	return std::string("TYPE_UNKNOWN");
}

#endif