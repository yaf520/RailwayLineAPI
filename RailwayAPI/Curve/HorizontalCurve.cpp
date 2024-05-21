//
//  HorizontalObject.cpp
//  RailwayAPI
//
//  Created by 易傲飞 on 2023/11/6.
//

#include "HorizontalCurve.hpp"
#include <assert.h>
#include <stdio.h>
#include <ctype.h>

HorizontalCurve::HorizontalCurve()
{
    m_pDLArr = nullptr;
    m_nDLCount = 0;
}

HorizontalCurve::~HorizontalCurve()
{
    if (m_pDLArr)
        delete [] m_pDLArr;
}

//计算点的投影属于哪一线元
BaseLineElement* HorizontalCurve::PosBelongTo(Point2d pos)
{
    //查找距离起点最近的线元
    uint32_t nMinDisIndex = 0;
    double dMinDis = __DBL_MAX__;
    double dDisTemp = 0.0;
    for (uint32_t i = 0; i < m_nElementCount; i++)
    {
        dDisTemp = m_arrLineElement[i]->m_posStart.distanceTo(pos);
        if (dDisTemp < dMinDis)
        {
            dMinDis = dDisTemp;
            nMinDisIndex = i;
        }
    }
    
#ifdef USE_VECTOR
    
    Vector2d vecCal = pos - m_arrLineElement[nMinDisIndex]->m_posStart;
    if (vecCal.isZeroVec())
        return m_arrLineElement[nMinDisIndex];
    
    double dLineElementAngle = m_arrLineElement[nMinDisIndex]->m_dStartTanAngle;
    Vector2d vecTan(cos(dLineElementAngle), sin(dLineElementAngle));
    
    double dDot = BaseCalFun::Round(vecTan.dot(vecCal) / vecCal.model()) ;
    assert(dDot >= -1.0 && dDot <= 1.0);
    if (dDot < -1.0 || dDot > 1.0) return nullptr;
    //是否在前一线元
    bool bInPreElement = (dDot < 0.0);
    if (bInPreElement && nMinDisIndex == 0)
    {
        Vector2d vec = m_arrLineElement[nMinDisIndex]->m_posStart - m_arrLineElement[m_nElementCount - 1]->m_posEnd;
        if (vec.model() < 0.0001 &&
            abs(m_arrLineElement[nMinDisIndex]->m_dStartTanAngle - m_arrLineElement[m_nElementCount - 1]->m_dStartTanAngle) < s_dCalPrecision)
            return m_arrLineElement[m_nElementCount - 1];
        
        return nullptr;
    }
    
    //最终属于哪一线元
    return m_arrLineElement[(bInPreElement ? nMinDisIndex - 1 : nMinDisIndex)];
    
#else
    
    double dMinElementAngle = BaseCalFun::CalAngleX(m_arrLineElement[iMinDisIndex]->m_posStart, pos);
    double dLineElementAngle = m_arrLineElement[iMinDisIndex]->m_dStartTanAngle;
    double dDeltaAngle = abs(dMinElementAngle - dLineElementAngle);
    //是否在前一线元
    bool bInPreElement = (dDeltaAngle > MATH_PI_2 && dDeltaAngle < MATH_PI * 1.5);
    if (bInPreElement && iMinDisIndex == 0)
        return nullptr;
    
    //最终属于哪一线元
    return m_arrLineElement[(bInPreElement ? iMinDisIndex - 1 : iMinDisIndex)];
    
#endif
    
}

BaseLineElement* HorizontalCurve::CmlBelongTo(double dCml)
{
    dCml = __min(GetLength(), dCml);
    dCml = __max(0.0, dCml);
    
    //通过里程查询在哪一线元范围内(二分查找法)
    int left = 0;
    int right = m_nElementCount - 1;
    while (left <= right) {
        int mid = (left + right) / 2;
        if (m_arrLineElement[mid]->m_dStartCml > dCml)
            right = mid - 1;
        else
            left = mid + 1;
    }
    if (right < 0 || right >= m_nElementCount)
        return nullptr;
    
    return m_arrLineElement[right];
}

double HorizontalCurve::GetSumLengthOfMile(char strMark[16], double dMileValue, int& iReliability, char strErr[64])
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

