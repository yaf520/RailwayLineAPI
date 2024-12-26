//
//  MileConvert.cpp
//  RailwayAPI
//
//  Created by 易傲飞 on 2024/12/25.
//

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include "MileConvert.hpp"
#include "BaseCalFun.hpp"

MileConvert::MileConvert()
{
    m_pDLArr = nullptr;
    m_nDLCount = 0;
}

MileConvert::~MileConvert()
{
    if (m_pDLArr)
    {
        delete [] m_pDLArr;
        m_nDLCount = 0;
    }
}

///设置断链数据
bool MileConvert::SetDLData(const tagDLInfo* pDLInfo, int count)
{
    if (count <= 0)
        return false;
    m_nDLCount = count;
    m_pDLArr = new tagDLInfo[m_nDLCount];
    for (uint32_t i = 0; i < m_nDLCount; i++)
        m_pDLArr[i] = *(pDLInfo + i);
    
    return true;
}

double MileConvert::GetMilepost(double dConitinueMile, char strMark[16], int& iReliability, char strErr[64])
{
    double dMileValue = __DBL_MIN__;
    strMark[0] = '\0';
    strErr[0] = '\0';
    
    double dConMileTab = 0.0;
    int iDl = 0;
    iReliability = 1;
    
    dConMileTab = m_pDLArr[0].dBehLich;
    if (dConitinueMile < dConMileTab)
    {
        snprintf(strErr, 64, "输入里程超出范围");
        iReliability = 0;
        return 0.0;
    }
    else if (fabs(dConMileTab - dConitinueMile) <= 0.0000001)
    {
        if (fabs(m_pDLArr[0].dBehLich - floor(m_pDLArr[0].dBehLich + 0.0000001)) < 0.000001)
        {
            strcpy(strMark, m_pDLArr[0].strBehNo);
            dMileValue = m_pDLArr[0].dBehLich;
        }
        else
        {
            strcpy(strMark, m_pDLArr[0].strBefNo);
            dMileValue = m_pDLArr[0].dBefLich;
        }
        
        iReliability = 1;
        return dMileValue;
    }
    
    for (iDl = 1; iDl < m_nDLCount; iDl++)
    {
        dConMileTab = dConMileTab + m_pDLArr[iDl].dBefLich - m_pDLArr[iDl - 1].dBehLich;
        
        if (dConitinueMile < dConMileTab)
        {
            dMileValue = m_pDLArr[iDl].dBefLich - (dConMileTab - dConitinueMile);
            if (strcmp(m_pDLArr[iDl - 1].strBehNo, m_pDLArr[iDl].strBefNo) == 0)
            {
                strcpy(strMark, m_pDLArr[iDl].strBefNo);
            }
            else
            {
                if (m_pDLArr[iDl - 1].dBehLich < m_pDLArr[iDl - 1].dBefLich
                    && dMileValue < m_pDLArr[iDl - 1].dBefLich)
                {
                    strcpy(strMark, m_pDLArr[iDl - 1].strBehNo);
                }
                else if (m_pDLArr[iDl].dBehLich < m_pDLArr[iDl].dBefLich
                         && dMileValue < m_pDLArr[iDl].dBefLich)
                {
                    strcpy(strMark, m_pDLArr[iDl].strBefNo);
                }
                else
                {
                    if (2.0 * (dConMileTab - dConitinueMile) > m_pDLArr[iDl].dBefLich - m_pDLArr[iDl - 1].dBehLich)
                        strcpy(strMark, m_pDLArr[iDl - 1].strBehNo);
                    else
                        strcpy(strMark, m_pDLArr[iDl].strBefNo);
                }
            }
            
            break;
        }
        else if (fabs(dConMileTab - dConitinueMile) <= 0.0000001)
        {
            if (fabs(m_pDLArr[iDl].dBefLich - floor(m_pDLArr[iDl].dBefLich + 0.0000001)) < 0.000001)
            {
                strcpy(strMark, m_pDLArr[iDl].strBefNo);
                dMileValue = m_pDLArr[iDl].dBefLich;
            }
            else
            {
                strcpy(strMark, m_pDLArr[iDl].strBehNo);
                dMileValue = m_pDLArr[iDl].dBehLich;
            }
            
            break;
        }
    }
    
    if (iDl >= m_nDLCount)
    {
        snprintf(strErr, 64, "输入里程超出范围");
        iReliability = 0;
        return 0.0;
    }
    
    return dMileValue;
}

