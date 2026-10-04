#include "Circle.hpp"
#include "../TestSupport.hpp"
#include <fstream>
using namespace _geo2;

double boundaryDistance(const std::vector<Point<double>> &ps, Point<double> p) {
    double best = std::numeric_limits<double>::infinity();
    for (int i=0,n=int(ps.size());i<n;++i) {
        auto a=ps[i],b=ps[(i+1)%n];
        double dx=b.x-a.x,dy=b.y-a.y,x=p.x-a.x,y=p.y-a.y;
        double t=dx==0 && dy==0?0:std::clamp((x*dx+y*dy)/(dx*dx+dy*dy),0.,1.);
        best=std::min(best,std::hypot(x-t*dx,y-t*dy));
    }
    return best;
}
template<class T> double number(T x) {
    if constexpr(std::is_arithmetic_v<T>) return double(x);
    else return double(x.val());
}
template<class T> void run(const char *path) {
    std::ifstream in(path);
    CHECK(bool(in));
    int count; in>>count;
    for (int rep=0;rep<count;++rep) {
        int n,k; double area; in>>n>>k>>area;
        std::vector<Point<T>> box(4);
        for(auto &p:box) { double x,y;in>>x>>y;p={T(x),T(y)}; }
        std::vector<Line<T>> lines;
        for(int i=0;i<n;++i) { double x,y,u,v;in>>x>>y>>u>>v;lines.emplace_back(Point<T>{T(x),T(y)},Point<T>{T(u),T(v)}); }
        std::vector<Point<double>> expected(k);
        for(auto &p:expected) in>>p.x>>p.y;
        CHECK(bool(in));
        auto result=halfPlaneIntersection(lines,Convex<T>::fromBoundary(box));
        if(result.empty() != expected.empty()) {
            std::cerr<<"HPI case="<<rep<<" native="<<std::is_arithmetic_v<T><<" expected vertices="<<k<<" actual="<<result.size()<<'\n';
        }
        CHECK(result.empty()==expected.empty());
        double perimeter=0;
        for(int i=0;i<k;++i) perimeter+=std::hypot(expected[i].x-expected[(i+1)%k].x,expected[i].y-expected[(i+1)%k].y);
        // Thin polygons amplify coordinate rounding in relative area error.
        double areaError=1E-7+1E-12*area+2E-5*(perimeter+number(result.perimeter()));
        if (std::abs(number(result.area())-area)>areaError) {
            std::cerr<<"HPI case="<<rep<<" native="<<std::is_arithmetic_v<T><<" area expected="<<area<<" actual="<<number(result.area())<<'\n';
            for (auto p:result.vertices()) std::cerr<<"actual "<<number(p.x)<<' '<<number(p.y)<<'\n';
            for (auto p:expected) std::cerr<<"expected "<<p<<'\n';
        }
        CHECK(std::abs(number(result.area())-area)<=areaError);
        if(result.empty()) continue;
        std::vector<Point<double>> actual;
        for(auto p:result.vertices()) actual.push_back({number(p.x),number(p.y)});
        for(auto p:actual) {
            if(boundaryDistance(expected,p)>2E-5) std::cerr<<"HPI case="<<rep<<" native="<<std::is_arithmetic_v<T><<" extra vertex="<<p<<" distance="<<boundaryDistance(expected,p)<<'\n';
            CHECK(boundaryDistance(expected,p)<=2E-5);
        }
        for(auto p:expected) CHECK(boundaryDistance(actual,p)<=2E-5);
    }
}
int main(int argc,char **argv) {
    CHECK(argc==2);
    run<double>(argv[1]);run<FloatPointNumber<double>>(argv[1]);
    std::cout<<"Geo2 HPI exact Fraction oracle: native/wrapped PASS; cases="<<argv[1]<<'\n';
}
