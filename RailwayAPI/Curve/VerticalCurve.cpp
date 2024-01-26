//
//  VerticalObject.cpp
//  RailwayAPI
//
//  Created by 易傲飞 on 2023/11/6.
//

#include "VerticalCurve.hpp"

void VerticalCurve::SetSlopeData(const tagSlopeInfo* pSlopeInfo, uint32_t iSlopeCount)
{
    if (m_pHorizontalCurve == nullptr)
        return;
    
    tagJDInfo* pJDArr = new tagJDInfo[iSlopeCount];
    int iReliability = 0;
    char strErr[64] = {0};
    for (uint32_t i = 0; i < iSlopeCount; i++)
    {
        auto dataItem = *(pSlopeInfo + i);
        double dCml = m_pHorizontalCurve->GetSumLengthOfMile(dataItem.strNo, dataItem.dCml, iReliability, strErr);
        pJDArr[i].dX = dCml;
        pJDArr[i].dY = dataItem.dHeight;
        pJDArr[i].dArcR = dataItem.dArcR;
        pJDArr[i].dBL = 0.0;
        pJDArr[i].dFL = 0.0;
    }
    this->SetJDData(pJDArr, iSlopeCount);
    
    delete [] pJDArr;
}

BaseLineElement* VerticalCurve::CmlBelongTo(double dCml)
{
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
    BaseLineElement* pLineElement = CmlBelongTo(dCml);
    if (!pLineElement) return false;
    return pLineElement->TrsCmlToHeight(dCml, dHeight, dAngle);
}
