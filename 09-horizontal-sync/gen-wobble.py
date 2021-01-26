#!/usr/bin/env python3
import sys
import math

if __name__ == '__main__':
    left = int(sys.argv[1]);
    right = int(sys.argv[2]);
    radius = (right-left)//2;
    size = int(sys.argv[3]);
    precalc = [left + radius + int(radius * math.sin(i*2.0*math.pi/size))
               for i in range(size)];
    print(", ".join([str(i) for i in precalc]))