double HorizontalCurve::GetMilepost(double dConitinueMile, char strMark[16], int& iReliability, char strErr[64])
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

///设置断链数据
void HorizontalCurve::SetDLData(const tagDLInfo* pDLInfo, uint32_t count)
{
    if (count <= 0) return;
    m_nDLCount = count;
    m_pDLArr = new tagDLInfo[m_nDLCount];
    for (uint32_t i = 0; i < m_nDLCount; i++)
        m_pDLArr[i] = *(pDLInfo + i);
}

//里程+投影计算坐标+切线角
bool HorizontalCurve::TrsCmlDistToNE(const double& dCml, const double& dDist, double& dX, double& dY, double& dAngle)
{
    BaseLineElement* pLineElement = CmlBelongTo(dCml);
    if (!pLineElement) return false;
    return pLineElement->TrsCmlDistToNE(dCml, dDist, dX, dY, dAngle);
}

//坐标计算投影点里程+投影距离+切线角
bool HorizontalCurve::TrsNEToCmlDist(const double& dX, const double& dY, double& dCml, double& dDist, double& dAngle)
{
    uint32_t nCount = 0;
    const tagCmlDistAngle* pArr = TrsNEToCmlDist(dX, dY, nCount);
    if (nCount > 0 && pArr)
    {
        dCml = pArr->dCml;
        dDist = pArr->dDist;
        dAngle = pArr->dFwj;
        
        delete [] pArr;
        
        return true;
    }
    
    return false;
}

tagCmlDistAngle* HorizontalCurve::TrsNEToCmlDist(const double& dX, const double& dY, uint32_t& nArrCount)
{
    nArrCount = 0;
    tagCmlDistAngle* pArrRet = nullptr;
    
    for (uint32_t nIndex = 0; nIndex < m_nElementCount; nIndex++)
    {
        double dCml = 0.0, dDist = 0.0, dAngle = 0.0;
        if (m_arrLineElement[nIndex]->TrsNEToCmlDist(dX, dY, dCml, dDist, dAngle))
        {
            if (!pArrRet)
                pArrRet = new tagCmlDistAngle[m_nElementCount];
            
            //插入排序
            uint32_t nInsertIndex = 0;
            for (nInsertIndex = 0; nInsertIndex < nArrCount; nInsertIndex++)
                if (abs(dDist) < abs(pArrRet[nInsertIndex].dDist))
                    break;
            
            if (nArrCount > 0 && nInsertIndex < nArrCount)
                memcpy(pArrRet + nInsertIndex + 1, pArrRet + nInsertIndex, (nArrCount - nInsertIndex) * sizeof(tagCmlDistAngle));
            
            pArrRet[nInsertIndex].dCml = dCml;
            pArrRet[nInsertIndex].dDist = -dDist;
            double dAngleTmp = MATH_PI / 2.0 - dAngle;
            BaseCalFun::KeepAngleIn2PI(dAngleTmp);
            pArrRet[nInsertIndex].dFwj = dAngleTmp;
                
            nArrCount++;
        }
    }
    
    //调整内存大小
    if (pArrRet && nArrCount < m_nElementCount)
    {
        tagCmlDistAngle* pNewArr = new tagCmlDistAngle[nArrCount];
        memcpy(pNewArr, pArrRet, sizeof(tagCmlDistAngle) * nArrCount);
        delete [] pArrRet;
        pArrRet = pNewArr;
    }
    
    return pArrRet;
}

double HorizontalCurve::GetLength()
{
    return m_arrLineElement[m_nElementCount - 1]->m_dStartCml + m_arrLineElement[m_nElementCount - 1]->m_dTotalLen;
}

bool HorizontalCurve::TrsCmltoCkml(const double& cml, char ckml[64], int nPrec)
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

bool HorizontalCurve::TrsCkmlToCml(char ckml[64], double& cml, char strErr[64])
{
    double dMileValue = 0.0;
    char strNO[16] = {0};
    BaseCalFun::SplitMilepost(ckml, dMileValue, strNO);
    
    int iReliability = 0;
    cml = GetSumLengthOfMile(strNO, dMileValue, iReliability, strErr);
    return iReliability;
}

