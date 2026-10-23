#include "../../../../../src/extra/Geometry/PolarTwoPointer/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("PolarTwoPointer/oracle", [] {
std::vector<Vec<long long>> empty;
        CHECK(twoPointerWithPolarSorting(empty).empty());
        for (int it=0;it<30;++it) {
            std::vector<Vec<long long>> a;
            for (int i=0;i<30;++i) { int x=0,y=0; while(x==0 and y==0) { x=randomInt(-3,3); y=randomInt(-3,3); } a.emplace_back(x,y); }
            auto got=twoPointerWithPolarSorting(a);
            const long double pi=std::acos(-1.L);
            for (int i=0;i<int(a.size());++i) {
                int k=1;
                for (;k<int(a.size());++k) {
                    const auto& v=a[(i+k)%a.size()];
                    long double ang=std::atan2(static_cast<long double>(v.y),static_cast<long double>(v.x))-std::atan2(static_cast<long double>(a[i].y),static_cast<long double>(a[i].x));
                    if (ang < -1e-15L) { ang+=2*pi; }
                    if (ang >= pi-1e-15L) { break; }
                }
                CHECK(got[i]==(i+k)%int(a.size()));
            }
        }
    });
    return 0;
}
