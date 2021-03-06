from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import matplotlib.pyplot as plt
from itertools import product
import time,os



# suffix_arr=["new/l3_0.1_T3_1_100_T4_1_100_thrhld_10_prolong_10__1",
#             "new/l3_0.1_T3_1_100_T4_1_100_thrhld_10_prolong_10__2",
#             "new/l3_0.1_T3_1_100_T4_1_100_thrhld_10_prolong_10__3",
#             "new/l3_0.1_T3_1_100_T4_1_100_thrhld_10_prolong_10__4",
#             "new/l3_0.1_T3_1_100_T4_1_100_thrhld_10_prolong_10__5",
#             "new/l3_0.1_T3_1_100_T4_1_100_thrhld_10_prolong_10__6",
#             "new/l3_0.1_T3_1_100_T4_1_100_thrhld_10_prolong_10__7",
#             "new/l3_0.1_T3_1_100_T4_1_100_thrhld_10_prolong_10__8",
#             "new/l3_0.1_T3_1_100_T4_1_100_thrhld_10_prolong_10__9"]
suffix_arr=["new/l3_0.1_T3_1_100_T4_1_100_thrhld_10_prolong_10__1"]

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

    plt.plot(T_4_4, T_3_4, color='blue', label='Equal loads line (cycle case)')
    plt.legend()
    

def ShowSampleLoads():
    loadsHigh = np.loadtxt(loadsHighName)
    loadsLow = np.loadtxt(loadsLowName)
    loadsDiff = np.absolute(loadsHigh - loadsLow)

    (X_2d,Y_2d) = np.meshgrid (axs1,axs2)
    print loadsDiff
    idxOfInterest = loadsDiff < 0.05
    X_2d=X_2d[idxOfInterest]
    Y_2d=Y_2d[idxOfInterest]
    plt.plot(X_2d, Y_2d, color='lightblue', label='Equal loads line (prolong case)', marker='o')
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


def CalcBestEqualCycleShifted():
    beta=(mu_4 * lambda_3)/(lambda_1 * mu_3 )
    ArrNum = int(beta*axs1[-1]+axs2[-1]+1)
    eqArr=np.zeros(ArrNum)
    eqIdx=np.zeros((ArrNum,2))
    for i in range (workArray.shape[0]):
        for j in range (workArray.shape[1]):
            T_total=int(beta*axs1[i]+axs2[j])
            if (eqArr[T_total] == 0 or eqArr[T_total] > workArray[i][j]):
                eqArr[T_total] = workArray[i][j]
                eqIdx[T_total][:]=(axs1[i],int(axs2[j]))

    return (eqArr, eqIdx)

def CalcBestEqualCycleVertical():
    ArrNum = int(workArray.shape[1])
    eqArr=np.zeros(ArrNum)
    eqIdx=np.zeros((ArrNum,2))
    for i in range (workArray.shape[0]):
        for j in range (workArray.shape[1]):
            T_total=j
            if (eqArr[T_total] == 0 or eqArr[T_total] > workArray[i][j]):
                eqArr[T_total] = workArray[i][j]
                eqIdx[T_total][:]=(axs1[i],int(axs2[j]))

    return (eqArr, eqIdx)

def CalcAlmostBestEqualCycle(eqArr):
    eqArrNew=np.zeros(axs1[-1]+axs2[-1]+1)
    eqIdxNew=np.zeros((axs1[-1]+axs2[-1]+1,2))

    for k in range (0,workArray.shape[0] + workArray.shape[1]-1):
        firstIdx=-1
        lastIdx=-1
        T_total=0
        for j in range (min(k,workArray.shape[0]-1),max(-1,k-workArray.shape[1]),-1):
            i = k-j
            T_total=(axs1[i]+axs2[j])
            if (firstIdx == -1 and abs(workArray[i][j]-eqArr[T_total])/eqArr[T_total]<0.002):
                firstIdx = i
            if (abs(workArray[i][j]-eqArr[T_total])/eqArr[T_total]<0.002):
                lastIdx = i
            # print "%d %f" % (T_total,abs(workArray[i][j]-eqArr[T_total])/eqArr[T_total])
            # if (eqArr[T_total] == 0 or eqArr[T_total] > workArray[i][j]):
            #     eqArr[T_total] = workArray[i][j]
            #     eqIdx[T_total][:]=(axs1[i],axs2[j])
        j_mean = int((firstIdx + lastIdx)/2)
        i_mean = k - j_mean
        eqArrNew[T_total] = workArray[j_mean][i_mean]
        eqIdxNew[T_total][:]=(axs1[j_mean],axs2[i_mean])

    return (eqArrNew, eqIdxNew)


def CalcAlmostBestEqualCycleVertical(eqArr):
    ArrNum = int(workArray.shape[1])
    eqArr=np.zeros(ArrNum)
    eqIdx=np.zeros((ArrNum,2))
    for i in range (workArray.shape[0]):
        for j in range (workArray.shape[1]):
            T_total=j
            if (eqArr[T_total] == 0 or eqArr[T_total] > workArray[i][j]):
                eqArr[T_total] = workArray[i][j]
                eqIdx[T_total][:]=(axs1[i],int(axs2[j]))

    return (eqArr, eqIdx)

    ArrNum = int(workArray.shape[1])
    eqArrNew=np.zeros(ArrNum)
    eqIdxNew=np.zeros((ArrNum,2))

    for i in range (0,workArray.shape[0]):
        firstIdx=-1
        lastIdx=-1
        T_total=0
        for j in range (0,workArray.shape[1]):
            T_total=j
            if (firstIdx == -1 and abs(workArray[i][j]-eqArr[T_total])/eqArr[T_total]<0.002):
                firstIdx = i
            if (abs(workArray[i][j]-eqArr[T_total])/eqArr[T_total]<0.002):
                lastIdx = i
            # print "%d %f" % (T_total,abs(workArray[i][j]-eqArr[T_total])/eqArr[T_total])
            # if (eqArr[T_total] == 0 or eqArr[T_total] > workArray[i][j]):
            #     eqArr[T_total] = workArray[i][j]
            #     eqIdx[T_total][:]=(axs1[i],axs2[j])
        j_mean = int((firstIdx + lastIdx)/2)
        i_mean = k - j_mean
        eqArrNew[T_total] = workArray[j_mean][i_mean]
        eqIdxNew[T_total][:]=(axs1[j_mean],axs2[i_mean])

    return (eqArrNew, eqIdxNew)

