//
//  HorizontalLogic.cpp
//
//  Created by 易傲飞 on 2023/10/27.
//

#include "LineElementManager.hpp"
#include "BaseCalFun.hpp"

LineElementManager::LineElementManager(CurveType eCurveType)
{
    m_arrLineElement = nullptr;
    m_nElementCount = 0;
    m_eCurveType = eCurveType;
    m_arrJD = nullptr;
    m_nJDCount = 0;
    m_arrCurveElement = nullptr;
    m_nCurveElementCount = 0;
}

LineElementManager::~LineElementManager()
{
    this->ResetData();
}

void LineElementManager::ResetData()
{
    if (m_arrLineElement)
    {
        for (uint32_t i = 0; i < m_nElementCount; i++)
            if (m_arrLineElement[i])
                delete m_arrLineElement[i];
        
        delete [] m_arrLineElement;
        m_arrLineElement = nullptr;
        m_nElementCount = 0;
    }
    
    if (m_arrJD)
    {
        delete m_arrJD;
        m_arrJD = nullptr;
        m_nJDCount = 0;
    }
    
    if (m_arrCurveElement)
    {
        delete [] m_arrCurveElement;
        m_arrCurveElement = nullptr;
        m_nCurveElementCount = 0;
    }
}

Point2d LineElementManager::CalUnitStartPos(uint32_t nIndex, const Vector2d& vecWhole)
{
    //交点坐标
    Point2d posJD1(((m_arrJD + nIndex)->nJDType != JDType::ThreeUnitBack ? Point2d((m_arrJD + nIndex)->dX, (m_arrJD + nIndex)->dY) : Point2d((m_arrJD + nIndex)->dX_End, (m_arrJD + nIndex)->dY_End)));
    Point2d posJD2((m_arrJD + nIndex + 1)->dX, (m_arrJD + nIndex + 1)->dY);
    Point2d posJD3((m_arrJD + nIndex + 2)->dX, (m_arrJD + nIndex + 2)->dY);
    
    Vector2d vecJD1_JD2 = posJD2 - posJD1;
    Vector2d vecJD2_JD3 = posJD3 - posJD2;
    
    double a = vecJD1_JD2.y;
    double b = -vecJD1_JD2.x;
    double c = vecJD1_JD2.y * posJD1.x - vecJD1_JD2.x * posJD1.y;
    double d = vecJD2_JD3.y;
    double e = -vecJD2_JD3.x;
    double f = vecJD2_JD3.x * (vecWhole.y - posJD2.y) - vecJD2_JD3.y * (vecWhole.x - posJD2.x);
    //世界坐标
    return Point2d((e * c - b * f) / (a * e - b * d), (d * c - a * f) / (b * d - a * e));
}

