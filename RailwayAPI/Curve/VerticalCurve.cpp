//
//  VerticalObject.cpp
//  RailwayAPI
//
//  Created by 易傲飞 on 2023/11/6.
//

#include "VerticalCurve.hpp"

VerticalCurve::VerticalCurve(MileConvert* pMileconvert)
    : LineElementManager(CurveType::VerticalCurve)
{
    m_pMileConvert = pMileconvert;
}

void VerticalCurve::SetSlopeData(const tagSlopeInfo* pSlopeInfo, uint32_t iSlopeCount)
{
    if (m_pMileConvert == nullptr)
        return;
    
    tagJDInfo* pJDArr = new tagJDInfo[iSlopeCount];
    int iReliability = 0;
    char strErr[64] = {0};
    for (uint32_t i = 0; i < iSlopeCount; i++)
    {
        auto dataItem = *(pSlopeInfo + i);
        double dCml = m_pMileConvert->GetSumLengthOfMile(dataItem.strNo, dataItem.dCml, iReliability, strErr);
        pJDArr[i].dX = dCml;
        pJDArr[i].dY = dataItem.dHeight;
        pJDArr[i].dArcR1 = dataItem.dArcR;
        pJDArr[i].dL2 = 0.0;
        pJDArr[i].dL1 = 0.0;
    }
    //this->SetJDData(pJDArr, iSlopeCount);
    
    delete [] pJDArr;
}

BaseLineElement* VerticalCurve::CmlBelongTo(double dCml)
{
    if (dCml > GetLength())
        return nullptr;
    
    //通过里程查询在哪一线元范围内(二分查找法)
    int left = 0;
    int right = m_nElementCount - 1;
    while (left <= right) {
        int mid = (left + right) / 2;
        if (m_arrLineElement[mid]->m_posStart.x > dCml)
            right = mid - 1;
        else
            left = mid + 1;
    }
    if (right < 0 || right >= m_nElementCount)
        return nullptr;
    
    return m_arrLineElement[right];
}

bool VerticalCurve::TrsCmlToHeight(const double& dCml, double& dHeight, double& dAngle)
{
    double dCmlRound = BaseCalFun::Round(dCml);
    BaseLineElement* pLineElement = CmlBelongTo(dCmlRound);
    if (!pLineElement)
        return false;
    return pLineElement->TrsCmlToHeight(dCmlRound, dHeight, dAngle);
}
