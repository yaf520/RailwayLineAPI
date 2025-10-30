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
: LineElementManager(CurveType::HorizontalCurve)
{
    
}

HorizontalCurve::~HorizontalCurve()
{
    
}

//计算点的投影属于哪一线元
BaseLineElement* HorizontalCurve::PosBelongTo(Point2d pos) const
{
    //查找距离起点最近的线元
    uint32_t nMinDisIndex = 0;
    double dMinDis = __DBL_MAX__;
    double dDisTemp = 0.0;
    for (uint32_t i = 0; i < m_nElementCount; i++)
    {
        dDisTemp = m_arrLineElement[i]->pntStart.distanceTo(pos);
        if (dDisTemp < dMinDis)
        {
            dMinDis = dDisTemp;
            nMinDisIndex = i;
        }
    }
    
#ifdef USE_VECTOR
    
    Vector2d vecCal = pos - m_arrLineElement[nMinDisIndex]->pntStart;
    if (vecCal.isZeroVec())
        return m_arrLineElement[nMinDisIndex];
    
    double dLineElementAngle = m_arrLineElement[nMinDisIndex]->dStartTanAngle;
    Vector2d vecTan(cos(dLineElementAngle), sin(dLineElementAngle));
    
    double dDot = BaseCalFun::Round(vecTan.dot(vecCal) / vecCal.model()) ;
    assert(dDot >= -1.0 && dDot <= 1.0);
    if (dDot < -1.0 || dDot > 1.0) return nullptr;
    //是否在前一线元
    bool bInPreElement = (dDot < 0.0);
    if (bInPreElement && nMinDisIndex == 0)
    {
        Vector2d vec = m_arrLineElement[nMinDisIndex]->pntStart - m_arrLineElement[m_nElementCount - 1]->pntEnd;
        if (vec.model() < 0.0001 &&
            abs(m_arrLineElement[nMinDisIndex]->dStartTanAngle - m_arrLineElement[m_nElementCount - 1]->dStartTanAngle) < s_dCalPrecision)
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

BaseLineElement* HorizontalCurve::CmlBelongTo(double dCml) const
{
    if (dCml < 0.0 || dCml > GetLength() + s_dCalPrecision)
        return nullptr;
    
    //通过里程查询在哪一线元范围内(二分查找法)
    int left = 0;
    int right = m_nElementCount - 1;
    while (left <= right) {
        int mid = (left + right) / 2;
        if (m_arrLineElement[mid]->dStartCml > dCml)
            right = mid - 1;
        else
            left = mid + 1;
    }
    if (right < 0 || right >= m_nElementCount)
        return nullptr;
    
    return m_arrLineElement[right];
}

//里程+投影计算坐标+切线角
bool HorizontalCurve::TrsCmlDistToNE(double dCml, double dDist, double& dX, double& dY, double& dAngle) const
{
    BaseLineElement* pLineElement = CmlBelongTo(dCml);
    if (!pLineElement)
        return false;
    return pLineElement->TrsCmlDistToNE(dCml, dDist, dX, dY, dAngle);
}

//坐标计算投影点里程+投影距离+切线角
bool HorizontalCurve::TrsNEToCmlDist(double dX, double dY, double& dCml, double& dDist, double& dAngle) const
{
    uint32_t nCount = 0;
    tagCmlDistAngle* pArr = this->TrsNEToCmlDist(dX, dY, nCount);
    if (nCount > 0)
    {
        dCml = pArr->dCml;
        dDist = pArr->dDist;
        dAngle = pArr->dAngle;
        
        delete [] pArr;
        
        return true;
    }
    
    return false;
}

tagCmlDistAngle* HorizontalCurve::TrsNEToCmlDist(double dX, double dY, uint32_t& nArrCount) const
{
    nArrCount = 0;
    tagCmlDistAngle* pArrRet = nullptr;
    
    uint32_t nMaxCount = 8;
    for (uint32_t nIndex = 0; nIndex < m_nElementCount; nIndex++)
    {
        if (m_arrLineElement[nIndex]->dTotalLen < s_dLenPrecision)
            continue;
        
        double arrCml[s_nMaxArrCount] = {0.0};
        double arrDist[s_nMaxArrCount] = {0.0};
        double arrAngle[s_nMaxArrCount] = {0.0};
        
        uint32_t nCount = m_arrLineElement[nIndex]->TrsNEToCmlDist(dX, dY, arrCml, arrDist, arrAngle);
        if (nCount > 0)
        {
            if (!pArrRet)
                pArrRet = new tagCmlDistAngle[nMaxCount];
            
            if (nArrCount + nCount > nMaxCount)
            {
                nMaxCount *= 2;
                tagCmlDistAngle* pNewArr = new tagCmlDistAngle[nMaxCount];
                memcpy(pNewArr, pArrRet, sizeof(tagCmlDistAngle) * nArrCount);
                delete [] pArrRet;
                pArrRet = pNewArr;
            }
            
            for (int nProIndex = 0; nProIndex < nCount; nProIndex++)
            {
                //插入排序
                uint32_t nInsertIndex = 0;
                for (nInsertIndex = 0; nInsertIndex < nArrCount; nInsertIndex++)
                    if (abs(arrDist[nProIndex]) < abs(pArrRet[nInsertIndex].dDist))
                        break;
                
                if (nArrCount > 0 && nInsertIndex < nArrCount)
                    memmove(pArrRet + nInsertIndex + 1, pArrRet + nInsertIndex, (nArrCount - nInsertIndex) * sizeof(tagCmlDistAngle));
                
                (pArrRet + nInsertIndex)->dCml = arrCml[nProIndex];
                (pArrRet + nInsertIndex)->dDist = arrDist[nProIndex];
                (pArrRet + nInsertIndex)->dAngle = arrAngle[nProIndex];
                
                nArrCount++;
            }
        }
    }
    
    return pArrRet;
}

Point2d* HorizontalCurve::IntersectWithLine(double dAngle, double dX, double dY, uint32_t& nArrCount) const
{
    nArrCount = 0;
    Point2d* arrRet = nullptr;
    
    uint32_t nMaxCount = 8;
    //基准点
    Point2d posBase(dX, dY);
    for (uint32_t nIndex = 0; nIndex < m_nElementCount; nIndex++)
    {
        if (m_arrLineElement[nIndex]->dTotalLen < s_dLenPrecision)
            continue;
        
        Point2d arrPos[s_nMaxArrCount];
        uint32_t nCount = m_arrLineElement[nIndex]->IntersectWithLine(dAngle, dX, dY, arrPos);
        if (nCount > 0)
        {
            if (!arrRet)
                arrRet = new Point2d[nMaxCount];
            
            if (nArrCount + nCount > nMaxCount)
            {
                nMaxCount *= 2;
                Point2d* pNewArr = new Point2d[nMaxCount];
                memcpy((void*)pNewArr, (void*)arrRet, sizeof(Point2d) * nArrCount);
                delete [] arrRet;
                arrRet = pNewArr;
            }
            
            for (int nCrossIndex = 0; nCrossIndex < nCount; nCrossIndex++)
            {
                //插入排序
                uint32_t nInsertIndex = 0;
                for (nInsertIndex = 0; nInsertIndex < nArrCount; nInsertIndex++)
                    if (posBase.distanceTo(arrPos[nCrossIndex]) < posBase.distanceTo(arrRet[nInsertIndex]))
                        break;
                
                if (nArrCount > 0 && nInsertIndex < nArrCount)
                {
                    int nIndex = nArrCount;
                    while (nIndex > nInsertIndex) {
                        *(arrRet + nIndex) = *(arrRet + nIndex - 1);
                        nIndex--;
                    }
                }
                //memmove((void*)(arrRet + nInsertIndex + 1), (void*)(arrRet + nInsertIndex), (nArrCount - nInsertIndex) * sizeof(Point2d));
                
                *(arrRet + nInsertIndex) = *(arrPos + nCrossIndex);
                nArrCount++;
            }
        }
    }
    
    return arrRet;
}

tagJDInfo* HorizontalCurve::ExportOffsetJDInfo(double dOffsetDist, int& nJDCount) const
{
    bool bSuccess = true;
    
    tagJDInfo* arrJDInfo = new tagJDInfo[m_nJDCount];
    
    double dDir = dOffsetDist / abs(dOffsetDist);
    for (int nCurIndex = 0; nCurIndex + 2 < m_nJDCount; nCurIndex++)
    {
        const tagJDInfo* pCurItem = m_arrJD + nCurIndex;
        
        bool bType = (pCurItem + 1)->nJDType != JDType::ThreeUnit;
        bool bSpiralR = (((pCurItem + 1)->dEnterR != __DBL_MAX__ && (pCurItem + 1)->dL1 != 0.0)
                         || ((pCurItem + 1)->dExitR != __DBL_MAX__ && (pCurItem + 1)->dL2 != 0.0)
                         || abs((pCurItem + 1)->dL1 - (pCurItem + 1)->dL2) > s_dLenPrecision);
        if (bType || bSpiralR)
        {
            bSuccess = false;
            break;
        }
        
        Point2d pntJD1(pCurItem->dX, pCurItem->dY);
        Point2d pntJD2((pCurItem + 1)->dX, (pCurItem + 1)->dY);
        Point2d pntJD3((pCurItem + 2)->dX, (pCurItem + 2)->dY);
        
        double dAngle1 = BaseCalFun::CalAngleX(pntJD1, pntJD2);
        dAngle1 += MATH_PI_2 * dDir;
        Vector2d vecOffset1(cos(dAngle1), sin(dAngle1));
        
        double dAngle2 = BaseCalFun::CalAngleX(pntJD2, pntJD3);
        dAngle2 += MATH_PI_2 *dDir;
        Vector2d vecOffset2(cos(dAngle2), sin(dAngle2));
        
        Point2d pntNewJD1 = pntJD1 + vecOffset1 * abs(dOffsetDist);
        Point2d pntNewJD3 = pntJD3 + vecOffset2 * abs(dOffsetDist);
        Point2d pntNewJD2;
        if (!BaseCalFun::Intersect2Line(pntNewJD1, pntJD2 - pntJD1, pntNewJD3, pntJD3 - pntJD2, pntNewJD2))
        {
            bSuccess = false;
            break;
        }
        
        double dTurnAngle = BaseCalFun::CalTurnAngle(pntJD1, pntJD2, pntJD3);
        double dSpiralAngle = (pCurItem + 1)->dL1 / 2.0 / (pCurItem + 1)->dArcR1;
        double old_p = (pCurItem + 1)->dL1 * (pCurItem + 1)->dL1 / (pCurItem + 1)->dArcR1 / 24.0 - pow((pCurItem + 1)->dL1, 4.0) / pow((pCurItem + 1)->dArcR1, 3.0) / 2688.0;
        
        dOffsetDist *= (dTurnAngle > 0.0 ? -1.0 : 1.0);
        double dNewArcR = ((old_p + (pCurItem + 1)->dArcR1) + dOffsetDist) / (1.0 + dSpiralAngle * dSpiralAngle / 6.0 - pow(dSpiralAngle, 4.0) / 168.0);
        
        double dNewL = dSpiralAngle * dNewArcR * 2.0;
        
        if (nCurIndex == 0)
        {
            arrJDInfo[nCurIndex].dX = pntNewJD1.x;
            arrJDInfo[nCurIndex].dY = pntNewJD1.y;
        }
        if (nCurIndex + 2)
        {
            arrJDInfo[nCurIndex + 2].dX = pntNewJD3.x;
            arrJDInfo[nCurIndex + 2].dY = pntNewJD3.y;
        }
        arrJDInfo[nCurIndex + 1].dX = pntNewJD2.x;
        arrJDInfo[nCurIndex + 1].dY = pntNewJD2.y;
        arrJDInfo[nCurIndex + 1].dArcR1 = dNewArcR;
        arrJDInfo[nCurIndex + 1].dL1 = arrJDInfo[nCurIndex + 1].dL2 = dNewL;
        
    }
    
    if (!bSuccess)
    {
        delete [] arrJDInfo;
        nJDCount = 0;
        return nullptr;
    }
    
    return arrJDInfo;
}

