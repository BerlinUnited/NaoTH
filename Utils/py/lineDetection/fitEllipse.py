
import numpy as np
import numpy.linalg as la
import math
import random

# Ellipse: Ax^2+Bxy+Cy^2+Dx+Ey+F=0

def fitEllipse(x,y):
    x = x[:,np.newaxis]
    y = y[:,np.newaxis]

    # design matrix
    D = np.hstack((x*x, x*y, y*y, x, y, np.ones_like(x)))
    print(D)
    # scatter matrix
    S = np.dot(D.T,D)

    # constraint matrix
    C = np.zeros([6,6])
    C[0,2] = C[2,0] = 2; C[1,1] = -1

    # solve eigensystem
    E, V =  la.eig(np.dot(la.inv(S), C))

    # find positive eigenvalue
    n = np.argmax(E)
    #n = np.argmax(np.abs(E))

    # corresponding eigenvector
    a = V[:,n]
    return a

def improved_fitEllipse(x,y):
    c1 = np.mean(x)
    c2 = np.mean(y)

    # quadratic part of the design matrix
    D1 = np.stack((
        np.power(x - c1, 2),
        (x - c1)*(y - c2),
        np.power(y - c2, 2)
        ), axis=-1)

    # linear part of the design matrix
    D2 = np.stack((
        x-c1,
        y-c2,
        np.ones_like(x)
        ), axis=-1)

    # quadratic part of the scatter matrix
    S1 = np.dot(D1.T, D1)
    # combined part of the scatter matrix
    S2 = np.dot(D1.T, D2)
    # linear part of the scatter matrix
    S3 = np.dot(D2.T, D2)

    # for getting A1 from A0
    T = np.dot(-la.inv(S3), S2.T)

    # reduced scatter matrix
    M = S1 + np.dot(S2,T)

    M = np.stack((
        M[2]/2,
        -M[1],
        M[0]/2
        ))

    # solve eigensystem
    E, V =  la.eig(M)

    # evaluate a'Ca
    cond = 4*V[0] * V[2] - np.power(V[1],2)
    A0 = V[:, np.argmax(cond)]

    A = np.concatenate((
        A0,
        np.dot(T, A0)
        ))

    A3 = A[3] - 2*A[0]*c1 - A[1]*c2
    A4 = A[4] - 2*A[2]*c2 - A[1]*c1
    A5 = A[5] + A[0]*c1*c1 + A[2]*c2*c2 + A[1]*c1*c2 - A[3]*c1 - A[4]*c2;

    A[3] = A3
    A[4] = A4
    A[5] = A5


    R = max(ellipse_axis_length(A))
    center = ellipse_center(A)

    r = center[0]*center[0]*A[0] + center[0]*center[1]*A[1] + center[1]*center[1]*A[2]

    A = A * R/np.sqrt(r)

    #A = A / A[0]
    #A = A/la.norm(A)

    return A

def ellipse_center(a):
    A = a[0]
    B = np.divide(a[1], 2)
    C = a[2]
    D = np.divide(a[3], 2)
    F = np.divide(a[4], 2)

    num = np.power(B,2) - A*C
    x0 = np.divide((C*D-B*F), num)
    y0 = np.divide((A*F-B*D), num)
    return np.array([x0,y0])


def ellipse_axis_length(a):
    A = a[0]
    B = np.divide(a[1], 2)
    C = a[2]
    D = np.divide(a[3], 2)
    F = np.divide(a[4], 2)
    G = a[5]

    num = 2*( A*np.power(F,2) + C*np.power(D,2) + G*np.power(B,2) - 2*B*D*F - A*C*G)

    sd = np.sqrt( np.power(A-C,2) + 4*np.power(B,2) )

    den1 = (np.power(B,2) - A*C) * ( sd - (A+C) )
    den2 = (np.power(B,2) - A*C) * ( -sd - (A+C) )

    return np.array( [np.sqrt(num/den1), np.sqrt(num/den2)] )

def mahalanobis_radius(a, center):
    #A = np.array([[a[0],a[1]/2],[a[1]/2, a[2]]])
    #r = np.dot(np.dot(center, A), center)
    r = center[0]*center[0]*a[0] + center[0]*center[1]*a[1] + center[1]*center[1]*a[2]
    return r - a[5]