void LineElementManager::CalculateLineElement(uint32_t nCurIndex, BaseLineElement** arrLineElement, uint8_t& nCurveElementCount)
{
    //交点坐标
    Point2d posJD1 = ((m_arrJD + nCurIndex)->nJDType != JDType::ThreeUnitBack ? Point2d((m_arrJD + nCurIndex)->dX, (m_arrJD + nCurIndex)->dY) : Point2d((m_arrJD + nCurIndex)->dX_End, (m_arrJD + nCurIndex)->dY_End));
    Point2d posJD2((m_arrJD + nCurIndex + 1)->dX, (m_arrJD + nCurIndex + 1)->dY);
    Point2d posJD3((m_arrJD + nCurIndex + 2)->dX, (m_arrJD + nCurIndex + 2)->dY);
    Point2d posJDRef((m_arrJD + nCurIndex + 1)->dX_End, (m_arrJD + nCurIndex + 1)->dY_End);
    //交点类型
    JDType nJDType = (m_arrJD + nCurIndex + 1)->nJDType;
    //起始半径
    double dEnterR = (m_arrJD + nCurIndex + 1)->dEnterR;
    double dExitR = (m_arrJD + nCurIndex + 1)->dExitR;
    //单元起点(相对)
    Point2d posCurveStart;
    //置零
    nCurveElementCount = 0;
    
    switch (nJDType) {
        case JDType::ThreeUnit:
        case JDType::ThreeUnitBack:
        {
            //圆曲线半径
            double dArcR = (m_arrJD + nCurIndex + 1)->dArcR1;
            //转向角
            double dTurnAngle = (nJDType == JDType::ThreeUnit ? BaseCalFun::CalTurnAngle(posJD1, posJD2, posJD3) : BaseCalFun::CalTurnAngle(posJD1, posJD2, posJDRef, posJD3));
            //转向方向
            bool bTurnLeft = ((nJDType == JDType::ThreeUnit && dTurnAngle > 0.0) || (nJDType == JDType::ThreeUnitBack && dTurnAngle < 0.0));
            if (nJDType == JDType::ThreeUnitBack)
                dTurnAngle = MATH_PI * 2.0 - abs(dTurnAngle);
            
            //缓和曲线长度
            double dL1 = (m_arrJD + nCurIndex + 1)->dL1;
            double dL2 = (m_arrJD + nCurIndex + 1)->dL2;
            if (dL1 < 0.0)
            {
                double dC1 = pow((m_arrJD + nCurIndex + 1)->dA1, 2);
                dL1 = (dEnterR == __DBL_MAX__ ? dC1 / dArcR : dC1 * abs(dEnterR - dArcR) / dEnterR / dArcR);
            }
            
            if (dL2 < 0.0)
            {
                double dC2 = pow((m_arrJD + nCurIndex + 1)->dA2, 2);
                dL2 = (dExitR == __DBL_MAX__ ? dC2 / dArcR : dC2 * abs(dExitR - dArcR) / dExitR / dArcR);
            }
            
            //缓和曲线角
            double dEnterCurveAngle = (abs(dL1) < s_dCalPrecision ? 0.0 : ((dEnterR == __DBL_MAX__) ? dL1 / (dArcR * 2.0) : dL1 * (dEnterR + dArcR) / 2.0 / dEnterR / dArcR));
            double dExitCurveAngle = (abs(dL2) < s_dCalPrecision ? 0.0 : ((dExitR == __DBL_MAX__) ? dL2 / (dArcR * 2.0) : dL2 * (dExitR + dArcR) / 2.0 / dExitR / dArcR));
            //圆弧角度
            double dArcAngle = abs(dTurnAngle) - dEnterCurveAngle - dExitCurveAngle;
            //起点角度
            double dTanAngleStart = BaseCalFun::CalAngleX(posJD1, posJD2);
            //向量定义
            Vector2d vecWhole;
            
            if (nJDType == JDType::ThreeUnitBack)
                posCurveStart = posJD2;
            
            if (abs(dL1) > s_dCalPrecision)
            {
                //前缓和曲线
                SpiralLineElement* pSpiralElement = nullptr;
                if (arrLineElement[nCurveElementCount] && arrLineElement[nCurveElementCount]->eElementType == ElementType::SpiralCurve)
                    pSpiralElement = (SpiralLineElement*)arrLineElement[nCurveElementCount++];
                else
                {
                    pSpiralElement = new SpiralLineElement();
                    arrLineElement[nCurveElementCount++] = pSpiralElement;
                }
                
                pSpiralElement->pntStart = posCurveStart;
                pSpiralElement->dStartTanAngle = dTanAngleStart;
                pSpiralElement->dTotalLen = dL1;
                pSpiralElement->bTurnLeft = bTurnLeft;
                pSpiralElement->dEnterR =  dEnterR;
                pSpiralElement->dExitR = dArcR;
                pSpiralElement->InitData();
                
                vecWhole += (pSpiralElement->pntEnd - pSpiralElement->pntStart);
                
                //变换起点
                posCurveStart = pSpiralElement->pntEnd;
                //变换角度
                dTanAngleStart = pSpiralElement->dEndTanAngle;
            }
            
            //圆曲线
            do
            {
                ArcElement* pArcElement = nullptr;
                if (arrLineElement[nCurveElementCount] && arrLineElement[nCurveElementCount]->eElementType == ElementType::Arc)
                    pArcElement = (ArcElement*)arrLineElement[nCurveElementCount++];
                else
                {
                    pArcElement = new ArcElement();
                    arrLineElement[nCurveElementCount++] = pArcElement;
                }
                
                pArcElement->pntStart = posCurveStart;
                pArcElement->dStartTanAngle = dTanAngleStart;
                pArcElement->dArcR = dArcR;
                pArcElement->bTurnLeft = bTurnLeft;
                pArcElement->dTotalLen = dArcR * abs(dArcAngle);
                pArcElement->InitData();

                vecWhole += (pArcElement->pntEnd - pArcElement->pntStart);
                
                //变换起点
                posCurveStart = pArcElement->pntEnd;
                //变换角度
                dTanAngleStart = pArcElement->dEndTanAngle;
            } while(0);
            
            if (abs(dL2) > s_dCalPrecision)
            {
                //后缓和曲线
                SpiralLineElement* pSpiralElement = nullptr;
                if (arrLineElement[nCurveElementCount] && arrLineElement[nCurveElementCount]->eElementType == ElementType::SpiralCurve)
                    pSpiralElement = (SpiralLineElement*)arrLineElement[nCurveElementCount++];
                else
                {
                    pSpiralElement = new SpiralLineElement();
                    arrLineElement[nCurveElementCount++] = pSpiralElement;
                }
                
                pSpiralElement->pntStart = posCurveStart;
                pSpiralElement->dStartTanAngle = dTanAngleStart;
                pSpiralElement->dTotalLen = dL2;
                pSpiralElement->bTurnLeft = bTurnLeft;
                pSpiralElement->dEnterR = dArcR;
                pSpiralElement->dExitR = dExitR;
                pSpiralElement->InitData();

                vecWhole += (pSpiralElement->pntEnd - pSpiralElement->pntStart);
            }
            
            //计算曲线起点世界坐标
            posCurveStart = (nJDType == JDType::ThreeUnit ? CalUnitStartPos(nCurIndex, vecWhole) : posJD2);
            
            break;
        }
        case JDType::FiveUnit:
        {
            //起点角度
            double dTanAngleStart = BaseCalFun::CalAngleX(posJD1, posJD2);
            //转向角
            double dTurnAngle = BaseCalFun::CalTurnAngle(posJD1, posJD2, posJD3);
            //转向方向
            bool bTurnLeft = (dTurnAngle > 0.0);
            //弧长控制参数
            double dID = (m_arrJD + nCurIndex + 1)->dID;
            //圆弧半径
            double dArcR1 = (m_arrJD + nCurIndex + 1)->dArcR1;
            double dArcR2 = (m_arrJD + nCurIndex + 1)->dArcR2;
            //缓和曲线长度
            double dL1 = (m_arrJD + nCurIndex + 1)->dL1;
            double dL2 = (m_arrJD + nCurIndex + 1)->dL2;
            double dL3 = (m_arrJD + nCurIndex + 1)->dL3;
            if (dL1 < 0.0)
            {
                double dC1 = pow((m_arrJD + nCurIndex + 1)->dA1, 2);
                dL1 = (dEnterR == __DBL_MAX__ ? dC1 / dArcR1 : dC1 * abs(dEnterR - dArcR1) / dEnterR / dArcR1);
            }
            if (dL2 < 0.0)
            {
                double dC2 = pow((m_arrJD + nCurIndex + 1)->dA2, 2);
                dL2 = dC2 * abs(dArcR2 - dArcR1) / dArcR2 / dArcR1;
            }
            if (dL3 < 0.0)
            {
                double dC3 = pow((m_arrJD + nCurIndex + 1)->dA3, 2);
                dL3 = (dExitR == __DBL_MAX__ ? dC3 / dArcR2 : dC3 * abs(dExitR - dArcR2) / dExitR / dArcR2);
            }
            //缓和曲线角度
            double dCurveAngle1 = (abs(dL1) < s_dCalPrecision ? 0.0 : (dEnterR == __DBL_MAX__ ? dL1 / dArcR1 / 2.0 : dL1 * (dEnterR + dArcR1) / 2.0 / dEnterR / dArcR1));
            double dCurveAngle2 = dL2 * (dArcR1 + dArcR2) / 2.0 / dArcR1 / dArcR2;
            double dCurveAngle3 = (abs(dL3) < s_dCalPrecision ? 0.0 : (dExitR == __DBL_MAX__ ? dL3 / dArcR2 / 2.0 : dL3 * (dExitR + dArcR2) / 2.0 / dExitR / dArcR2));
            //圆曲线角度之和
            double dArcAngleTotal = abs(dTurnAngle) - dCurveAngle1 - dCurveAngle2 - dCurveAngle3;
            //圆弧角度
            double dArcAngle1 = (dID == 0.0 ? dArcR2 * dArcAngleTotal / (dArcR1 + dArcR2) : dID / dArcR1);
            double dArcAngle2 = dArcAngleTotal - dArcAngle1;
            //变量定义
            Vector2d vecWhole;
            
            if (dL1 > 0.0)
            {
                //第一段缓和曲线
                SpiralLineElement* pSpiralElement = nullptr;
                if (arrLineElement[nCurveElementCount] && arrLineElement[nCurveElementCount]->eElementType == ElementType::SpiralCurve)
                    pSpiralElement = (SpiralLineElement*)arrLineElement[nCurveElementCount++];
                else
                {
                    pSpiralElement = new SpiralLineElement();
                    arrLineElement[nCurveElementCount++] = pSpiralElement;
                }
                
                pSpiralElement->pntStart = posCurveStart;
                pSpiralElement->dStartTanAngle = dTanAngleStart;
                pSpiralElement->dTotalLen = dL1;
                pSpiralElement->bTurnLeft = bTurnLeft;
                pSpiralElement->dEnterR = dEnterR;
                pSpiralElement->dExitR = dArcR1;
                pSpiralElement->InitData();
                
                vecWhole += (pSpiralElement->pntEnd - pSpiralElement->pntStart);
                
                //变换起点
                posCurveStart = pSpiralElement->pntEnd;
                //变换角度
                dTanAngleStart = pSpiralElement->dEndTanAngle;
            }
            
            do {
                //第一段圆曲线
                ArcElement* pArcElement = nullptr;
                if (arrLineElement[nCurveElementCount] && arrLineElement[nCurveElementCount]->eElementType == ElementType::Arc)
                    pArcElement = (ArcElement*)arrLineElement[nCurveElementCount++];
                else
                {
                    pArcElement = new ArcElement();
                    arrLineElement[nCurveElementCount++] = pArcElement;
                }
                
                pArcElement->pntStart = posCurveStart;
                pArcElement->dStartTanAngle = dTanAngleStart;
                pArcElement->dArcR = dArcR1;
                pArcElement->bTurnLeft = bTurnLeft;
                pArcElement->dTotalLen = dArcR1 * abs(dArcAngle1);
                pArcElement->InitData();
                
                vecWhole += (pArcElement->pntEnd - pArcElement->pntStart);
                
                //变换起点
                posCurveStart = pArcElement->pntEnd;
                //变换角度
                dTanAngleStart = pArcElement->dEndTanAngle;
            } while (false);
            
            do {
                //第二段缓和曲线
                SpiralLineElement* pSpiralElement = nullptr;
                if (arrLineElement[nCurveElementCount] && arrLineElement[nCurveElementCount]->eElementType == ElementType::SpiralCurve)
                    pSpiralElement = (SpiralLineElement*)arrLineElement[nCurveElementCount++];
                else
                {
                    pSpiralElement = new SpiralLineElement();
                    arrLineElement[nCurveElementCount++] = pSpiralElement;
                }
                
                pSpiralElement->pntStart = posCurveStart;
                pSpiralElement->dStartTanAngle = dTanAngleStart;
                pSpiralElement->dTotalLen = dL2;
                pSpiralElement->bTurnLeft = bTurnLeft;
                pSpiralElement->dEnterR =  dArcR1;
                pSpiralElement->dExitR = dArcR2;
                pSpiralElement->InitData();
                
                vecWhole += (pSpiralElement->pntEnd - pSpiralElement->pntStart);
                
                //变换起点
                posCurveStart = pSpiralElement->pntEnd;
                //变换角度
                dTanAngleStart = pSpiralElement->dEndTanAngle;
            } while (false);
            
            do {
                //第二段圆曲线
                ArcElement* pArcElement = nullptr;
                if (arrLineElement[nCurveElementCount] && arrLineElement[nCurveElementCount]->eElementType == ElementType::Arc)
                    pArcElement = (ArcElement*)arrLineElement[nCurveElementCount++];
                else
                {
                    pArcElement = new ArcElement();
                    arrLineElement[nCurveElementCount++] = pArcElement;
                }
                
                pArcElement->pntStart = posCurveStart;
                pArcElement->dStartTanAngle = dTanAngleStart;
                pArcElement->dArcR = dArcR2;
                pArcElement->bTurnLeft = bTurnLeft;
                pArcElement->dTotalLen = dArcR2 * abs(dArcAngle2);
                pArcElement->InitData();

                vecWhole += (pArcElement->pntEnd - pArcElement->pntStart);
                
                //变换起点
                posCurveStart = pArcElement->pntEnd;
                //变换角度
                dTanAngleStart = pArcElement->dEndTanAngle;
            } while (false);
            
            if (dL3 > 0.0)
            {
                //第三段缓和曲线
                SpiralLineElement* pSpiralElement = nullptr;
                if (arrLineElement[nCurveElementCount] && arrLineElement[nCurveElementCount]->eElementType == ElementType::SpiralCurve)
                    pSpiralElement = (SpiralLineElement*)arrLineElement[nCurveElementCount++];
                else
                {
                    pSpiralElement = new SpiralLineElement();
                    arrLineElement[nCurveElementCount++] = pSpiralElement;
                }
                
                pSpiralElement->pntStart = posCurveStart;
                pSpiralElement->dStartTanAngle = dTanAngleStart;
                pSpiralElement->dTotalLen = dL3;
                pSpiralElement->bTurnLeft = bTurnLeft;
                pSpiralElement->dEnterR = dArcR2;
                pSpiralElement->dExitR = dExitR;
                pSpiralElement->InitData();

                vecWhole += (pSpiralElement->pntEnd - pSpiralElement->pntStart);
            }
            
            //计算曲线起点世界坐标
            posCurveStart = CalUnitStartPos(nCurIndex, vecWhole);
            
            break;
        }
        case JDType::FiveUnitBack:
        {
            break;
        }
            
        default:
            break;
    }
    
    //调整为世界坐标系
    if (nJDType == JDType::ThreeUnit || nJDType == JDType::FiveUnit)
        for (uint8_t i = 0; i < nCurveElementCount; i++)
            arrLineElement[i]->AdjustData(posCurveStart);
}

