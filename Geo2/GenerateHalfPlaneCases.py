"""Generate bounded half-plane cases with exact Fraction clipping oracles."""
import argparse
from fractions import Fraction as F
import random


def clip(ps, a, b):
    def side(p):
        return (b[0]-a[0])*(p[1]-a[1])-(b[1]-a[1])*(p[0]-a[0])
    result = []
    for p, q in zip(ps, ps[1:]+ps[:1]):
        x, y = side(p), side(q)
        if x >= 0:
            result.append(p)
        if (x < 0 < y) or (y < 0 < x):
            t = x/(x-y)
            result.append((p[0]+(q[0]-p[0])*t, p[1]+(q[1]-p[1])*t))
    unique = []
    for p in result:
        if not unique or unique[-1] != p:
            unique.append(p)
    if len(unique) > 1 and unique[0] == unique[-1]:
        unique.pop()
    return unique


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('output')
    parser.add_argument('--seed', type=int, default=20261001)
    parser.add_argument('--rounds', type=int, default=1200)
    args = parser.parse_args()
    rng = random.Random(args.seed)
    with open(args.output, 'w') as out:
        out.write(str(args.rounds)+'\n')
        for case in range(args.rounds):
            scale = (1, 1000, 1000000, 100000000)[case % 4]
            tx, ty = rng.randint(-3, 3)*scale, rng.randint(-3, 3)*scale
            box = [(tx-6*scale,ty-6*scale),(tx+6*scale,ty-6*scale),
                   (tx+6*scale,ty+6*scale),(tx-6*scale,ty+6*scale)]
            mode = (case//4) % 6
            rational_point = mode == 0 and (case//24) % 2 == 1
            lines = []
            for _ in range(rng.randint(4, 30)):
                dx, dy = rng.randint(-4, 4), rng.randint(-4, 4)
                if not (dx or dy):
                    dx = 1
                # Common vertices, repeated directions, and general rational corners.
                if mode in (0, 1):
                    a = (tx, ty)
                else:
                    a = (tx+rng.randint(-2,2)*scale,ty+rng.randint(-2,2)*scale)
                b = (a[0]+dx*scale,a[1]+dy*scale)
                if rational_point:
                    qx, qy = F(tx)+F(2*scale,3), F(ty)+F(scale,3)
                    if (b[0]-a[0])*(qy-a[1])-(b[1]-a[1])*(qx-a[0]) < 0:
                        a,b = b,a
                lines.append((a,b))
                if mode == 2:
                    lines.append((a,b))
            if mode == 0:  # Force a common point.
                if rational_point:
                    lines += [((tx,ty-scale),(tx+scale,ty+scale)),
                              ((tx,ty+scale),(tx+scale,ty)),
                              ((tx,ty),(tx-2*scale,ty-scale))]
                else:
                    lines += [((tx,ty),(tx+scale,ty)),((tx+scale,ty),(tx,ty)),
                              ((tx,ty),(tx,ty+scale)),((tx,ty+scale),(tx,ty))]
            elif mode == 1:  # Force a common line, possibly clipped to a point/empty.
                lines += [((tx,ty),(tx+scale,ty)),((tx+scale,ty),(tx,ty))]
            elif mode == 3:  # A contradictory pair.
                lines += [((tx,ty),(tx+scale,ty)),((tx+scale,ty-scale),(tx,ty-scale))]
            elif mode == 4:  # Large, almost parallel directions, determinant -1.
                lines += [((0,0),(1000000000,999999999)),((1,0),(1000000000,999999998))]
            elif mode == 5:  # The same directions meeting at an exact common point.
                lines += [((0,0),(1000000000,999999999)),((0,0),(999999999,999999998))]
            rng.shuffle(lines)
            ps = [(F(x),F(y)) for x,y in box]
            for a,b in lines:
                ps = clip(ps,a,b)
            area = abs(sum(p[0]*q[1]-p[1]*q[0] for p,q in zip(ps,ps[1:]+ps[:1])))/2
            out.write(f'{len(lines)} {len(ps)} {float(area):.17g}\n')
            out.write(' '.join(str(c) for p in box for c in p)+'\n')
            for a,b in lines:
                out.write(f'{a[0]} {a[1]} {b[0]} {b[1]}\n')
            for x,y in ps:
                out.write(f'{float(x):.17g} {float(y):.17g}\n')
    print(f'{args.rounds} exact Fraction cases; seed={args.seed}; output={args.output}')


if __name__ == '__main__':
    main()
