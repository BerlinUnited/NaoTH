import math
import sys
from math3d import * 

if __name__ == "__main__":
  print "puh"
  
  v = Vector3(1,2,3)
  print v*3
  w = Vector3(3,2,1)
  u = w*v
  print u
  m = Matrix3x3()
  m.c1.x=1
  m.c2.y=2
  m.c3.z=3
  m.c3.x=1
  print m,"\n"
  print m.transpose()
  
  z = m*v
  print z
  
  p = Pose3D()
  p*v
  (~p)*v