void LineElementManager::JointLineElement(uint32_t nCurIndex, BaseLineElement** arrLineElement, uint8_t& nCurveElementCount, double& dCurrentCml)
{
    //交点坐标
    Point2d posJD1 = ((m_arrJD + nCurIndex)->nJDType != JDType::ThreeUnitBack ? Point2d((m_arrJD + nCurIndex)->dX, (m_arrJD + nCurIndex)->dY) : Point2d((m_arrJD + nCurIndex)->dX_End, (m_arrJD + nCurIndex)->dY_End));
    Point2d posJD2((m_arrJD + nCurIndex + 1)->dX, (m_arrJD + nCurIndex + 1)->dY);
    Point2d posJD3((m_arrJD + nCurIndex + 2)->dX, (m_arrJD + nCurIndex + 2)->dY);
    Point2d posJDRef((m_arrJD + nCurIndex + 1)->dX_End, (m_arrJD + nCurIndex + 1)->dY_End);
    //交点类型
    JDType nJDType = (m_arrJD + nCurIndex + 1)->nJDType;
    
    //单元起点
    Point2d posCurveStart = (nCurveElementCount > 0 ? arrLineElement[0]->pntStart : posJD2);
    
    //拼接线元
    if (m_nElementCount > 0 && m_arrLineElement[m_nElementCount - 1]->eElementType == ElementType::Line)
    {
        BaseLineElement* pPreLineElement = m_arrLineElement[m_nElementCount - 1];
        //调整前一段直线
        pPreLineElement->pntEnd = posCurveStart;
        pPreLineElement->dTotalLen = pPreLineElement->pntStart.distanceTo(posCurveStart);
        
        dCurrentCml += pPreLineElement->dTotalLen;
    }
    else
    {
        //当前阶段起点
        Point2d posStart = (m_nElementCount == 0 ? posJD1 : m_arrLineElement[m_nElementCount - 1]->pntEnd);
        
        //入直线
        StraightLineElement* pStraightLineElement = new StraightLineElement();
        pStraightLineElement->pntStart = posStart;
        pStraightLineElement->pntEnd = posCurveStart;
        pStraightLineElement->dStartTanAngle = pStraightLineElement->dEndTanAngle = BaseCalFun::CalAngleX(posJD1, posJD2);
        pStraightLineElement->dTotalLen = pStraightLineElement->pntStart.distanceTo(pStraightLineElement->pntEnd);
        pStraightLineElement->dStartCml = dCurrentCml;
        pStraightLineElement->nIndex = m_nElementCount;
        dCurrentCml += pStraightLineElement->dTotalLen;
        m_arrLineElement[m_nElementCount++] = pStraightLineElement;
    }
    
    //记录索引
    (m_arrJD + nCurIndex + 1)->nIndexCount = nCurveElementCount;
    for (uint8_t j = 0; j < nCurveElementCount; j++)
    {
        arrLineElement[j]->dStartCml = dCurrentCml;
        arrLineElement[j]->nIndex = (m_arrJD + nCurIndex + 1)->arrUnitsIndex[j] = m_nElementCount;
        
        m_arrLineElement[m_nElementCount++] = arrLineElement[j];
        
        dCurrentCml += arrLineElement[j]->dTotalLen;
    }
    
    if ((m_arrJD + nCurIndex + 1)->dExitR == __DBL_MAX__ || (m_arrJD + nCurIndex + 2)->nJDType == JDType::End)
    {
        BaseLineElement* pPreLineElement = m_arrLineElement[m_nElementCount - 1];
        
        //出直线
        StraightLineElement* pStraightLineElement = new StraightLineElement();
        pStraightLineElement->pntStart = pPreLineElement->pntEnd;
        pStraightLineElement->pntEnd = posJD3;
        pStraightLineElement->dStartTanAngle = pStraightLineElement->dEndTanAngle
            = BaseCalFun::CalAngleX((nJDType == JDType::ThreeUnitBack ? posJDRef : posJD2), posJD3);
        pStraightLineElement->dStartCml = dCurrentCml;
        pStraightLineElement->nIndex = m_nElementCount;
        pStraightLineElement->dTotalLen = pStraightLineElement->pntStart.distanceTo(pStraightLineElement->pntEnd);
        m_arrLineElement[m_nElementCount++] = pStraightLineElement;
    }
}

