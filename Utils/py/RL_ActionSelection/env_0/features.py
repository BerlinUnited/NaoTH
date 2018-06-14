import numpy as np

from scipy.spatial import cKDTree as KDTree


def heinrichs_feature_vec(position, offset=250):
    # STEP 1
    # the following only needs to be created once
    # create the feature positions

    lengthX = 9000
    lengthY = 6000
    # offset = 250

    # the coordinates of the centers of the grid-tiles
    xv = np.arange(-lengthX/2+offset/2, lengthX/2, offset)
    yv = np.arange(-lengthY/2+offset/2, lengthY/2, offset)

    points = np.array([[x,y] for x in xv for y in yv])
    # print points.shape

    # create the search tree
    tree = KDTree(points)


    # STEP 2
    # calculate the feature vector
    # p = [700, -900] # ball position :)
    p = position
    radius = offset # search radius
    errors, neighbors = tree.query(p, k=5, distance_upper_bound = radius)# search 5 closest neighbors
    neighbors = [n for n in neighbors if n < points.shape[0]] # only valid neighbors

    # calculate the feature vector
    f = np.zeros((points.shape[0],))
    f[neighbors] = 1

    # Hinweis: die Liste 'neighbors' entspricht den aktivierten Features (wie im Buch).
    #          Mann koennte also auf die Berechnung des gesamten Vektors f verzichten und die aktuelisierung nur die Elemente von 'neighbors' beschrenken.
    #          Das koennte zu einer signiffikanten Beschleunigung fuehren.

    """
    from matplotlib import pyplot as plt
    
    #STEP 3
    # show some stuff
    
    # visualize the feature vector in the correct coordinates
    fs = np.reshape(f,(xv.shape[0],yv.shape[0])).transpose()
    plt.imshow(fs, extent=[-lengthX/2, lengthX/2,-lengthY/2, lengthY/2])
    
    # show the actual points
    plt.plot(points[:,0],points[:,1],'.')
    plt.plot(p[0],p[1],'o')
    plt.plot(points[neighbors,0],points[neighbors,1],'*')
    
    plt.show()
    """

    return f


def rotation_feature_vec(angle_rad):
    f = np.zeros(8)
    rad = angle_rad % (2*np.pi)

    for i, radian in enumerate(np.linspace(0.25*np.pi,2*np.pi,8)):
        if rad < radian:
            f[i] = 1
            break

    return f


def feature_vec(state, description_output=False):
    angle = state.direction.angle()
    position = (state.position.x, state.position.y)

    position_feature = heinrichs_feature_vec(position)
    position_len = position_feature.shape[0]

    rotation_feature = rotation_feature_vec(angle)
    rotation_len = rotation_feature.shape[0]

    feature_vec = np.append(position_feature, rotation_feature).reshape((1,position_len+rotation_len))

    # TODO: add dictionary which includes all feature creation params
    # currently feature_vec is (872,) MultiBinary numpy array 864 for the field 8 for rotation

    feature_vec = np.transpose(feature_vec)

    if description_output:
        return feature_vec, (position_len, rotation_len)
    else:
        return feature_vec



if __name__ == "__main__":
    from state import State

    print callable(feature_vec)

    print feature_vec(State()).shape