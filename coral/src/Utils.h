/*! holds miscellaneous utilities
 */

#ifndef CORAL_UTILS_H
#define CORAL_UTILS_H

#include "PolyValue.h"
#include "Numeric.h"
#include "BoolAttribute.h"
#include "StringAttribute.h"
#include "coralDefinitions.h"

namespace Util{
// use to convert from one value type enum to another
coral::Numeric::Type polyTypeToNumeric(coral::PolyValue::ValueType type);
coral::String::Type polyTypeToString(coral::PolyValue::ValueType type);
coral::Bool::Type polyTypeToBool(coral::PolyValue::ValueType type);
coral::PolyValue::ValueType numericTypeToPoly(coral::Numeric::Type type);
coral::PolyValue::ValueType stringTypeToPoly(coral::String::Type type);
coral::PolyValue::ValueType boolTypeToPoly(coral::Bool::Type type);
}

#endif
