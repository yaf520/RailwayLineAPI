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
    if (dCml < 0.0 || dCml > GetLength())
        return nullptr;
    
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

//里程+投影计算坐标+切线角
bool HorizontalCurve::TrsCmlDistToNE(const double& dCml, const double& dDist, double& dX, double& dY, double& dAngle)
{
    double dCmlRound = BaseCalFun::Round(dCml);
    BaseLineElement* pLineElement = CmlBelongTo(dCmlRound);
    if (!pLineElement)
        return false;
    return pLineElement->TrsCmlDistToNE(dCmlRound, dDist, dX, dY, dAngle);
}

//坐标计算投影点里程+投影距离+切线角
bool HorizontalCurve::TrsNEToCmlDist(const double& dX, const double& dY, double& dCml, double& dDist, double& dAngle)
{
    DyArray<tagCmlDistAngle> arrPos = TrsNEToCmlDist(dX, dY);
    if (arrPos.GetCount() > 0)
    {
        dCml = arrPos.begin()->dCml;
        dDist = arrPos.begin()->dDist;
        dAngle = arrPos.begin()->dFwj;
        
        return true;
    }

    return false;
}

tagCmlDistAngle* HorizontalCurve::TrsNEToCmlDist(const double& dX, const double& dY, uint32_t& nArrCount)
{
    nArrCount = 0;
    tagCmlDistAngle* pArrRet = nullptr;
    
    uint32_t nMaxCount = 8;
    for (uint32_t nIndex = 0; nIndex < m_nElementCount; nIndex++)
    {
        double arrCml[s_nMaxArrCount] = {0.0};
        double arrDist[s_nMaxArrCount] = {0.0};
        double arrAngle[s_nMaxArrCount] = {0.0};
        
        uint32_t nCount = m_arrLineElement[nIndex]->TrsNEToCmlDist(dX, dY, arrCml, arrDist, arrAngle);
        if (nCount > 0)
        {
            if (!pArrRet)
                pArrRet = new tagCmlDistAngle[m_nElementCount];
            
            if (nArrCount >= nMaxCount)
            {
                nMaxCount += __max(nCount, nMaxCount);
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
                (pArrRet + nInsertIndex)->dDist = -arrDist[nProIndex];
                (pArrRet + nInsertIndex)->dFwj = MATH_PI_2 - arrAngle[nProIndex];
                BaseCalFun::KeepAngleIn2PI((pArrRet+ nInsertIndex)->dFwj);
                    
                nArrCount++;
            }
        }
    }
    
    return pArrRet;
}

DyArray<tagCmlDistAngle> HorizontalCurve::TrsNEToCmlDist(const double& dX, const double& dY)
{
    DyArray<tagCmlDistAngle> arrRet;
    
    for (uint32_t nIndex = 0; nIndex < m_nElementCount; nIndex++)
    {
        double arrCml[s_nMaxArrCount] = {0.0};
        double arrDist[s_nMaxArrCount] = {0.0};
        double arrAngle[s_nMaxArrCount] = {0.0};
        
        uint32_t nCount = m_arrLineElement[nIndex]->TrsNEToCmlDist(dX, dY, arrCml, arrDist, arrAngle);
        if (nCount > 0)
        {
            for (int nProIndex = 0; nProIndex < nCount; nProIndex++)
            {
                //插入排序
                uint32_t nInsertIndex = 0;
                for (nInsertIndex = 0; nInsertIndex < arrRet.GetCount(); nInsertIndex++)
                    if (abs(arrDist[nProIndex]) < abs(arrRet[nInsertIndex].dDist))
                        break;
                
                tagCmlDistAngle item;
                item.dCml = arrCml[nProIndex];
                item.dDist = -arrDist[nProIndex];
                item.dFwj = MATH_PI_2 - arrAngle[nProIndex];
                BaseCalFun::KeepAngleIn2PI(item.dFwj);

                arrRet.InsertAt(nInsertIndex, item);
            }
        }
    }
    
    return arrRet;
}

Point2d* HorizontalCurve::IntersectWithLine(const double& dAngle, const double& dX, const double& dY, uint32_t& nArrCount)
{
    nArrCount = 0;
    Point2d* arrRet = nullptr;
    
    uint32_t nMaxCount = 8;
    //基准点
    Point2d posBase(dX, dY);
    for (uint32_t nIndex = 0; nIndex < m_nElementCount; nIndex++)
    {
        Point2d arrPos[s_nMaxArrCount];
        uint32_t nCount = m_arrLineElement[nIndex]->IntersectWithLine(dAngle, dX, dY, arrPos);
        if (nCount > 0)
        {
            if (!arrRet)
                arrRet = new Point2d[m_nElementCount];
            
            if (nArrCount >= nMaxCount)
            {
                nMaxCount += __max(nCount, nMaxCount);
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

DyArray<Point2d> HorizontalCurve::IntersectWithLine(const double& dAngle, const double& dX, const double& dY)
{
    DyArray<Point2d> arrRet;
    
    //基准点
    Point2d posBase(dX, dY);
    for (uint32_t nIndex = 0; nIndex < m_nElementCount; nIndex++)
    {
        Point2d arrPos[s_nMaxArrCount];
        uint32_t nCount = m_arrLineElement[nIndex]->IntersectWithLine(dAngle, dX, dY, arrPos);
        if (nCount > 0)
        {
            for (int nCrossIndex = 0; nCrossIndex < nCount; nCrossIndex++)
            {
                //插入排序
                uint32_t nInsertIndex = 0;
                for (nInsertIndex = 0; nInsertIndex < arrRet.GetCount(); nInsertIndex++)
                    if (posBase.distanceTo(arrPos[nCrossIndex]) < posBase.distanceTo(arrRet[nInsertIndex]))
                        break;

                arrRet.InsertAt(nInsertIndex, arrPos[nCrossIndex]);
            }
        }
    }
    
    return arrRet;
}

