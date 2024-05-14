//
//  main.cpp
//  ProjectTest
//
//  Created by 易傲飞 on 2023/11/3.
//

#include <iostream>
#include <assert.h>
#include <vector>
#include "RailwayAPI.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
    // insert code here...
    //std::cout << "Hello, World!\n";
    cout << fixed;
    cout.precision(5);
    
    RailwayAPI* pAPI = new RailwayAPI;
    double dCml, dDist, dFwj, dX, dY, dHZ, dFyj;
    //交点信息
    std::vector<tagJDInfo> vecJD;
    //断链信息
    std::vector<tagDLInfo> vecDL;
    //坡度信息
    std::vector<tagSlopeInfo> vecSlope;
    //交点信息
    tagJDInfo JDInfo;
    //断链信息
    tagDLInfo DLInfo;
    //坡度信息
    tagSlopeInfo SlopeInfo;
    
    /*
    //宁芜-南京 DK10-DK39
    JDInfo.dX = 489281.586940352;
    JDInfo.dY = 3546242.31466608;
    JDInfo.dArcR = 0;
    JDInfo.dBL = 0;
    JDInfo.dFL = 0;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 487655.492167638;
    JDInfo.dY = 3544844.80995549;
    JDInfo.dArcR = 1000;
    JDInfo.dBL = 100;
    JDInfo.dFL = 100;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 486214.88333506;
    JDInfo.dY = 3543021.39995891;
    JDInfo.dArcR = 3504.1;
    JDInfo.dBL = 60;
    JDInfo.dFL = 60;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 485601.46934437;
    JDInfo.dY = 3542064.19085722;
    JDInfo.dArcR = 1995.85;
    JDInfo.dBL = 110;
    JDInfo.dFL = 110;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 484797.563685757;
    JDInfo.dY = 3541135.22301249;
    JDInfo.dArcR = 1195.82;
    JDInfo.dBL = 140;
    JDInfo.dFL = 140;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 482821.78605808;
    JDInfo.dY = 3540070.48092821;
    JDInfo.dArcR = 3495.91;
    JDInfo.dBL = 100;
    JDInfo.dFL = 100;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 482282.896952294;
    JDInfo.dY = 3539869.03247463;
    JDInfo.dArcR = 1204.22;
    JDInfo.dBL = 90;
    JDInfo.dFL = 90;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 481701.511658492;
    JDInfo.dY = 3539471.24726189;
    JDInfo.dArcR = 1195.82;
    JDInfo.dBL = 140;
    JDInfo.dFL = 140;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 480857.208213873;
    JDInfo.dY = 3539240.95972339;
    JDInfo.dArcR = 1004.29;
    JDInfo.dBL = 100;
    JDInfo.dFL = 100;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 479892.804501224;
    JDInfo.dY = 3538721.26493953;
    JDInfo.dArcR = 3004.1;
    JDInfo.dBL = 50;
    JDInfo.dFL = 50;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 479181.469396114;
    JDInfo.dY = 3538228.07643835;
    JDInfo.dArcR = 1995.85;
    JDInfo.dBL = 110;
    JDInfo.dFL = 110;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 478688.460523196;
    JDInfo.dY = 3537952.75178638;
    JDInfo.dArcR = 2504.107;
    JDInfo.dBL = 60;
    JDInfo.dFL = 60;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 477360.653107665;
    JDInfo.dY = 3537051.83671067;
    JDInfo.dArcR = 804.34;
    JDInfo.dBL = 150;
    JDInfo.dFL = 150;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 477316.465416408;
    JDInfo.dY = 3536153.73713046;
    JDInfo.dArcR = 800;
    JDInfo.dBL = 170;
    JDInfo.dFL = 170;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 476332.342204408;
    JDInfo.dY = 3534688.93885251;
    JDInfo.dArcR = 800;
    JDInfo.dBL = 170;
    JDInfo.dFL = 170;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 475178.49951908;
    JDInfo.dY = 3534241.14467909;
    JDInfo.dArcR = 1604.15;
    JDInfo.dBL = 80;
    JDInfo.dFL = 80;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 474030.646336566;
    JDInfo.dY = 3533581.43810364;
    JDInfo.dArcR = 1595.83;
    JDInfo.dBL = 120;
    JDInfo.dFL = 120;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 472666.13666518;
    JDInfo.dY = 3533233.77835697;
    JDInfo.dArcR = 3995.9;
    JDInfo.dBL = 110;
    JDInfo.dFL = 110;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 470633.981890279;
    JDInfo.dY = 3533120.06628721;
    JDInfo.dArcR = 800;
    JDInfo.dBL = 170;
    JDInfo.dFL = 170;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 470102.631444879;
    JDInfo.dY = 3533409.78240507;
    JDInfo.dArcR = 804.34;
    JDInfo.dBL = 150;
    JDInfo.dFL = 150;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 468907.572589936;
    JDInfo.dY = 3533619.16091138;
    JDInfo.dArcR = 500;
    JDInfo.dBL = 80;
    JDInfo.dFL = 80;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 468210.233616109;
    JDInfo.dY = 3534485.34222565;
    JDInfo.dArcR = 505;
    JDInfo.dBL = 50;
    JDInfo.dFL = 50;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 467394.236787673;
    JDInfo.dY = 3532905.29919833;
    JDInfo.dArcR = 800;
    JDInfo.dBL = 80;
    JDInfo.dFL = 80;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 466249.149601401;
    JDInfo.dY = 3532326.48669122;
    JDInfo.dArcR = 0;
    JDInfo.dBL = 0;
    JDInfo.dFL = 0;
    vecJD.emplace_back(JDInfo);
    
    
    DLInfo.dBefLich = 9833.44;
    DLInfo.dBehLich = 9833.44;
    snprintf(DLInfo.strBefNo, sizeof(DLInfo.strBefNo), "DK");
    snprintf(DLInfo.strBehNo, sizeof(DLInfo.strBehNo), "DK");
    vecDL.emplace_back(DLInfo);
    
    DLInfo.dBefLich = 12333.44;
    DLInfo.dBehLich = 12500;
    snprintf(DLInfo.strBefNo, sizeof(DLInfo.strBefNo), "DK");
    snprintf(DLInfo.strBehNo, sizeof(DLInfo.strBehNo), "DK");
    vecDL.emplace_back(DLInfo);
    
    DLInfo.dBefLich = 18699.697;
    DLInfo.dBehLich = 18700;
    snprintf(DLInfo.strBefNo, sizeof(DLInfo.strBefNo), "DK");
    snprintf(DLInfo.strBehNo, sizeof(DLInfo.strBehNo), "DK");
    vecDL.emplace_back(DLInfo);
    
    DLInfo.dBefLich = 23999.608;
    DLInfo.dBehLich = 24000;
    snprintf(DLInfo.strBefNo, sizeof(DLInfo.strBefNo), "DK");
    snprintf(DLInfo.strBehNo, sizeof(DLInfo.strBehNo), "DK");
    vecDL.emplace_back(DLInfo);
    
    DLInfo.dBefLich = 29447.795;
    DLInfo.dBehLich = 29400;
    snprintf(DLInfo.strBefNo, sizeof(DLInfo.strBefNo), "DK");
    snprintf(DLInfo.strBehNo, sizeof(DLInfo.strBehNo), "ADK");
    vecDL.emplace_back(DLInfo);
    
    DLInfo.dBefLich = 29500;
    DLInfo.dBehLich = 29500;
    snprintf(DLInfo.strBefNo, sizeof(DLInfo.strBefNo), "ADK");
    snprintf(DLInfo.strBehNo, sizeof(DLInfo.strBehNo), "DK");
    vecDL.emplace_back(DLInfo);
    
    DLInfo.dBefLich = 34196.118;
    DLInfo.dBehLich = 34200;
    snprintf(DLInfo.strBefNo, sizeof(DLInfo.strBefNo), "DK");
    snprintf(DLInfo.strBehNo, sizeof(DLInfo.strBehNo), "DK");
    vecDL.emplace_back(DLInfo);
    
    DLInfo.dBefLich = 37162.9508;
    DLInfo.dBehLich = 37000;
    snprintf(DLInfo.strBefNo, sizeof(DLInfo.strBefNo), "DK");
    snprintf(DLInfo.strBehNo, sizeof(DLInfo.strBehNo), "ADK");
    vecDL.emplace_back(DLInfo);
    
    DLInfo.dBefLich = 37200;
    DLInfo.dBehLich = 37200;
    snprintf(DLInfo.strBefNo, sizeof(DLInfo.strBefNo), "ADK");
    snprintf(DLInfo.strBehNo, sizeof(DLInfo.strBehNo), "DK");
    vecDL.emplace_back(DLInfo);
    
    DLInfo.dBefLich = 38912.6072;
    DLInfo.dBehLich = 38900;
    snprintf(DLInfo.strBefNo, sizeof(DLInfo.strBefNo), "DK");
    snprintf(DLInfo.strBehNo, sizeof(DLInfo.strBehNo), "K");
    vecDL.emplace_back(DLInfo);
    
    DLInfo.dBefLich = 39100.0619646316;
    DLInfo.dBehLich = 39099.9233646316;
    snprintf(DLInfo.strBefNo, sizeof(DLInfo.strBefNo), "K");
    snprintf(DLInfo.strBehNo, sizeof(DLInfo.strBehNo), "K");
    vecDL.emplace_back(DLInfo);
    
    
    SlopeInfo.dCml = 9840.0;
    SlopeInfo.dArcR = 3.0;
    SlopeInfo.dHeight = 10.471;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 10010.0;
    SlopeInfo.dArcR = 0;
    SlopeInfo.dHeight = 10.301;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 11400.0;
    SlopeInfo.dArcR = 0;
    SlopeInfo.dHeight = 10.718;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 13500.0;
    SlopeInfo.dArcR = 10000.0;
    SlopeInfo.dHeight = 10.869;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 14650.0;
    SlopeInfo.dArcR = 0;
    SlopeInfo.dHeight = 17.7;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 15350.0;
    SlopeInfo.dArcR = 0;
    SlopeInfo.dHeight = 21.9;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 16300.0;
    SlopeInfo.dArcR = 10000;
    SlopeInfo.dHeight = 25.814;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 17200.0;
    SlopeInfo.dArcR = 0;
    SlopeInfo.dHeight = 20.954;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 18850.0;
    SlopeInfo.dArcR = 0;
    SlopeInfo.dHeight = 11.056;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 19600.0;
    SlopeInfo.dArcR = 0;
    SlopeInfo.dHeight = 6.706;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 20700.0;
    SlopeInfo.dArcR = 0;
    SlopeInfo.dHeight = 1.134;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 21450.0;
    SlopeInfo.dArcR = 10000;
    SlopeInfo.dHeight = -2.022;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 21900.0;
    SlopeInfo.dArcR = 10000;
    SlopeInfo.dHeight = 0.228;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 22300.0;
    SlopeInfo.dArcR = 0;
    SlopeInfo.dHeight = -1.771;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 23200.0;
    SlopeInfo.dArcR = 0;
    SlopeInfo.dHeight = -4.599;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 24625.0;
    SlopeInfo.dArcR = 10000;
    SlopeInfo.dHeight = -11.768;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 25100.0;
    SlopeInfo.dArcR = 0;
    SlopeInfo.dHeight = -9.492;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 25560.0;
    SlopeInfo.dArcR = 0;
    SlopeInfo.dHeight = -7.514;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 29500.0;
    SlopeInfo.dArcR = 0;
    SlopeInfo.dHeight = 4.448385;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "ADK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 30700.0;
    SlopeInfo.dArcR = 0;
    SlopeInfo.dHeight = 7.903;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 33350.0;
    SlopeInfo.dArcR = 10000;
    SlopeInfo.dHeight = 18.503;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 34200.0;
    SlopeInfo.dArcR = 0;
    SlopeInfo.dHeight = 15.953;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 34750.0;
    SlopeInfo.dArcR = 10000;
    SlopeInfo.dHeight = 14.853;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 35400.0;
    SlopeInfo.dArcR = 10000;
    SlopeInfo.dHeight = 11.993;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 36400.0;
    SlopeInfo.dArcR = 10000;
    SlopeInfo.dHeight = 14.363;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 37050.0;
    SlopeInfo.dArcR = 15000;
    SlopeInfo.dHeight = 12.57420838;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "ADK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 38400.0;
    SlopeInfo.dArcR = 15000;
    SlopeInfo.dHeight = 13.51920838;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "DK");
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dCml = 39100.0;
    SlopeInfo.dArcR = 0;
    SlopeInfo.dHeight = 17.271;
    snprintf(SlopeInfo.strNo, sizeof(SlopeInfo.strNo), "K");
    vecSlope.emplace_back(SlopeInfo);
    */
     
    //#define FIVE_UNIT
        
    #ifdef FIVE_UNIT
        
        //五单元曲线
        JDInfo.dY = 8929.8900452472;
        JDInfo.dX = 22307.7869113941;
        vecJD.emplace_back(JDInfo);
        vecJD1.emplace_back(AcGePoint2d(JDInfo.dX, JDInfo.dY));
        
        JDInfo.nJDType = 5;
        JDInfo.dY = 6336.4158;
        JDInfo.dX = 19285.7296;
        JDInfo.dArcR1 = 2000.0;
        JDInfo.dArcR2 = 4000.0;
        JDInfo.dL1 = 300.0;
        JDInfo.dL2 = 300.0;
        JDInfo.dL3 = 300.0;
        JDInfo.dEnterR = __DBL_MAX__;
        JDInfo.dExitR = __DBL_MAX__;
        JDInfo.dID = 0.0;
        vecJD.emplace_back(JDInfo);
        vecJD1.emplace_back(AcGePoint2d(JDInfo.dX, JDInfo.dY));
        
        JDInfo.dY = 7446.9206357642;
        JDInfo.dX = 14772.6551081784;
        vecJD.emplace_back(JDInfo);
        vecJD1.emplace_back(AcGePoint2d(JDInfo.dX, JDInfo.dY));
        
    #else

        JDInfo.nJDType = -1;
        JDInfo.dY = 6923.823102;
        JDInfo.dX = 6068.774307;
        JDInfo.dArcR1 = 0.0;
        JDInfo.dL1 = 0.0;
        JDInfo.dL2 = 0.0;
        JDInfo.dEnterR = __DBL_MAX__;
        JDInfo.dExitR = __DBL_MAX__;
        vecJD.emplace_back(JDInfo);
        
        JDInfo.nJDType = 3;
        JDInfo.dY = 7666.0720415746;
        JDInfo.dX = 7925.9405660053;
        JDInfo.dArcR1 = 2000.0;
        JDInfo.dL1 = 400.0;
        JDInfo.dL2 = 400.0;
        JDInfo.dEnterR = __DBL_MAX__;
        JDInfo.dExitR = __DBL_MAX__;
        vecJD.emplace_back(JDInfo);
         
        JDInfo.nJDType = 3;
        JDInfo.dY = 6829.9712457906;
        JDInfo.dX = 11437.1811917216;
        JDInfo.dArcR1 = 6000;
        JDInfo.dL1 = 0.0;
        JDInfo.dL2 = 0.0;
        JDInfo.dEnterR = 6000.0;
        JDInfo.dExitR = 6000.0;
        vecJD.emplace_back(JDInfo);
         
        JDInfo.nJDType = 3;
        JDInfo.dY = 7446.9206357642;
        JDInfo.dX = 14772.6551081784;
        JDInfo.dArcR1 = 2000.0;
        JDInfo.dL1 = 200.0;
        JDInfo.dL2 = 0.0;
        JDInfo.dEnterR = __DBL_MAX__;
        JDInfo.dExitR = 2000.0;
        vecJD.emplace_back(JDInfo);
         
        JDInfo.nJDType = 3;
        JDInfo.dY = 6580.8047549563;
        JDInfo.dX = 18292.5365610525;
        JDInfo.dArcR1 = 4000.0;
        JDInfo.dL1 = 300.0;
        JDInfo.dL2 = 0.0;
        JDInfo.dEnterR = __DBL_MAX__;
        JDInfo.dExitR = 4000.0;
        vecJD.emplace_back(JDInfo);
        
        JDInfo.nJDType = 3;
        JDInfo.dY = 6631.7287959183;
        JDInfo.dX = 19629.8443797163;
        JDInfo.dArcR1 = 2000.0;
        JDInfo.dL1 = 300.0;
        JDInfo.dL2 = 300.0;
        JDInfo.dEnterR = 4000.0;
        JDInfo.dExitR = __DBL_MAX__;
        vecJD.emplace_back(JDInfo);
         
        JDInfo.nJDType = 4;
        JDInfo.dY = 8929.8900452472;
        JDInfo.dX = 22307.7869113941;
        JDInfo.dY_End = 9904.0310064550;
        JDInfo.dX_End = 21132.8602230030;
        JDInfo.dArcR1 = 800.0;
        JDInfo.dL1 = 400.0;
        JDInfo.dL2 = 400.0;
        JDInfo.dEnterR = __DBL_MAX__;
        JDInfo.dExitR = __DBL_MAX__;
        vecJD.emplace_back(JDInfo);
         
        JDInfo.nJDType = 3;
        JDInfo.dY = 1994.2213683958;
        JDInfo.dX = 16573.4198425560;
        JDInfo.dArcR1 = 600;
        JDInfo.dL1 = 300;
        JDInfo.dL2 = 300;
        JDInfo.dEnterR = __DBL_MAX__;
        JDInfo.dExitR = 3000.0;
        vecJD.emplace_back(JDInfo);

        JDInfo.nJDType = 3;
        JDInfo.dY = 8985.5078841175;
        JDInfo.dX = 19012.7730132892;
        JDInfo.dArcR1 = 3000.0;
        JDInfo.dL1 = 0.0;
        JDInfo.dL2 = 300.0;
        JDInfo.dEnterR = 3000.0;
        JDInfo.dExitR = __DBL_MAX__;
        vecJD.emplace_back(JDInfo);
         
        JDInfo.nJDType = 3;
        JDInfo.dY = 10999.2483919726;
        JDInfo.dX = 21383.2363567082;
        JDInfo.dArcR1 = 1500.0;
        JDInfo.dL1 = 200.0;
        JDInfo.dL2 = 0.0;
        JDInfo.dEnterR = __DBL_MAX__;
        JDInfo.dExitR = 1500.0;
        vecJD.emplace_back(JDInfo);
         
        JDInfo.nJDType = 3;
        JDInfo.dY = 12715.6821514557;
        JDInfo.dX = 21550.7928005448;
        JDInfo.dArcR1 = 500.0;
        JDInfo.dL1 = 200.0;
        JDInfo.dL2 = 200.0;
        JDInfo.dEnterR = 1500.0;
        JDInfo.dExitR = 2000.0;
        vecJD.emplace_back(JDInfo);
        
        JDInfo.nJDType = 3;
        JDInfo.dY = 11827.9255412088;
        JDInfo.dX = 20188.0260210685;
        JDInfo.dArcR1 = 2000.0;
        JDInfo.dL1 = 0.0;
        JDInfo.dL2 = 300.0;
        JDInfo.dEnterR = 2000.0;
        JDInfo.dExitR = __DBL_MAX__;
        vecJD.emplace_back(JDInfo);
        
        JDInfo.nJDType = 3;
        JDInfo.dY = 9056.0911245568;
        JDInfo.dX = 18655.3105786835;
        JDInfo.dArcR1 = 2000.0;
        JDInfo.dL1 = 200.0;
        JDInfo.dL2 = 0.0;
        JDInfo.dEnterR = __DBL_MAX__;
        JDInfo.dExitR = 2000.0;
        vecJD.emplace_back(JDInfo);
        
        JDInfo.nJDType = 3;
        JDInfo.dY = 8575.9217401184;
        JDInfo.dX = 18206.6639186605;
        JDInfo.dArcR1 = 1000.0;
        JDInfo.dL1 = 200.0;
        JDInfo.dL2 = 200.0;
        JDInfo.dEnterR = 2000.0;
        JDInfo.dExitR = 2500.0;
        vecJD.emplace_back(JDInfo);
        
        JDInfo.nJDType = 3;
        JDInfo.dY = 8485.1763475140;
        JDInfo.dX = 17546.5131152047;
        JDInfo.dArcR1 = 2500.0;
        JDInfo.dL1 = 0.0;
        JDInfo.dL2 = 200.0;
        JDInfo.dEnterR = 2500.0;
        JDInfo.dExitR = __DBL_MAX__;
        vecJD.emplace_back(JDInfo);

        
        JDInfo.nJDType = 3;
        JDInfo.dY = 8793.8671598653;
        JDInfo.dX = 12448.8769575797;
        JDInfo.dArcR1 = 1000.0;
        JDInfo.dL1 = 100.0;
        JDInfo.dL2 = 100.0;
        JDInfo.dEnterR = __DBL_MAX__;
        JDInfo.dExitR = __DBL_MAX__;
        vecJD.emplace_back(JDInfo);
        
        JDInfo.nJDType = 3;
        JDInfo.dY = 8251.2643253635;
        JDInfo.dX = 11741.5886529887;
        JDInfo.dArcR1 = 600.0;
        JDInfo.dL1 = 100.0;
        JDInfo.dL2 = 200.0;
        JDInfo.dEnterR = __DBL_MAX__;
        JDInfo.dExitR = __DBL_MAX__;
        vecJD.emplace_back(JDInfo);
        
        JDInfo.nJDType = 3;
        JDInfo.dY = 9642.1501690871;
        JDInfo.dX = 9503.7630423550;
        JDInfo.dArcR1 = 1000.0;
        JDInfo.dL1 = 200.0;
        JDInfo.dL2 = 200.0;
        JDInfo.dEnterR = __DBL_MAX__;
        JDInfo.dExitR = __DBL_MAX__;
        vecJD.emplace_back(JDInfo);
        
        JDInfo.nJDType = 3;
        JDInfo.dY = 9478.0586618932;
        JDInfo.dX = 8383.2527285064;
        JDInfo.dArcR1 = 1000.0;
        JDInfo.dL1 = 200.0;
        JDInfo.dL2 = 150.0;
        JDInfo.dEnterR = __DBL_MAX__;
        JDInfo.dExitR = __DBL_MAX__;
        vecJD.emplace_back(JDInfo);
        
        JDInfo.nJDType = 3;
        JDInfo.dY = 8636.1374615765;
        JDInfo.dX = 7378.4362275103;
        JDInfo.dArcR1 = 60.0;
        JDInfo.dL1 = 60.0;
        JDInfo.dL2 = 0.0;
        JDInfo.dEnterR = __DBL_MAX__;
        JDInfo.dExitR = 60.0;
        vecJD.emplace_back(JDInfo);
        
        JDInfo.nJDType = 4;
        JDInfo.dY = 8632.1188147997;
        JDInfo.dX = 7358.3454223503;
        JDInfo.dY_End = 8727.6131104866;
        JDInfo.dX_End = 7394.0751316094;
        JDInfo.dArcR1 = 60.0;
        JDInfo.dL1 = 0.0;
        JDInfo.dL2 = 0.0;
        JDInfo.dEnterR = 60.0;
        JDInfo.dExitR = 60.0;
        vecJD.emplace_back(JDInfo);
        
        JDInfo.nJDType = 3;
        JDInfo.dY = 8711.3934964201;
        JDInfo.dX = 7406.5936815824;
        JDInfo.dArcR1 = 60.0;
        JDInfo.dL1 = 0.0;
        JDInfo.dL2 = 60.0;
        JDInfo.dEnterR = 60.0;
        JDInfo.dExitR = __DBL_MAX__;
        vecJD.emplace_back(JDInfo);
        
        JDInfo.nJDType = -2;
        JDInfo.dY = 8520.881018;
        JDInfo.dX = 7436.814345;
        JDInfo.dArcR1 = 0.0;
        JDInfo.dL1 = 0.0;
        JDInfo.dL2 = 0.0;
        JDInfo.dEnterR = __DBL_MAX__;
        JDInfo.dExitR = __DBL_MAX__;
        vecJD.emplace_back(JDInfo);
        
    #endif
    
    pAPI->SetData(&vecJD.front(), vecJD.size(), &vecDL.front(), vecDL.size(), &vecSlope.front(), vecSlope.size());
    
