#include "../../../../../src/Math/MathPackage/FloatPointNumber/code.hpp"
#include <sstream>
#include <string>
std::string formatFromOtherTranslationUnit(double value) {
    std::ostringstream stream;
    stream << Float(value);
    return stream.str();
}