void LineElementManager::CurveElement(uint32_t nCurIndex, BaseLineElement** arrLineElement, uint8_t nCurveElementCount)
{
    //交点坐标
    Point2d posJD1 = ((m_arrJD + nCurIndex)->nJDType != JDType::ThreeUnitBack ? Point2d((m_arrJD + nCurIndex)->dX, (m_arrJD + nCurIndex)->dY) : Point2d((m_arrJD + nCurIndex)->dX_End, (m_arrJD + nCurIndex)->dY_End));
    Point2d posJD2((m_arrJD + nCurIndex + 1)->dX, (m_arrJD + nCurIndex + 1)->dY);
    Point2d posJD3((m_arrJD + nCurIndex + 2)->dX, (m_arrJD + nCurIndex + 2)->dY);
    Point2d posJDRef((m_arrJD + nCurIndex + 1)->dX_End, (m_arrJD + nCurIndex + 1)->dY_End);
    //交点类型
    JDType nJDType = (m_arrJD + nCurIndex + 1)->nJDType;
    
    if (nCurIndex == 0)
    {
        //起点
        m_arrCurveElement[nCurIndex].nJDIndex = nCurIndex;
        m_arrCurveElement[nCurIndex].dN_Y = posJD1.y;
        m_arrCurveElement[nCurIndex].dE_X = posJD1.x;
    }
    else if (nCurIndex + 2 == m_nJDCount - 1)
    {
        //终点
        m_arrCurveElement[nCurIndex + 2].nJDIndex = nCurIndex + 2;
        m_arrCurveElement[nCurIndex + 2].dN_Y = posJD3.y;
        m_arrCurveElement[nCurIndex + 2].dE_X = posJD3.x;
    }
    
    ///这里暂时只处理三单元曲线
    tagCurveElementItem& item = m_arrCurveElement[nCurIndex + 1];
    item.nJDIndex = nCurIndex + 1;
    item.dN_Y = posJD2.y;
    item.dE_X = posJD2.x;
    item.dTurnAngle = (nJDType == JDType::ThreeUnit || nJDType == JDType::FiveUnitBack ? BaseCalFun::CalTurnAngle(posJD1, posJD2, posJD3) : BaseCalFun::CalTurnAngle(posJD1, posJD2, posJDRef, posJD3));
    if (nJDType == JDType::ThreeUnitBack || nJDType == JDType::FiveUnitBack)
        item.dTurnAngle = MATH_PI * 2.0 - abs(item.dTurnAngle);
    
    if (nCurveElementCount > 0)
    {
        //切线长
        item.dT1 = arrLineElement[0]->pntStart.distanceTo(posJD2);
        item.dT2 = arrLineElement[nCurveElementCount - 1]->pntEnd.distanceTo(posJD2);
        
        for (int i = 0; i < nCurveElementCount; i++)
        {
            //总计长度
            item.dCurveLen += arrLineElement[i]->dTotalLen;
            
            if (arrLineElement[i]->eElementType == ElementType::Arc)
            {
                //圆曲线
                ArcElement* pArcElement = (ArcElement*)arrLineElement[i];
                //半径
                item.dArcR = pArcElement->dArcR;
                
                if (i > 0)
                {
                    item.dL1 = arrLineElement[i - 1]->dTotalLen;
                    item.dA1 = pow(((SpiralLineElement*)arrLineElement[i - 1])->dC, 0.5);
                }
                if (i + 1 < nCurveElementCount)
                {
                    item.dL2 = arrLineElement[i + 1]->dTotalLen;
                    item.dA2 = pow(((SpiralLineElement*)arrLineElement[i + 1])->dC, 0.5);
                }
                
                //属性点里程
                item.dHYCml = pArcElement->dStartCml;
                item.dYHCml = pArcElement->dStartCml + pArcElement->dTotalLen;
                item.dQZCml = pArcElement->dStartCml + pArcElement->dTotalLen / 2.0;
                item.dZHCml = (i > 0 ? arrLineElement[i - 1]->dStartCml : item.dHYCml);
                item.dHZCml = (i + 1 < nCurveElementCount ? arrLineElement[i + 1]->dStartCml + arrLineElement[i + 1]->dTotalLen : item.dYHCml);
                
                //外矢距
                Point2d posQZ;
                double dAngle = 0.0;
                pArcElement->TrsCmlDistToNE(item.dQZCml, 0.0, posQZ.x, posQZ.y, dAngle);
                item.dE = posQZ.distanceTo(posJD2);
            }
        }
    }
}