//    int nArrCount = 0;
//    auto pArrLineElement = pAPI->ExportHorCurve(nArrCount, 0.0, pAPI->GetLength(), 0.0, 10.0);
//    for (int i = 0; i < nArrCount; i++)
//    {
//        int nStartIndex = 0;
//        int nEndIndex = (pArrLineElement[i].eLineType == ElementType::Arc ? 1 : pArrLineElement[i].nPosCount - 1);
//        
//        assert(pAPI->TrsNEToCmlDist(pArrLineElement[i].pArrPos[nStartIndex].dY, pArrLineElement[i].pArrPos[nStartIndex].dX, dCml, dDist, dFwj));
//        assert(abs(dDist) < 0.000001);
//        assert(pAPI->TrsNEToCmlDist(pArrLineElement[i].pArrPos[nEndIndex].dY, pArrLineElement[i].pArrPos[nEndIndex].dX, dCml, dDist, dFwj));
//        assert(abs(dDist) < 0.000001);
//    }
//    
//    delete[] pArrLineElement;
    
    char buffer[200] = {0};
    //char strErr[64] = {0};
    
    dDist = 500.0;
    double dTotalLen = pAPI->GetLength();
    for (dCml = 0; dCml <= dTotalLen; dCml += 1.3958)
    {
        dCml = std::min(dCml, dTotalLen);
        pAPI->TrsCmlDistToNE(dCml, dDist, dY, dX, dFwj);
        snprintf(buffer, sizeof(buffer), "dCml: %0.5f, dDist: %0.5f =====> dX: %0.5f, dY: %0.5f, dAngle: %0.5f", dCml, dDist, dX, dY, dFwj);
        cout << buffer << endl;
        
        bool bFind = false;
        uint32_t nCount = 0;
        tagCmlDistAngle* pArr = pAPI->TrsNEToCmlDist(dY, dX, nCount);
        for (int nIndex = 0; nIndex < nCount; nIndex++)
        {
            if (abs(pArr[nIndex].dDist - dDist) < 0.00001)
            {
                snprintf(buffer, sizeof(buffer), "dX: %0.5f, dY: %0.5f =====> dCml: %0.5f, dDist: %0.5f, dAngle: %0.5f", dX, dY, pArr[nIndex].dCml, pArr[nIndex].dDist, pArr[nIndex].dFwj);
                cout << buffer << endl;
                assert(abs(dCml - pArr[nIndex].dCml) < 0.00001);
                bFind = true;
                break;
            }
        }
        assert(bFind);
        delete [] pArr;
        
//        double dCmlTemp = 0.0;
//        double dDistTemp = 0.0;
//        double dAngleTemp = 0.0;
//        pAPI->TrsNEToCmlDist(dY, dX, dCmlTemp, dDistTemp, dAngleTemp);
//        snprintf(buffer, sizeof(buffer), "dX: %0.5f, dY: %0.5f =====> dCml: %0.5f, dDist: %0.5f, dAngle: %0.5f", dX, dY, dCmlTemp, dDistTemp, dAngleTemp);
//        cout << buffer << endl;
//        assert(abs(dCml - dCmlTemp) < 0.00001);
        
        /*
        pAPI->TrsCmltoCkml(dCml, buffer);
        cout << "Ckml: " << buffer << endl;
        pAPI->TrsCkmlToCml(buffer, dCml, strErr);
        cout << "Cml: " << dCml << endl;
        pAPI->GetDesignHeight(dCml, dHZ, dFyj);
        cout << "Cml: " << dCml << " Height: " << dHZ << " Fyj: " << dFyj << endl;
        */
        cout << endl;
    }
    
    delete pAPI;

    return 0;
}
