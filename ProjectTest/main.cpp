//
//  main.cpp
//  ProjectTest
//
//  Created by 易傲飞 on 2023/11/3.
//

#include <iostream>
#include <assert.h>
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
    
    api.SetData(&vecJD.front(), vecJD.size(), &vecDL.front(), vecDL.size(), &vecSlope.front(), vecSlope.size());
    */
    
    
    
    //曲线交点数据
    JDInfo.dX = 494434.708600988;
    JDInfo.dY = 3046345.07027477;
    JDInfo.dArcR = 0.0;
    JDInfo.dFL = 0.0;
    JDInfo.dBL = 0.0;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 494258.718726684;
    JDInfo.dY = 3044352.95420617;
    JDInfo.dArcR = 12000.0;
    JDInfo.dFL = 370.0;
    JDInfo.dBL = 370.0;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 493981.255393979;
    JDInfo.dY = 3042422.64930532;
    JDInfo.dArcR = 12000.0;
    JDInfo.dFL = 370.0;
    JDInfo.dBL = 370.0;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 493305.671077984 /*+ 23.927*/;
    JDInfo.dY = 3032712.60604076 /*- 17.986*/;
    JDInfo.dArcR = 8000.0;
    JDInfo.dFL = 590.0;
    JDInfo.dBL = 590.0;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 492147.938191911;
    JDInfo.dY = 3028026.88928905;
    JDInfo.dArcR = 9000.0;
    JDInfo.dFL = 530.0;
    JDInfo.dBL = 530.0;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 493796.243139339;
    JDInfo.dY = 3019302.46648355;
    JDInfo.dArcR = 11000.0;
    JDInfo.dFL = 410.0;
    JDInfo.dBL = 410.0;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.dX = 494159.091851016;
    JDInfo.dY = 3016277.94252242;
    JDInfo.dArcR = 0;
    JDInfo.dFL = 0;
    JDInfo.dBL = 0;
    vecJD.emplace_back(JDInfo);

    //坡度数据
    SlopeInfo.dArcR = 0.0;
    SlopeInfo.dCml = 195920.0;
    SlopeInfo.dHeight = 290.740633;
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dArcR = 25000.0;
    SlopeInfo.dCml = 197250.0;
    SlopeInfo.dHeight = 317.00091976;
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dArcR = 25000.0;
    SlopeInfo.dCml = 198800.0;
    SlopeInfo.dHeight = 335.29091976;
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dArcR = 25000.0;
    SlopeInfo.dCml = 199880.0;
    SlopeInfo.dHeight = 322.33091976;
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dArcR = 25000.0;
    SlopeInfo.dCml = 200880.0;
    SlopeInfo.dHeight = 328.33091976;
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dArcR = 25000.0;
    SlopeInfo.dCml = 205650.0;
    SlopeInfo.dHeight = 411.80591976;
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dArcR = 25000.0;
    SlopeInfo.dCml = 208150.0;
    SlopeInfo.dHeight = 383.05591976;
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dArcR = 25000.0;
    SlopeInfo.dCml = 211080.0;
    SlopeInfo.dHeight = 365.47591976;
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dArcR = 25000.0;
    SlopeInfo.dCml = 211980.0;
    SlopeInfo.dHeight = 375.82591976;
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dArcR = 25000.0;
    SlopeInfo.dCml = 213800.0;
    SlopeInfo.dHeight = 369.81991976;
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dArcR = 25000.0;
    SlopeInfo.dCml = 215050.0;
    SlopeInfo.dHeight = 360.19491976;
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dArcR = 25000.0;
    SlopeInfo.dCml = 217800.0;
    SlopeInfo.dHeight = 368.44491976;
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dArcR = 25000.0;
    SlopeInfo.dCml = 222100.0;
    SlopeInfo.dHeight = 282.44491976;
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dArcR = 25000.0;
    SlopeInfo.dCml = 223240.0;
    SlopeInfo.dHeight = 287.24691976;
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dArcR = 25000.0;
    SlopeInfo.dCml = 225960.0;
    SlopeInfo.dHeight = 274.83491976;
    vecSlope.emplace_back(SlopeInfo);
    
    SlopeInfo.dArcR = 0.0;
    SlopeInfo.dCml = 226300.0;
    SlopeInfo.dHeight = 270.95891976;
    vecSlope.emplace_back(SlopeInfo);
    
    DLInfo.dBefLich = 195920.0;
    DLInfo.dBehLich = 195920.0;
    snprintf(DLInfo.strBefNo, sizeof(DLInfo.strBefNo), "DK");
    snprintf(DLInfo.strBehNo, sizeof(DLInfo.strBehNo), "DK");
    vecDL.emplace_back(DLInfo);
    
    DLInfo.dBefLich = 195989.6124;
    DLInfo.dBehLich = 196000.0;
    snprintf(DLInfo.strBefNo, sizeof(DLInfo.strBefNo), "DK");
    snprintf(DLInfo.strBehNo, sizeof(DLInfo.strBehNo), "DK");
    vecDL.emplace_back(DLInfo);
    
    DLInfo.dBefLich = 226299.9996;
    DLInfo.dBehLich = 226299.9996;
    snprintf(DLInfo.strBefNo, sizeof(DLInfo.strBefNo), "DK");
    snprintf(DLInfo.strBehNo, sizeof(DLInfo.strBehNo), "DK");
    vecDL.emplace_back(DLInfo);
    
    pAPI->SetData(&vecJD.front(), vecJD.size(), &vecDL.front(), vecDL.size(), &vecSlope.front(), vecSlope.size());
    
    int nArrCount = 0;
    auto pArrLineElement = pAPI->ExportHorCurve(nArrCount, 0.0, pAPI->GetLength(), 0.0, 10.0);