void LineElementManager::SetJDData(const tagJDInfo* pJDInfo, uint32_t nCount)
{
    if (!pJDInfo || nCount < 2) return;
    
    //重置数据
    ResetData();
    
    //最大线元数量
    uint32_t nMaxCount = nCount * 6 - 11;
    m_arrLineElement = new BaseLineElement*[nMaxCount];
    
    //保存交点
    m_nJDCount = nCount;
    m_arrJD = new tagJDInfo[m_nJDCount];
    memcpy(m_arrJD, pJDInfo, sizeof(tagJDInfo) * m_nJDCount);
    
    ///曲线要素数组
    m_nCurveElementCount = m_nJDCount;
    m_arrCurveElement = new tagCurveElementItem[m_nCurveElementCount];
    
    if (nCount == 2)
    {
        m_arrLineElement = new BaseLineElement*[1];
        
        //两个交点
        Point2d posJD1(pJDInfo->dX, pJDInfo->dY);
        Point2d posJD2((pJDInfo + 1)->dX, (pJDInfo + 1)->dY);
        double dAngle_JD1_JD2 = BaseCalFun::CalAngleX(posJD1, posJD2);
        
        BaseLineElement* pLineElement = new StraightLineElement;
        pLineElement->pntStart = posJD1;
        pLineElement->pntEnd = posJD2;
        pLineElement->dStartTanAngle = pLineElement->dEndTanAngle = dAngle_JD1_JD2;
        pLineElement->dStartCml = 0.0;
        pLineElement->dTotalLen = posJD1.distanceTo(posJD2);
        pLineElement->nIndex = m_nElementCount;
        m_arrLineElement[m_nElementCount++] = pLineElement;
        
        return;
    }
    
    //变量定义
    double dCurrentCml = 0.0;
    for (uint32_t nCurIndex = 0; nCurIndex + 2 < nCount; nCurIndex++)
    {
        //变量定义
        BaseLineElement* arrLineElement[5];
        memset(arrLineElement, 0, sizeof(BaseLineElement*) * 5);
        uint8_t nCurveElementCount = 0;
        
        //计算线元
        CalculateLineElement(nCurIndex, arrLineElement, nCurveElementCount);
        
        //拼接线元
        JointLineElement(nCurIndex, arrLineElement, nCurveElementCount, dCurrentCml);
        
        //计算曲线要素
        CurveElement(nCurIndex, arrLineElement, nCurveElementCount);
    }
    
    //调整线元数组大小
    if (m_nElementCount < nMaxCount)
    {
        BaseLineElement** arrLineElement = new BaseLineElement*[m_nElementCount];
        memcpy(arrLineElement, m_arrLineElement, sizeof(BaseLineElement*) * m_nElementCount);
        delete[] m_arrLineElement;
        m_arrLineElement = arrLineElement;
    }
}

