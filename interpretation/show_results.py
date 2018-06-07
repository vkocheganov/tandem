from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import matplotlib.pyplot as plt
from itertools import product
import time,os


#suffix="l3_0.1_thrhld_-1"
# suffix="l3_0.1_thrhld_5"
# suffix="l3_0.2_thrhld_-1"
#suffix="l3_0.2_thrhld_5"

# suffix="l3_0.2_T3_1_50_T4_1_50_thrhld_5"
#suffix="l3_0.2_T3_1_50_T4_1_50_thrhld_-1"
#suffix="l3_0.2_prol_1_30_thrhld_1_30"

# suffix="l3_0.2_T3_1_100_T4_1_100_thrhld_-1"
#suffix="l3_0.2_T3_1_100_T4_1_100_thrhld_0"
# suffix="l3_0.2_T3_1_100_T4_1_100_thrhld_5"

# suffix="l3_0.2_T3_1_100_T4_1_100_thrhld_10"
# suffix="l3_0.2_T3_1_100_T4_1_100_thrhld_15"
# suffix="l3_0.2_T3_1_100_T4_1_100_thrhld_5_prol_10"

# suffix="l3_0.1_T3_1_100_T4_1_100_thrhld_-1"
suffix="l3_0.1_T3_1_100_T4_1_100_thrhld_5"
suffix="l3_0.1_T3_1_100_T4_1_100_thrhld_10"

suffix_arr=["l3_0.2_T3_1_100_T4_1_100_thrhld_-1",
            "l3_0.2_T3_1_100_T4_1_100_thrhld_0",
            "l3_0.2_T3_1_100_T4_1_100_thrhld_5",
            "l3_0.2_T3_1_100_T4_1_100_thrhld_10",
            "l3_0.2_T3_1_100_T4_1_100_thrhld_15",
            "l3_0.2_T3_1_100_T4_1_100_thrhld_5_prol_10",
            "l3_0.1_T3_1_100_T4_1_100_thrhld_-1",
            "l3_0.1_T3_1_100_T4_1_100_thrhld_5",
            "l3_0.1_T3_1_100_T4_1_100_thrhld_10"]

def ShowStationarBorders():
    T_4=np.linspace(T_4_low, T_4_high,100)
    T_3_1=lambda_3/(mu_3-lambda_3) * T_4
    idxs=np.logical_and(T_3_1 >= T_3_low, T_3_1 <= T_3_high)
    T_3_1 = T_3_1[idxs]
    T_4_1 = T_4[idxs]

    T_3_2=(mu_4-lambda_1)/lambda_1 * T_4
    idxs=np.logical_and(T_3_2 >= T_3_low, T_3_2 <= T_3_high)
    T_3_2 = T_3_2[idxs]
    T_4_2 = T_4[idxs]

    plt.plot(T_4_1, T_3_1, color='black', label='Stationar bounds', linewidth=5)
    plt.plot(T_4_2, T_3_2, color='black', linewidth=5)
    plt.legend()


def ShowStationarProlongBorders():
    T_4=np.linspace(T_4_low, T_4_high,100)

    # T_3_3=(mu_4-lambda_1)/lambda_1 * np.maximum(T_4,thresh/lambda_3)
    T_3_3=np.maximum((mu_4-lambda_1)/lambda_1 * T_4,(thresh/lambda_3 * (mu_4-lambda_1) + prol*(mu_4-lambda_1))/lambda_1)
    idxs=np.logical_and(T_3_3 >= T_3_low, T_3_3 <= T_3_high)
    T_3_3 = T_3_3[idxs]
    T_4_3 = T_4[idxs]

    plt.plot(T_4_3, T_3_3, color='gray', label='Higher stationar bound (prol case)')
    plt.legend()


def ShowLoads():
    T_4=np.linspace(T_4_low, T_4_high,100)

    T_3_4=(mu_4 * lambda_3)/(lambda_1 * mu_3 ) * T_4
    idxs=np.logical_and(T_3_4 >= T_3_low, T_3_4 <= T_3_high)
    T_3_4 = T_3_4[idxs]
    T_4_4 = T_4[idxs]

    plt.plot(T_4_4, T_3_4, color='blue', label='Equal loads line')
    plt.legend()

def CalcBestEqualCycle():
    eqArr=np.zeros(axs1[-1]+axs2[-1]+1)
    eqIdx=np.zeros((axs1[-1]+axs2[-1]+1,2))
    for i in range (workArray.shape[0]):
        for j in range (workArray.shape[1]):
            T_total=(axs1[i]+axs2[j])
            if (eqArr[T_total] == 0 or eqArr[T_total] > workArray[i][j]):
                eqArr[T_total] = workArray[i][j]
                eqIdx[T_total][:]=(axs1[i],axs2[j])

    return (eqArr, eqIdx)

def ShowBestEqualCycle(eqArr, eqIdx):
    highest_cycle=T_4_high * (1 + lambda_3/(mu_3-lambda_3))
    print highest_cycle
    idxs_to_show=((np.arange(eqArr.shape[0])[np.logical_and(eqArr>0,(eqIdx[:,0]+eqIdx[:,1])<highest_cycle)])[::4])
    plt.plot([x[1] for x in eqIdx[idxs_to_show]], [x[0] for x in eqIdx[idxs_to_show]], color='darkviolet', label='Best target. Fixed cycle-length')
    plt.legend()

