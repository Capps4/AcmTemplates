#include "Circle.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <set>
using namespace std;
using ll = long long;
#define sz(x) int((x).size())
#define rep(i,a,b) for(int i=(a);i<(b);++i)
#define all(x) begin(x),end(x)
namespace reference {
#include "References/LineHullIntersection.h"
#undef cmp
#undef cmpL
#undef extr
#include "References/SegmentIntersection.h"
#include "References/HullDiameter.h"
// Same diameter kernel, with only the by-value input parameter changed.
array<P,2> hullDiameterView(const vector<P> &s) {
    int n=sz(s),j=n<2?0:1;
    pair<ll,array<P,2>> result({0,{s[0],s[0]}});
    rep(i,0,j) for(;;j=(j+1)%n) {
        result=max(result,{(s[i]-s[j]).dist2(),{s[i],s[j]}});
        if((s[(j+1)%n]-s[j]).cross(s[i+1]-s[i])>=0) break;
    }
    return result.second;
}
}
#undef rep
#undef sz
#undef all
using RP=reference::Point<double>;
volatile double checksum=0;

template<class F> double measure(int repeats,F f) {
    array<double,7> samples;
    for(auto &sample:samples) {
        auto start=chrono::steady_clock::now();
        double total=0;
        for(int i=0;i<repeats;++i) total+=f(i);
        checksum=total;
        sample=chrono::duration<double,milli>(chrono::steady_clock::now()-start).count();
    }
    sort(samples.begin(),samples.end());return samples[3];
}
template<class A,class B> void compare(const char *operation,int n,int repeats,A a,B b,const char *label="KACTL") {
    for(int i=0;i<min(repeats,1000);++i) {
        double x=a(i),y=b(i);
        if(abs(x-y)>1E-6*(1+abs(x)+abs(y))) { cerr<<operation<<" checksum mismatch "<<x<<' '<<y<<'\n';exit(1); }
    }
    // Alternate ordering between operations/sizes; generation is outside timing.
    double ours,other;
    if((n+int(operation[0]))%2) ours=measure(repeats,a),other=measure(repeats,b);
    else other=measure(repeats,b),ours=measure(repeats,a);
    cout<<"Geo2,"<<operation<<','<<n<<','<<repeats<<','<<ours<<'\n';
    cout<<label<<','<<operation<<','<<n<<','<<repeats<<','<<other<<'\n';
}
int main() {
    cout<<"implementation,operation,n,repeats,median_ms\n"<<fixed<<setprecision(6);
    vector<RP> directions;
    for(int i=0;i<40000;++i) { double a=i*2.399963229728653;directions.emplace_back(cos(a),sin(a)); }
    for(int n:{1024,8192,65536}) {
        vector<_geo2::Point<double>> ps;
        for(int i=0;i<n;++i) { double a=2*acos(-1.)*i/n;ps.emplace_back(100*cos(a),100*sin(a)); }
        auto h=_geo2::Convex<double>::fromBoundary(ps);
        vector<RP> poly;for(auto p:h.vertices())poly.emplace_back(p.x,p.y);
        auto line=[&](int i) { auto v=directions[i];double offset=i%3==0?200:i%3==1?50:0;return pair<RP,RP>{v.perp()*offset,v}; };
        auto ours=[&](int i) { auto [p,v]=line(i);return double(_geo2::inter(h,_geo2::Line<double>::fromVec({p.x,p.y},{v.x,v.y}),nullptr)); };
        compare("line_predicate_public",n,40000,ours,[&](int i) { auto [p,v]=line(i);auto q=reference::lineHull(p,p+v,poly);return double(q[0]!=-1); });
        compare("line_predicate_kernel",n,40000,ours,[&](int i) {
            auto [p,v]=line(i);
            int a=reference::extrVertex(poly,v.perp()),b=reference::extrVertex(poly,v.perp()*-1);
            return double(v.cross(poly[a]-p)>=0 && v.cross(poly[b]-p)<=0);
        },"KACTL-adapted");
        compare("near_line_disjoint",n,40000,[&](int i) {
            auto v=directions[i];auto p=v.perp()*200;
            auto q=_geo2::near(h,_geo2::Line<double>::fromVec({p.x,p.y},{v.x,v.y}));return q.first.dist2(q.second);
        },[&](int i) {
            auto v=directions[i],p=v.perp()*200;
            int a=reference::extrVertex(poly,v.perp()),b=reference::extrVertex(poly,v.perp()*-1);
            auto q=abs(v.cross(poly[a]-p))<abs(v.cross(poly[b]-p))?poly[a]:poly[b];
            auto projected=p+v*(v.dot(q-p)/v.dist2());return (q-projected).dist2();
        },"KACTL-adapted");
    }
    for(int n:{1025,8193,60001}) {
        vector<_geo2::Point<ll>> ps;
        for(ll i=-n/2;i<=n/2;++i) ps.emplace_back(i,i*i);
        auto h=_geo2::convexHull(ps);
        vector<reference::P> poly;for(auto p:h.vertices())poly.emplace_back(p.x,p.y);
        auto ours=[&](int) { auto p=_geo2::farthestPair(h);return double(h[(*p)[0]].dist2(h[(*p)[1]])); };
        compare("diameter_public",int(poly.size()),30,ours,[&](int) { auto p=reference::hullDiameter(poly);return double((p[0]-p[1]).dist2()); });
        compare("diameter_kernel",int(poly.size()),30,ours,[&](int) { auto p=reference::hullDiameterView(poly);return double((p[0]-p[1]).dist2()); },"KACTL-adapted");
    }
    array<array<RP,4>,40000> cases;
    mt19937 rng(20261002);
    uniform_int_distribution<int> pick(-100,100);
    for(int i=0;i<int(cases.size());++i) {
        double x=pick(rng),y=pick(rng);
        switch(i%4) {
        case 0: cases[i]={RP{x-2,y-2},RP{x+2,y+2},RP{x-2,y+2},RP{x+2,y-2}};break;
        case 1: cases[i]={RP{x,y},RP{x+1,y},RP{x+2,y},RP{x+3,y}};break;
        case 2: cases[i]={RP{x,y},RP{x+1,y},RP{x+1,y},RP{x+1,y+1}};break;
        default:cases[i]={RP{x,y},RP{x+3,y},RP{x+1,y},RP{x+4,y}};break;
        }
    }
    compare("segment_intersection",int(cases.size()),int(cases.size()),[&](int i) {
        auto p=cases[i];return double(_geo2::inter(_geo2::Seg<double>{{p[0].x,p[0].y},{p[1].x,p[1].y}},_geo2::Seg<double>{{p[2].x,p[2].y},{p[3].x,p[3].y}}).size());
    },[&](int i) { auto p=cases[i];return double(reference::segInter(p[0],p[1],p[2],p[3]).size()); });
    cerr<<"checksum="<<checksum<<'\n';
}