//    uint32_t nUpdateIndex = 3;
//    double dUpdateX = 493305.671077984;
//    double dUpdateY = 3032712.60604076;
//    pAPI->UpdateHorJD(nUpdateIndex, dUpdateX + 23.927, dUpdateY - 17.986);
    
    char buffer[200] = {0};
    char strErr[64] = {0};
    
    dDist = 0.0;
    double dTotalLen = pAPI->GetLength();
    for (dCml = 0; dCml <= dTotalLen; dCml += 1.3958)
    {
        dCml = std::min(dCml, dTotalLen);
        pAPI->TrsCmlDistToNE(dCml, dDist, dY, dX, dFwj);
        snprintf(buffer, sizeof(buffer), "dCml: %0.5f, dDist: %0.5f =====> dX: %0.5f, dY: %0.5f, dAngle: %0.5f", dCml, dDist, dX, dY, dFwj);
        cout << buffer << endl;
        double dCmlTemp = 0.0;
        double dDistTemp = 0.0;
        double dAngleTemp = 0.0;
        pAPI->TrsNEToCmlDist(dY, dX, dCmlTemp, dDistTemp, dAngleTemp);
        snprintf(buffer, sizeof(buffer), "dX: %0.5f, dY: %0.5f =====> dCml: %0.5f, dDist: %0.5f, dAngle: %0.5f", dX, dY, dCmlTemp, dDistTemp, dAngleTemp);
        cout << buffer << endl;
        assert(abs(dCml - dCmlTemp) < 0.00001);
        
        pAPI->TrsCmltoCkml(dCml, buffer);
        cout << "Ckml: " << buffer << endl;
        pAPI->TrsCkmlToCml(buffer, dCml, strErr);
        cout << "Cml: " << dCml << endl;
        pAPI->GetDesignHeight(dCml, dHZ, dFyj);
        cout << "Cml: " << dCml << " Height: " << dHZ << " Fyj: " << dFyj << endl;
        
        cout << endl;
    }
    
    delete pAPI;

    return 0;
}