void LineElementManager::UpdateJD(int nIndex, const Vector2d& vecOffset)
{
    if (nIndex < 0 || nIndex >= m_nJDCount || vecOffset.isZeroVec())
        return;
    
    //#define RESET_ALL
    
#ifdef RESET_ALL
    
    //复制数据
    tagJDInfo* pJDCopy = new tagJDInfo[m_nJDCount];
    memcpy(pJDCopy, m_arrJD, sizeof(tagJDInfo) * m_nJDCount);
    //修改相应数据
    pJDCopy[nIndex].dX += vecOffset.x;
    pJDCopy[nIndex].dY += vecOffset.y;
    //重置数据
    SetJDData(pJDCopy, m_nJDCount);
    
    delete [] pJDCopy;
    
#else
    
    //修改数据
    m_arrJD[nIndex].dX += vecOffset.x;
    m_arrJD[nIndex].dY += vecOffset.y;
    //影响交点的索引范围
    int nStartJDIndex = __max(0, nIndex - 2);
    int nEndJDIndex = __min(m_nJDCount - 1, nIndex + 2);
    //单元线元
    BaseLineElement* arrLineElement[5];
    uint8_t nUnitElementCount = 0;
    
    //影响线元索引范围
    int nUpdateBeginIndex = 0;
    int nUpdateEndIndex = 0;
    for (int nJDIndex = nStartJDIndex; nJDIndex + 2 <= nEndJDIndex; nJDIndex++)
    {
        //重置数据
        memset(arrLineElement, 0, sizeof(BaseLineElement*) * 5);
        nUnitElementCount = 0;
        
        //包含索引
        int* arrIndex = (m_arrJD + nJDIndex + 1)->arrUnitsIndex;
        uint8_t nIndexCount = (m_arrJD + nJDIndex + 1)->nIndexCount;
        
        if (nJDIndex == nStartJDIndex)
            nUpdateBeginIndex = __max(arrIndex[0] - 1, 0);
        nUpdateEndIndex = __min(arrIndex[nIndexCount - 1] + 1, m_nElementCount - 1);
        
        //找出修改的线元
        for (uint8_t nIndex = 0; nIndex < nIndexCount; nIndex++)
            arrLineElement[nIndex] = m_arrLineElement[arrIndex[nIndex]];
        
        //重新计算数据
        CalculateLineElement(nJDIndex, arrLineElement, nUnitElementCount);
    }
    
    //更新其他线元
    double dCurCml = m_arrLineElement[nUpdateBeginIndex]->dStartCml;
    for (int i = nUpdateBeginIndex; i < m_nElementCount; i++)
    {
        m_arrLineElement[i]->dStartCml = dCurCml;
        if (i <= nUpdateEndIndex && m_arrLineElement[i]->eElementType == ElementType::Line)
        {
            int nPreIndex = __max(0, i - 1);
            int nNextIndex = __min(m_nElementCount - 1, i + 1);
            
            m_arrLineElement[i]->pntStart = (nPreIndex == 0 ? Point2d(m_arrJD->dX, m_arrJD->dY) : m_arrLineElement[nPreIndex]->pntEnd);
            m_arrLineElement[i]->pntEnd = (nNextIndex == m_nElementCount - 1 ? Point2d((m_arrJD + nEndJDIndex)->dX, (m_arrJD + nEndJDIndex)->dY) : m_arrLineElement[nNextIndex]->pntStart);
            m_arrLineElement[i]->dTotalLen = m_arrLineElement[i]->pntStart.distanceTo(m_arrLineElement[i]->pntEnd);
            m_arrLineElement[i]->dStartTanAngle = m_arrLineElement[i]->dEndTanAngle = BaseCalFun::CalAngleX(m_arrLineElement[i]->pntStart, m_arrLineElement[i]->pntEnd);
        }
        dCurCml += m_arrLineElement[i]->dTotalLen;
    }
    
#endif
    
}