def mahalanobis_distance(a, x, y, center):
    #A = np.array([[a[0],a[1]/2],[a[1]/2, a[2]]])
    p = np.array([x, y]) - center
    #d = np.dot(np.dot((p - center), A), (p - center))
    d = p[0]*p[0]*a[0] + p[0]*p[1]*a[1] + p[1]*p[1]*a[2]
    return d

def error_to(a, x, y, center):
    r = mahalanobis_radius(a, center)
    #print("RAD:", r)

    d = mahalanobis_distance(a, x, y, center)
    #print("DIS:", d)

    e = abs(d - r)
    #print("ERR:", e)

    return e, np.sqrt(e)

def ellipse_angle_of_rotation(a):
    A = a[0]
    B = a[1]#np.divide(a[1], 2)
    C = a[2]

    if B==0:
        if A < C:
            print(1)
            return 0
        else:
            print(2)
            return np.pi/2
    else:
        if A < C:
            print(3)
            return 0.5 * np.arctan( B/(A-C) )
        else:
            print(4)
            return np.pi/2 + 0.5 * np.arctan( B/(A-C) )
    #0.5 * np.arctan( (A-C) / (2*B) )


if __name__ == '__main__':
    
    #x = np.array([-20.1 ,2.5, 3, 4,  -5, 2.2]) * 100
    #y = np.array([1 ,2  ,-1, 2, 0.5, 0.2]) * 100
    #x = [3399.73, 3070.88, 2763.85, 2503.19, 2296.22]
    #y = [-621.92, -674.54, -712.061, -743.485, -775.087]
    
    x = [3399.73, 3070.88, 2763.85, 2503.19, 2296.22, 2116.13, 1956.95, 1877.82, 1868.99, 1867.03, 1875.7, 1884.6]
    y = [-621.92, -674.54, -712.061, 743.485, -775.087, -802.725, -827.03, -878.736, -960.967, -1049.74, -1148.71, -1252.38]
    
    #x = np.array(x) - 2698, 
    #y = np.array(y) + 946
    #x = np.array([2.5, 3, 4,  5])
    #y = np.array([2  ,-1, 2, 0.5])

    #a = fitEllipse(x,y)
    a = improved_fitEllipse(x,y)
    #a = -1*a
    #a = [-0.00819266, 0.0416844, -0.198772, -0.163809, 0.0892647, 0.961189]
    #a = [-0.0403062, 0.205078, -0.977915, -0.805908, 0.439164, 4.72885]
    #a = np.array(a)/a[0]
    print("PARAMS:", a)
    print(la.norm(a))
    center = ellipse_center(a)
    phi = ellipse_angle_of_rotation(a)
    axes = ellipse_axis_length(a)

    #xt = np.array([-1, 0, 1, 2, 3, 4, 5 ,6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17])
    #yt = np.array([0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])

    xt = np.array([random.uniform(min(x), 2* max(x)) for i in range(1000)])
    yt = np.array([random.uniform(min(y), 2* max(y)) for i in range(1000)])

    #x= np.append(x, [0])
    #y= np.append(y, [0])

    xr = []
    yr = []
    xb = []
    yb = []
    xc = []
    yc = []

    print("ABS")
    for c in range(len(xt)):
        e = error_to(a, xt[c], yt[c], center)
        #print(e)
        e = e[1]
        if e < 250:
            xb.append(xt[c])
            yb.append(yt[c])
        elif mahalanobis_distance(a, xt[c], yt[c], center) < mahalanobis_radius(a, center):
            xc.append(xt[c])
            yc.append(yt[c])
        else:
            xr.append(xt[c])
            yr.append(yt[c])
    print("ABS_END")

    print("center = ",  center)
    print("angle of rotation = ",  phi)
    print("axes = ", axes)

    # sample ellipse
    t = np.arange(0, 2*np.pi, 0.01)
    xx = axes[0]*np.cos(t)*np.cos(phi) - axes[1]*np.sin(t)*np.sin(phi) + center[0]
    yy = axes[0]*np.cos(t)*np.sin(phi) + axes[1]*np.sin(t)*np.cos(phi) + center[1]


    from matplotlib import pyplot as plt

    plt.scatter(x,y)
    plt.scatter(xr,yr)
    plt.scatter(xb,yb)
    plt.scatter(xc,yc)
    plt.scatter(center[0], center[1], marker='x', color = 'red')

    plt.plot(xx,yy, color = 'red')
    plt.show()
