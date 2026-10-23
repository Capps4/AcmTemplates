#include "../../../../../src/extra/Geometry/ClosestPair/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("ClosestPair/oracle", [] {
CHECK((closestPair<int,long long>({})==std::numeric_limits<long long>::max()));
        for (int it=0;it<40;++it) {
            std::vector<Point<long long>> a(randomInt(1,40));
            for (auto& p:a) { p.x=randomInt(-100,100); p.y=randomInt(-100,100); }
            long long want=std::numeric_limits<long long>::max();
            for (int i=0;i<int(a.size());++i) { for(int j=0;j<i;++j) { auto x=a[i].x-a[j].x,y=a[i].y-a[j].y; want=std::min(want,x*x+y*y); } }
            CHECK((closestPair<long long,long long>(a)==want));
        }
        CHECK((closestPair<int,long long>({{1,1},{1,1}})==0));
    });
    return 0;
}