tagExportLineElement* LineElementManager::ExportHorCurve(int& nArrCount, double dStartCml, double dEndCml, double dDist, double dCurveStep)
{
    nArrCount = 0;
    if (dEndCml - dStartCml < 0.0)
        return nullptr;
    
    BaseLineElement* pStartElement = CmlBelongTo(dStartCml);
    if (!pStartElement)
        return nullptr;
    
    BaseLineElement* pEndElement = CmlBelongTo(dEndCml);
    if (!pEndElement)
        return nullptr;
    
    int nStartIndex = pStartElement->nIndex;
    int nEndIndex = pEndElement->nIndex;
    
    nArrCount = nEndIndex - nStartIndex + 1;
    tagExportLineElement* pRet = new tagExportLineElement[nArrCount];
    int nExportIndex = 0;
    for (int nCurInex = nStartIndex; nCurInex <= nEndIndex; nCurInex++)
    {
        BaseLineElement* pCurLineElement = m_arrLineElement[nCurInex];
        if (pCurLineElement->dTotalLen <= s_dLenPrecision)
        {
            nArrCount--;
            continue;
        }
        
        double dCurStartCml = (nCurInex == nStartIndex ? dStartCml : pCurLineElement->dStartCml);
        double dCurEndCml = (nCurInex == nEndIndex ? dEndCml : pCurLineElement->dStartCml + pCurLineElement->dTotalLen);
        tagExportLineElement* pExportInfo = pCurLineElement->ExportHorCurve(dCurStartCml, dCurEndCml, dDist, dCurveStep);
        if (!pExportInfo)
        {
            nArrCount = 0;
            delete[] pRet;
            return nullptr;
        }
        *(pRet + nExportIndex++) = *pExportInfo;
        delete pExportInfo;
    }
    
    return pRet;
}

const tagJDInfo* LineElementManager::ExportJDInfo(int& nCount)
{
    nCount = m_nJDCount;
    return m_arrJD;
}

double LineElementManager::GetLength()
{
    if (m_nElementCount == 0 || !m_arrLineElement || m_nJDCount < 2 || !m_arrJD)
        return 0.0;
    
    switch (m_eCurveType) {
        case CurveType::HorizontalCurve:
            return m_arrLineElement[m_nElementCount - 1]->dStartCml + m_arrLineElement[m_nElementCount - 1]->dTotalLen;
        case CurveType::VerticalCurve:
            return m_arrJD[m_nJDCount - 1].dX - m_arrJD[0].dX;
        default:
            return 0.0;
    }
}