for suffix in suffix_arr:
    workFolder = "/home/victor/Development/tandem_imitate/interpretation/favorite_logs/" + suffix
    # axs1 = np.arange(5,31,5)
    # axs2 = np.arange(5,31,5)
    # (T_3_low, T_3_high, T_4_low, T_4_high) = (1,30,0,30)
    #(T_3_low, T_3_high, T_4_low, T_4_high) = (5,30,5,30)
    # (T_3_low, T_3_high, T_4_low, T_4_high) = (1,50,1,50)
    (T_3_low, T_3_high, T_3_step, T_4_low, T_4_high, T_4_step) = (1,100,2,1,100,2)

    axs1 = np.arange(T_3_low,T_3_high + 1,T_3_step)
    axs2 = np.arange(T_4_low,T_4_high + 1,T_4_step)

    stationarTheoreticalName = workFolder + "/stationaryReachingT"
    stationarFactName = workFolder + "/stationaryReachingF"
    targetName = workFolder + "/target"
    specName = workFolder + "/spec"

    points = list(product(axs1,axs2))

    timestamp=time.strftime("%Y%m%d-%H%M%S")
    picturesFolder= "/home/victor/Development/tandem_imitate/interpretation" + "/Pictures/" + suffix.replace(".","_")

    if not os.path.exists(picturesFolder):
        os.makedirs(picturesFolder)

    workArray = np.loadtxt ( specName )
    (lambda_1, mu_1, lambda_3, mu_3, mu_4, thresh, prol) = workArray[0:7]
    # thresh=5


    BINS=100
    # for filename in (targetName, stationarTheoreticalName, stationarFactName):
    for filename in ([targetName]):
        print filename
        workArray = np.loadtxt ( filename )
        myPercentiles = np.percentile(workArray, np.linspace(0, 100, BINS))
        print myPercentiles

        (myEqArr, myEqIdx)=CalcBestEqualCycle()
        ShowBestEqualCycle(myEqArr, myEqIdx)

        colors = np.searchsorted(myPercentiles, workArray)
        plt.scatter([x[0] for x in points],[x[1] for x in points], c=colors.ravel(order='F'), cmap='summer')
        plt.xlabel("T_4")
        plt.ylabel("T_3")
        # plt.ylabel("Duration")
        # plt.xlabel("Threshold")
        plt.colorbar(boundaries = np.concatenate(([0],myPercentiles)), values = np.arange(0,BINS))
        ShowStationarBorders()
        ShowStationarProlongBorders()
        ShowLoads()
        # plt.show()

        pictureFile = picturesFolder + "/target"
        plt.savefig(pictureFile)
        plt.clf()

        # (zlow,zhigh)=(550, 600)
        # fig = plt.figure()
        # ax = fig.add_subplot(111, projection='3d')
        # (X_2d,Y_2d) = np.meshgrid (axs1,axs2)
        # zToDisplay=np.minimum(workArray,zhigh)
        # surf = ax.plot_surface(X_2d, Y_2d, zToDisplay)
        # ax.set_zlim3d(550, 600, emit=False)
        # ax.set_xlabel("T_4")
        # ax.set_ylabel("T_3")
        # ax.set_zlabel("Target")
        # plt.show()


    BINS=2
    for filename in ([stationarFactName]):
    # for filename in ([stationarTheoreticalName, stationarFactName]):
        print filename
        workArray = np.loadtxt ( filename )
        myPercentiles = np.percentile(workArray, np.linspace(0, 100, BINS))
        print myPercentiles
        colors = np.searchsorted(myPercentiles, workArray)
        plt.scatter([x[0] for x in points],[x[1] for x in points], c=colors.ravel(order='F'), cmap='summer')
        plt.xlabel("T_4")
        plt.ylabel("T_3")
        # plt.ylabel("Duration")
        # plt.xlabel("Threshold")
        plt.colorbar(boundaries = [0, 0.5, 1], values = np.arange(0,BINS))
        ShowStationarBorders()
        ShowStationarProlongBorders()
        # plt.show()
        pictureFile = picturesFolder + "/fact_"
        plt.savefig(pictureFile)
        plt.clf()




    # BINS=2
    # for filename in ([stationarTheoreticalName]):
    # # for filename in ([stationarTheoreticalName, stationarFactName]):
    #     print filename
    #     workArray = np.loadtxt ( filename )
    #     myPercentiles = np.percentile(workArray, np.linspace(0, 100, BINS))
    #     print myPercentiles
    #     colors = np.searchsorted(myPercentiles, workArray)
    #     plt.scatter([x[0] for x in points],[x[1] for x in points], c=colors.ravel(order='F'), cmap='summer')
    #     plt.xlabel("T_4")
    #     plt.ylabel("T_3")
    #     # plt.ylabel("Duration")
    #     # plt.xlabel("Threshold")
    #     plt.colorbar(boundaries = [0, 0.5, 1], values = np.arange(0,BINS))
    #     ShowTheoreticalBorders()
    #     # plt.show()
    #     pictureFile = picturesFolder + "/theoretical_"
    #     plt.savefig(pictureFile)

    #     plt.show()
    #     # plt.clf()