def ShowBestEqualCycle(eqArr, eqIdx):
    highest_cycle=T_4_high * (1 + lambda_3/(mu_3-lambda_3))
    idxs_to_show=((np.arange(eqArr.shape[0])[np.logical_and(eqArr>0,(eqIdx[:,0]+eqIdx[:,1])<highest_cycle)])[::1])
    plt.plot([x[1] for x in eqIdx[idxs_to_show]], [x[0] for x in eqIdx[idxs_to_show]], color='darkviolet', label='Best target. Fixed cycle-length')
    plt.legend()
    
def ShowBestEqualCycleShifted(eqArr, eqIdx):
    beta=(mu_4 * lambda_3)/(lambda_1 * mu_3 )
    highest_cycle=T_4_high * (1 + beta*lambda_3/(mu_3-lambda_3))
    idxs_to_show=((np.arange(eqArr.shape[0])[np.logical_and(eqArr>0,(beta*eqIdx[:,0]+eqIdx[:,1])<highest_cycle)])[::1])
    plt.plot([x[1] for x in eqIdx[idxs_to_show]], [x[0] for x in eqIdx[idxs_to_show]], color='darkviolet', label='Best target. Fixed cycle-length')
    plt.legend()
    
def ShowBestEqualCycleVertical(eqArr, eqIdx):
    highest_cycle=workArray.shape[1]
    idxs_to_show=((np.arange(eqArr.shape[0])[np.logical_and(eqArr>0,0<highest_cycle)])[::1])
    plt.plot([x[1] for x in eqIdx[idxs_to_show]], [x[0] for x in eqIdx[idxs_to_show]], color='darkviolet', label='Best target. Vertical')
    plt.legend()



    

agg_arr=[]
for suffix in suffix_arr:
    workFolder = "/home/victor/Development/tandem_imitate/interpretation/favorite_logs/" + suffix
    # axs1 = np.arange(5,31,5)
    # axs2 = np.arange(5,31,5)
    # (T_3_low, T_3_high, T_4_low, T_4_high) = (1,30,0,30)
    #(T_3_low, T_3_high, T_4_low, T_4_high) = (5,30,5,30)
    # (T_3_low, T_3_high, T_4_low, T_4_high) = (1,50,1,50)
    # (T_3_low, T_3_high, T_3_step, T_4_low, T_4_high, T_4_step) = (1,100,2,1,100,2)
    (T_3_low, T_3_high, T_3_step, T_4_low, T_4_high, T_4_step) = (1,100,4,1,100,4)

    axs1 = np.arange(T_3_low,T_3_high + 1,T_3_step)
    axs2 = np.arange(T_4_low,T_4_high + 1,T_4_step)

    stationarTheoreticalName = workFolder + "/stationaryReachingT"
    stationarFactName = workFolder + "/stationaryReachingF"
    targetName = workFolder + "/target"
    specName = workFolder + "/spec"
    loadsHighName= workFolder + "/loadHigh"
    loadsLowName= workFolder + "/loadLow"

    points = list(product(axs1,axs2))

    timestamp=time.strftime("%Y%m%d-%H%M%S")
    picturesFolder= "/home/victor/Development/tandem_imitate/interpretation" + "/Pictures/" + suffix.replace(".","_")

    if not os.path.exists(picturesFolder):
        os.makedirs(picturesFolder)

    workArray = np.loadtxt ( specName )
    (lambda_1, mu_1, lambda_3, mu_3, mu_4, thresh, prol) = workArray[0:7]
    # thresh=5


    BINS=13
    # for filename in (targetName, stationarTheoreticalName, stationarFactName):
    for filename in ([targetName]):
        print filename
        workArray = np.loadtxt ( filename )
        agg_arr.append(workArray)
        
avg_arr = np.mean(agg_arr, axis=0)
work_array = avg_arr

picturesFolder= "/home/victor/Development/tandem_imitate/interpretation" + "/Pictures/" + "new/averaged"

if not os.path.exists(picturesFolder):
    os.makedirs(picturesFolder)

BINS=13
myPercentiles = np.percentile(workArray, np.linspace(0, 100, BINS))

# (myEqArr, myEqIdx)=CalcBestEqualCycle()
# # (myEqArr, myEqIdx) = CalcAlmostBestEqualCycle(myEqArr)
# ShowBestEqualCycle(myEqArr, myEqIdx)


# (myEqArr, myEqIdx)=CalcBestEqualCycleShifted()
# # (myEqArr, myEqIdx) = CalcAlmostBestEqualCycle(myEqArr)
# ShowBestEqualCycleShifted(myEqArr, myEqIdx)

(myEqArr, myEqIdx)=CalcBestEqualCycleVertical()
# (myEqArr, myEqIdx) = CalcAlmostBestEqualCycle(myEqArr)
ShowBestEqualCycleVertical(myEqArr, myEqIdx)

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
ShowSampleLoads()
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


