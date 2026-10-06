#include "../../../../../src/Math/MathPackage/FloatPointNumber/code.hpp"
#include "../../../../Support/CaseSupport.hpp"
#include <sstream>

int main() {
    runCase("FloatPointNumber/01-epsilon-low", [] {
        CHECK(Float(0) == Float(5e-13));
        CHECK(!(Float(0) == Float(2e-12)));
    });
    runCase("FloatPointNumber/02-signed-zero", [] {
        CHECK(std::signbit((-Float(0)).val()));
        CHECK(Float(-0.0).val() == 0);
    });
    runCase("FloatPointNumber/03-negative-tie", [] {
        CHECK(Float(-2.5).round<int>() == -2);
    });
    runCase("FloatPointNumber/04-positive-tie", [] {
        CHECK(Float(2.5).round<int>() == 3);
    });
    runCase("FloatPointNumber/05-epsilon-round", [] {
        CHECK(Float(0.5 - 5e-13).round<int>() == 1);
        CHECK(Float(0.5 - 2e-12).round<int>() == 0);
    });
    runCase("FloatPointNumber/06-large-integral", [] {
        CHECK(Float(4503599627370497.0).round<long long>() == 4503599627370497LL);
    });
    runCase("FloatPointNumber/07-comparison-order", [] {
        std::vector<Float> a{Float(1e-13), Float(-1e-13), Float(0)};
        std::sort(a.begin(), a.end());
        CHECK(a[0].val() == -1e-13 and a[2].val() == 1e-13);
    });
    runCase("FloatPointNumber/08-std-sqrt", [] {
        CHECK(std::sqrt(Float(81)).val() == 9);
    });
    runCase("FloatPointNumber/09-std-hypot-angle", [] {
        CHECK(std::atan2(Float(0), Float(1)).val() == 0);
        CHECK(std::cos(Float(0)).val() == 1);
    });
    runCase("FloatPointNumber/10-precision-stream", [] {
        Float::setprecision(3);
        std::ostringstream out;
        out << Float(1.25);
        CHECK(out.str() == "1.250");
        Float::setprecision(10);
    });
    return finishCases(10);
}