double MileConvert::GetSumLengthOfMile(char strMark[16], double dMileValue, int& iReliability, char strErr[64])
{
    double dConMile = 0.0;
    double dTmpConMile = 0.0;
    char strMarkCpy[16] = {0};
    int shCounter = 0;
    int iDl = 0;
    bool bLawless = false;
    
    if (strlen(strMark) < 1)
    {
        snprintf(strErr, 64, "没有里程冠号");
        iReliability = 0;
    }
    
    if (m_nDLCount  == 0)
    {
        snprintf(strErr, 64, "无断链表,无法进行里程转换");
        iReliability = 0;
        return 0.0;
    }
    
    //转大写
    for (int i = 0; strMark[i] != '\0'; i++)
        strMark[i] = toupper(strMark[i]);
    
    dTmpConMile = 0.0;
    for (iDl = 1; iDl < m_nDLCount; iDl++)
    {
        if (m_pDLArr[iDl - 1].dBehLich <= dMileValue && dMileValue <= m_pDLArr[iDl].dBefLich)
        {
            strcpy(strMarkCpy, m_pDLArr[iDl - 1].strBehNo);
            
            for (int i = 0; strMarkCpy[i] != '\0'; i++)
                strMarkCpy[i] = toupper(strMarkCpy[i]);
            
            if (strcmp(strMark, strMarkCpy) == 0)
            {
                dConMile = dTmpConMile + dMileValue - m_pDLArr[iDl - 1].dBehLich;
                shCounter++;
            }
            else
            {
                strcpy(strMarkCpy, m_pDLArr[iDl].strBefNo);
                
                for (int i = 0; strMarkCpy[i] != '\0'; i++)
                    strMarkCpy[i] = toupper(strMarkCpy[i]);
                
                if (strcmp(strMark, strMarkCpy) == 0)
                {
                    dConMile = dTmpConMile + dMileValue - m_pDLArr[iDl - 1].dBehLich;
                    shCounter++;
                }
            }
        }
        dTmpConMile = dTmpConMile + m_pDLArr[iDl].dBefLich - m_pDLArr[iDl - 1].dBehLich;
    }
    
    if (shCounter == 0)
    {
        uint64_t dTmpDecDgts = powl(10, BaseCalFun::CalDecimalDigits(dMileValue));
        if (floor(m_pDLArr[0].dBehLich * dTmpDecDgts) / dTmpDecDgts <= dMileValue && dMileValue <= m_pDLArr[0].dBehLich)
        {
            dConMile = 0.0;
            shCounter++;
        }
        else if (m_pDLArr[m_nDLCount - 1].dBefLich <= dMileValue &&
                 dMileValue <= ceil(m_pDLArr[m_nDLCount - 1].dBefLich * dTmpDecDgts) / dTmpDecDgts)
        {
            dConMile = dTmpConMile + (dMileValue - m_pDLArr[m_nDLCount - 1].dBefLich);
            shCounter++;
        }
        else if (dMileValue > m_pDLArr[m_nDLCount - 1].dBehLich)
        {
            dConMile = dTmpConMile + (dMileValue - m_pDLArr[m_nDLCount - 1].dBehLich);
            shCounter++;
            iReliability = 1;
            bLawless = true;
            snprintf(strErr, 64, "%s%0.3f超过了本方案设计终点", strMark, dMileValue);
        }
        else if (dMileValue < m_pDLArr[0].dBefLich)
        {
            dConMile = dMileValue - m_pDLArr[0].dBefLich;
            shCounter++;
            iReliability = 1;
            bLawless = true;
            snprintf(strErr, 64, "%s%0.3f在本方案设计起点之前", strMark, dMileValue);
        }
    }
    
    if (shCounter == 1)
    {
        if (!bLawless)
        {
            iReliability = 1;
            snprintf(strErr, 64, "");
        }
    }
    else if (shCounter > 1)
    {
        iReliability = 0;
        snprintf(strErr, 64, "%s%0.3f在线路上有%d个位置点，断链设置不合理", strMark, dMileValue, shCounter);
    }
    else
    {
        dConMile = dMileValue;
        iReliability = 0;
        snprintf(strErr, 64, "本方案上没有%s%0.3f里程点", strMark, dMileValue);
    }
    
    return dConMile;
}

bool MileConvert::TrsCmltoCkml(const double& cml, char ckml[64], int nPrec)
{
    
#if 0
    
    if (m_iDLCount <= 0) return;
    double dMaxLen = GetLength();
    double dCml = (cml > dMaxLen ? dMaxLen : cml);
    
    for (uint32_t i = 0; i + 1 < m_iDLCount; i++)
    {
        //后一段断前与前一段断后差值
        double dDeltaDis = m_pDLArr[i + 1].dBefLich - m_pDLArr[i].dBehLich;
        if (dCml < dDeltaDis)
        {
            //里程在这个区间
            double dRetLen = m_pDLArr[i].dBehLich + dCml;
            BaseCalFun::to_stringFormat(m_pDLArr[i].strBehNo, dRetLen, ckml);
            
            break;
        }
        else
        {
            //继续下一区间查找
            dCml -= dDeltaDis;
        }
    }
    
#else
    if (m_nDLCount == 0)
        return false;
    
    char strMark[16] = {0};
    int iReliability = 0;
    char strErr[64] = {0};
    
    double dCkml = GetMilepost(cml + m_pDLArr[0].dBehLich, strMark, iReliability, strErr);
    if (iReliability)
        BaseCalFun::MileCharacterStr(strMark, dCkml, nPrec, false, ckml);
    else
        strcpy(ckml, strErr);
    
    return iReliability;
#endif
    
}

bool MileConvert::TrsCkmlToCml(char ckml[64], double& cml, char strErr[64])
{
    double dMileValue = 0.0;
    char strNO[16] = {0};
    BaseCalFun::SplitMilepost(ckml, dMileValue, strNO);
    
    int iReliability = 0;
    cml = GetSumLengthOfMile(strNO, dMileValue, iReliability, strErr);
    return iReliability;
}
