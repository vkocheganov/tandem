from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import matplotlib.pyplot as plt
from itertools import product
import time,os

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# suffix="l3_0.1_thrhld_-1"
# suffix="l3_0.1_thrhld_5"
# suffix="l3_0.2_thrhld_-1"
suffix="l3_0.2_thrhld_5"

workFolder = "/home/victor/Development/tandem_imitate/interpretation/favorite_logs/" + suffix
# axs1 = np.arange(5,31,5)
# axs2 = np.arange(5,31,5)
(T_3_low, T_3_high, T_4_low, T_4_high) = (5,30,5,30)
axs1 = np.arange(T_3_low,T_3_high + 1,1)
axs2 = np.arange(T_4_low,T_4_high + 1,1)

stationarTheoreticalName = workFolder + "/stationaryReachingT"
stationarFactName = workFolder + "/stationaryReachingF"
targetName = workFolder + "/target"
specName = workFolder + "/spec"

points = list(product(axs1,axs2))

timestamp=time.strftime("%Y%m%d-%H%M%S")
picturesFolder= "/home/victor/Development/tandem_imitate/interpretation" + "/Pictures/" + suffix.replace(".","_")

# os.makedirs(picturesFolder)

workArray = np.loadtxt ( specName )
(lambda_1, mu_1, lambda_3, mu_3, mu_4) = workArray[0:5]
thresh=5
def ShowTheoreticalBorders():
    T_4=np.linspace(T_4_low, T_4_high,100)
    T_3_1=lambda_3/(mu_3-lambda_3) * T_4
    idxs=np.logical_and(T_3_1 >= T_3_low, T_3_1 <= T_3_high)
    T_3_1 = T_3_1[idxs]
    T_4_1 = T_4[idxs]
    T_3_2=(mu_4-lambda_1)/lambda_1 * T_4
    idxs=np.logical_and(T_3_2 >= T_3_low, T_3_2 <= T_3_high)
    T_3_2 = T_3_2[idxs]
    T_4_2 = T_4[idxs]

    T_3_3=(mu_4-lambda_1)/lambda_1 * np.maximum(T_4,thresh/lambda_3)
    idxs=np.logical_and(T_3_3 >= T_3_low, T_3_3 <= T_3_high)
    T_3_3 = T_3_3[idxs]
    T_4_3 = T_4[idxs]
    plt.plot(T_4_1, T_3_1, T_4_2, T_3_2, 'r--',T_4_3, T_3_3,'b--')
    # plt.show()


BINS=100
# for filename in (targetName, stationarTheoreticalName, stationarFactName):
for filename in ([targetName]):
    print filename
    workArray = np.loadtxt ( filename )
    myPercentiles = np.percentile(workArray, np.linspace(0, 100, BINS))
    print myPercentiles

    # (X_2d,Y_2d) = np.meshgrid (axs1,axs2)
    # surf = ax.plot_surface(X_2d, Y_2d, workArray)
    # ax.set_zlim3d(550, 600, emit=False)
    # ax.set_xlabel("T_4")
    # ax.set_ylabel("T_3")
    # ax.set_zlabel("Target")
    # # fig.colorbar(surf, shrink=0.5, aspect=5)
    # plt.show()
    
    colors = np.searchsorted(myPercentiles, workArray)
    plt.scatter([x[0] for x in points],[x[1] for x in points], c=colors.ravel(order='F'), cmap='summer')
    plt.xlabel("T_4")
    plt.ylabel("T_3")
    plt.colorbar(boundaries = np.concatenate(([0],myPercentiles)), values = np.arange(0,BINS))
    ShowTheoreticalBorders()
    # plt.show()
    pictureFile = picturesFolder + "/target"
    plt.savefig(pictureFile)
    plt.clf()

BINS=2
for filename in ([stationarTheoreticalName]):
# for filename in ([stationarTheoreticalName, stationarFactName]):
    print filename
    workArray = np.loadtxt ( filename )
    myPercentiles = np.percentile(workArray, np.linspace(0, 100, BINS))
    print myPercentiles
    colors = np.searchsorted(myPercentiles, workArray)
    plt.scatter([x[0] for x in points],[x[1] for x in points], c=colors.ravel(order='F'), cmap='summer')
    plt.colorbar(boundaries = [0, 0.5, 1], values = np.arange(0,BINS))
    ShowTheoreticalBorders()
    # plt.show()
    pictureFile = picturesFolder + "/theoretical_"
    plt.savefig(pictureFile)
    plt.clf()


for filename in ([stationarFactName]):
# for filename in ([stationarTheoreticalName, stationarFactName]):
    print filename
    workArray = np.loadtxt ( filename )
    myPercentiles = np.percentile(workArray, np.linspace(0, 100, BINS))
    print myPercentiles
    colors = np.searchsorted(myPercentiles, workArray)
    plt.scatter([x[0] for x in points],[x[1] for x in points], c=colors.ravel(order='F'), cmap='summer')
    plt.colorbar(boundaries = [0, 0.5, 1], values = np.arange(0,BINS))
    ShowTheoreticalBorders()
    # plt.show()
    pictureFile = picturesFolder + "/fact_"
    plt.savefig(pictureFile)
    plt.clf()

ShowTheoreticalBorders()
