

#include "Utils.h"

using namespace coral;

namespace Util{
Numeric::Type polyTypeToNumeric(coral::PolyValue::ValueType type){
	switch(type){
	case coral::PolyValue::numericTypeAny:
		return coral::Numeric::numericTypeAny;
	case coral::PolyValue::numericTypeInt:
		return coral::Numeric::numericTypeInt;
	case coral::PolyValue::numericTypeIntArray:
		return coral::Numeric::numericTypeIntArray;
	case coral::PolyValue::numericTypeFloat:
		return coral::Numeric::numericTypeFloat;
	case coral::PolyValue::numericTypeFloatArray:
		return coral::Numeric::numericTypeFloatArray;
	case coral::PolyValue::numericTypeVec3:
		return coral::Numeric::numericTypeVec3;
	case coral::PolyValue::numericTypeVec3Array:
		return coral::Numeric::numericTypeVec3Array;
	case coral::PolyValue::numericTypeCol4:
		return coral::Numeric::numericTypeCol4;
	case coral::PolyValue::numericTypeCol4Array:
		return coral::Numeric::numericTypeCol4Array;
	case coral::PolyValue::numericTypeMatrix44:
		return coral::Numeric::numericTypeMatrix44;
	case coral::PolyValue::numericTypeMatrix44Array:
		return coral::Numeric::numericTypeMatrix44Array;
	case coral::PolyValue::numericTypeQuat:
		return coral::Numeric::numericTypeQuat;
	case coral::PolyValue::numericTypeQuatArray:
		return coral::Numeric::numericTypeQuatArray;
	}
}

coral::String::Type polyTypeToString(coral::PolyValue::ValueType type){
	switch(type){
	case coral::PolyValue::stringTypeAny:
		return coral::String::stringTypeAny;
	case coral::PolyValue::stringType:
		return coral::String::stringType;
	case coral::PolyValue::stringTypeArray:
		return coral::String::stringTypeArray;
	case coral::PolyValue::pathType:
		return coral::String::pathType;
	case coral::PolyValue::pathTypeArray:
		return coral::String::pathTypeArray;
	}
}

coral::Bool::Type polyTypeToBool(coral::PolyValue::ValueType type){
	switch(type){
	case coral::PolyValue::boolTypeAny:
		return coral::Bool::boolTypeAny;
	case coral::PolyValue::boolType:
		return coral::Bool::boolType;
	case coral::PolyValue::boolTypeArray:
		return coral::Bool::boolTypeArray;
	}
}

coral::PolyValue::ValueType numericTypeToPoly(coral::Numeric::Type type){
	switch(type){
	case coral::Numeric::numericTypeAny:
		return coral::PolyValue::numericTypeAny;
	case coral::Numeric::numericTypeInt:
		return coral::PolyValue::numericTypeInt;
	case coral::Numeric::numericTypeIntArray:
		return coral::PolyValue::numericTypeIntArray;
	case coral::Numeric::numericTypeFloat:
		return coral::PolyValue::numericTypeFloat;
	case coral::Numeric::numericTypeFloatArray:
		return coral::PolyValue::numericTypeFloatArray;
	case coral::Numeric::numericTypeVec3:
		return coral::PolyValue::numericTypeVec3;
	case coral::Numeric::numericTypeVec3Array:
		return coral::PolyValue::numericTypeVec3Array;
	case coral::Numeric::numericTypeCol4:
		return coral::PolyValue::numericTypeCol4;
	case coral::Numeric::numericTypeCol4Array:
		return coral::PolyValue::numericTypeCol4Array;
	case coral::Numeric::numericTypeMatrix44:
		return coral::PolyValue::numericTypeMatrix44;
	case coral::Numeric::numericTypeMatrix44Array:
		return coral::PolyValue::numericTypeMatrix44Array;
	case coral::Numeric::numericTypeQuat:
		return coral::PolyValue::numericTypeQuat;
	case coral::Numeric::numericTypeQuatArray:
		return coral::PolyValue::numericTypeQuatArray;
	}
}

coral::PolyValue::ValueType stringTypeToPoly(coral::String::Type type){
	switch (type){
	case coral::String::stringTypeAny:
		return coral::PolyValue::stringTypeAny;
	case coral::String::stringType:
		return coral::PolyValue::stringType;
	case coral::String::stringTypeArray:
		return coral::PolyValue::stringTypeArray;
	case coral::String::pathType:
		return coral::PolyValue::pathType;
	case coral::String::pathTypeArray:
		return coral::PolyValue::pathTypeArray;
	}
}

coral::PolyValue::ValueType boolTypeToPoly(coral::Bool::Type type){
	switch(type){
	case coral::Bool::boolTypeAny:
		return coral::PolyValue::boolTypeAny;
	case coral::Bool::boolType:
		return coral::PolyValue::boolType;
	case coral::Bool::boolTypeArray:
		return coral::PolyValue::boolTypeArray;
	}
}

}
