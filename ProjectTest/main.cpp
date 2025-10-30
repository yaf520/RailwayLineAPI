//
//  main.cpp
//  ProjectTest
//
//  Created by 易傲飞 on 2023/11/3.
//

#include <iostream>
#include <fstream>
#include <assert.h>
#include <vector>
#include "RouteAPI.hpp"

using namespace std;

///读取EI文件
bool LoadEIFile(std::string strDir, std::vector<tagJDInfo>& vecJD)
{
    ifstream infile;
    infile.open(strDir, ios::in);
    
    if (infile.is_open())
    {
        std::string buffer;
        const std::string strBlank = " ";
        
        while (getline(infile, buffer)) {
            size_t nDataIndex = 0;
            
            size_t nStartIndex = 0;
            size_t nEndIndex = 0;
            tagJDInfo JDInfo;
            do {
                std::string strSub;
                nEndIndex = buffer.find(strBlank, nStartIndex);
                if (nEndIndex == std::string::npos)
                {
                    strSub = buffer.substr(nStartIndex);
                    if (JDInfo.nJDType == JDType::End)
                        JDInfo.dX = std::stod(strSub);
                    else if (JDInfo.nJDType == JDType::Start)
                    {
                        //起点桩号
                    }
                    else
                    {
                        double dExitR = std::stod(strSub);
                        JDInfo.dExitR = (dExitR == 10000000000.0 ? __DBL_MAX__ : dExitR);
                    }
                    break;
                }
                
                strSub = buffer.substr(nStartIndex, nEndIndex - nStartIndex);
                if (nDataIndex == 0)
                    JDInfo.nJDType = (JDType)std::stoi(strSub);
                else if (nDataIndex == 1)
                    JDInfo.dY = std::stod(strSub);
                else if (nDataIndex == 2)
                    JDInfo.dX = std::stod(strSub);
                else if (JDInfo.nJDType != JDType::Start && JDInfo.nJDType != JDType::End)
                {
                    if (JDInfo.nJDType == JDType::ThreeUnit)
                    {
                        if (nDataIndex == 3)
                        {
                            double dEnterR = std::stod(strSub);
                            JDInfo.dEnterR = (dEnterR == 10000000000.0 ? __DBL_MAX__ : dEnterR);
                        }
                        else if (nDataIndex == 4)
                        {
                            size_t nType = strSub.find("L");
                            if (nType == std::string::npos)
                                JDInfo.dA1 = std::stod(strSub.substr(nType + 1));
                            else
                                JDInfo.dL1 = std::stod(strSub.substr(nType + 1));
                        }
                        else if (nDataIndex == 5)
                            JDInfo.dArcR1 = std::stod(strSub);
                        else if (nDataIndex == 6)
                        {
                            size_t nType = strSub.find("L");
                            if (nType == std::string::npos)
                                JDInfo.dA2 = std::stod(strSub.substr(nType + 1));
                            else
                                JDInfo.dL2 = std::stod(strSub.substr(nType + 1));
                        }
                    }
                    else if (JDInfo.nJDType == JDType::ThreeUnitBack)
                    {
                        if (nDataIndex == 3)
                            JDInfo.dY_End = std::stod(strSub);
                        else if (nDataIndex == 4)
                            JDInfo.dX_End = std::stod(strSub);
                        else if (nDataIndex == 5)
                        {
                            double dEnterR = std::stod(strSub);
                            JDInfo.dEnterR = (dEnterR == 10000000000.0 ? __DBL_MAX__ : dEnterR);
                        }
                        else if (nDataIndex == 6)
                        {
                            size_t nType = strSub.find("L");
                            if (nType == std::string::npos)
                                JDInfo.dA1 = std::stod(strSub.substr(nType + 1));
                            else
                                JDInfo.dL1 = std::stod(strSub.substr(nType + 1));
                        }
                        else if (nDataIndex == 7)
                            JDInfo.dArcR1 = std::stod(strSub);
                        else if (nDataIndex == 8)
                        {
                            size_t nType = strSub.find("L");
                            if (nType == std::string::npos)
                                JDInfo.dA2 = std::stod(strSub.substr(nType + 1));
                            else
                                JDInfo.dL2 = std::stod(strSub.substr(nType + 1));
                        }
                    }
                    else if (JDInfo.nJDType == JDType::FiveUnit)
                    {
                        
                    }
                    else if (JDInfo.nJDType == JDType::FiveUnitBack)
                    {
                        
                    }
                }
                
                nStartIndex = nEndIndex + strBlank.length();
                nDataIndex++;
            } while (true);
            
            vecJD.emplace_back(JDInfo);
        }
    }
    
    return true;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    //std::cout << "Hello, World!\n";
    cout << fixed;
    cout.precision(7);
    
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
     
//#define FIVE_UNIT 五单元平曲线测试
        
#ifdef FIVE_UNIT
        
    //五单元曲线
    JDInfo.dY = 8929.8900452472;
    JDInfo.dX = 22307.7869113941;
    vecJD.emplace_back(JDInfo);
    
    JDInfo.nJDType = JDType::FiveUnit;
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
    
    JDInfo.dY = 7446.9206357642;
    JDInfo.dX = 14772.6551081784;
    vecJD.emplace_back(JDInfo);
    
#else

    LoadEIFile("/Users/yiaofei/Downloads/公路平曲线/1.JD", vecJD);
    
#endif
    
    //变量定义
    double dCml = 0.0, dDist = 0.0, dFwj = 0.0, dX = 0.0, dY = 0.0;
    //步长
    const double dStep = 1.0615;
    
    RouteAPI* pAPI = new RouteAPI;
    pAPI->SetData(vecJD.data(), vecJD.size(), vecDL.size() > 0 ? vecDL.data() : nullptr, vecDL.size(), vecSlope.size() > 0 ? vecSlope.data() : nullptr, vecSlope.size());
    
    double dLen1 = pAPI->GetLength();
    
    Point2d p4(11437.1811917216, 6829.9712457906);
    Point2d p5(11451.2969, 6189.4564);
    Vector2d vecOfffset = p5 - p4;
    
    pAPI->UpdateJD(2, vecOfffset);
    
    double dLen2 = pAPI->GetLength();
    
    //4
    //6829.9712457906 11437.1811917216
    
    //5
    //6189.4564 11451.2969 6000.0000000000
    
//#define SINGLE_DATA_TEST //单点测试
    
#ifdef SINGLE_DATA_TEST
    
    pAPI->TrsCmlDistToNE(46570.10750, 300, dY, dX, dFwj);
    uint32_t nCount = 0;
    auto p = pAPI->TrsNEToCmlDist(dY, dX, nCount);
    
#endif
    
#define KEY_POS_TEST //关键点测试
    
#ifdef KEY_POS_TEST
    
    int nArrCount = 0;
    const tagExportLineElement* pArrLineElement = pAPI->ExportHorCurve(nArrCount, 0.0, pAPI->GetLength(), 0.0, 1.0);
    for (int i = 0; i < nArrCount; i++)
    {
        int nStartIndex = 0;
        int nEndIndex = (pArrLineElement[i].eLineType == ElementType::Arc ? 1 : pArrLineElement[i].nPosCount - 1);
        
        assert(pAPI->TrsNEToCmlDist(pArrLineElement[i].pArrPos[nStartIndex].dY, pArrLineElement[i].pArrPos[nStartIndex].dX, dCml, dDist, dFwj));
        assert(abs(dDist) < 1.0e-6);
        assert(pAPI->TrsNEToCmlDist(pArrLineElement[i].pArrPos[nEndIndex].dY, pArrLineElement[i].pArrPos[nEndIndex].dX, dCml, dDist, dFwj));
        assert(abs(dDist) < 1.0e-6);
    }
    delete[] pArrLineElement;
    
#endif
     
    char buffer[200] = {0};
    dDist = 98.784;
    double dTotalLen = pAPI->GetLength();
    for (dCml = 0.0; dCml <= dTotalLen; dCml += dStep)
    {
        dCml = std::min(dCml, dTotalLen);
        pAPI->TrsCmlDistToNE(dCml, dDist, dY, dX, dFwj);
        snprintf(buffer, sizeof(buffer), "dCml: %0.5f, dDist: %0.5f =====> dX: %0.5f, dY: %0.5f, dAngle: %0.5f", dCml, dDist, dX, dY, dFwj);
        cout << buffer << endl;
        
        bool bFind = false;
        
        uint32_t nCount = 0;
        tagCmlDistAngle* arrRet = pAPI->TrsNEToCmlDist(dY, dX, nCount);
        if (nCount > 0)
        {
            for (int nIndex = 0; nIndex < nCount; nIndex++)
            {
                if (abs(arrRet[nIndex].dCml - dCml) < 1.0e-5)
                {
                    snprintf(buffer, sizeof(buffer), "dX: %0.5f, dY: %0.5f =====> dCml: %0.5f, dDist: %0.5f, dAngle: %0.5f", dX, dY, arrRet[nIndex].dCml, arrRet[nIndex].dDist, arrRet[nIndex].dAngle);
                    cout << buffer << endl;
                    assert(abs(dDist - arrRet[nIndex].dDist) < 0.01);
                    bFind = true;
                    break;
                }
            }
            
            delete [] arrRet;
            assert(bFind);
        }
        
        
        /*
        uint32_t nCount = 0;
        tagCmlDistAngle* pArr = pAPI->TrsNEToCmlDist(dY, dX, nCount);
        for (int nIndex = 0; nIndex < nCount; nIndex++)
        {
            if (abs(pArr[nIndex].dCml - dCml) < 1.0e-5)
            {
                snprintf(buffer, sizeof(buffer), "dX: %0.5f, dY: %0.5f =====> dCml: %0.5f, dDist: %0.5f, dAngle: %0.5f", dX, dY, pArr[nIndex].dCml, pArr[nIndex].dDist, pArr[nIndex].dFwj);
                cout << buffer << endl;
                assert(abs(dDist - pArr[nIndex].dDist) < 0.01);
                bFind = true;
                break;
            }
        }
        assert(bFind);
        delete [] pArr;
        */
        cout << endl;
    }
    
    delete pAPI;

    return 0;
}
