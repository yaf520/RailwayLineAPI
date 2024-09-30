//
//  CurveElement.hpp
//
//  Created by 易傲飞 on 2023/10/27.
//

#ifndef CurveElement_hpp
#define CurveElement_hpp

#include "BaseLineElement.hpp"
#include "BaseCalFun.hpp"

class SpiralLineElement : public BaseLineElement
{
    ///函数指针
    typedef double (SpiralLineElement::*pFunc)(const double&, const double&, const double&);
    
public:
    SpiralLineElement();
    virtual ~SpiralLineElement() {}
    
public:
    bool m_bTurnLeft;       //转向方向
    double m_dEnterR;       //曲线起始半径
    double m_dExitR;        //曲线结束半径
    double m_dC;            //缓和曲线参数
    
private:
    Point2d m_posBase;      //计算基准点
    double m_dBaseTanAngle; //计算基准切线角
    double m_dHideLen;      //非完整缓和曲线隐藏长度
    bool m_bEnter;          //入or出
    
public:
    bool TrsCmlDistToNE(const double& dCml, const double& dDist, double& dX, double& dY, double& dAngle) override;
    
    uint32_t TrsNEToCmlDist(const double& dX, const double& dY, double arrCml[s_nMaxArrCount], double arrDist[s_nMaxArrCount], double arrAngle[s_nMaxArrCount]) override;
    
    bool TrsCmlToHeight(const double& dCml, double& dHeight, double& dAngle) override { return false;};
    
    uint32_t IntersectWithLine(const double& dAngle, const double& dX, const double& dY, Point2d arrCrossPos[s_nMaxArrCount]) override;
    
    tagExportLineElement* ExportHorCurve(double dStartCml, double dEndCml, double dDist, double dCurveStep) override;
    
protected:
    //相对里程->相对坐标
    Point2d TrsCmlToNE_Relative(const double& dCml) override;
    //相对里程->相对角度
    inline double TrsCmlToAngle_Relative(const double& dCml) override { return dCml * dCml / 2.0 / m_dC; }
    
protected:
    //相对里程->圆心坐标
    Point2d TrsCmlToCenter_Relative(const double& dCml);

public:
    void InitData() override;
    
    void AdjustData(const Point2d& pos) override;
    
private:
    ///原函数
    double f_original_proj(const double& x0, const double& dParamX, const double& dParamY);
    ///原函数
    double f_original_cross(const double& x0, const double& k, const double& b);
    ///一阶导函数
    double f_first_deriv_proj(const double& x0, const double& dParamX, const double& dParamY);
    ///一阶导函数
    double f_first_deriv_cross(const double& x0, const double& k, const double& b);
    ///二阶导函数
    double f_second_deriv_proj(const double& x0, const double& dParamX, const double& dParamY);
    ///二阶导函数
    double f_second_deriv_cross(const double& x0, const double& k, const double& b);
    
private:
    ///预估根
    uint32_t EstimateRoot(pFunc pf_original, pFunc pf_first_deriv, pFunc pf_second_deriv, const double& dParamX, const double& dParamY, double arrEstimateRoot[s_nMaxArrCount]);
    ///牛顿迭代法
    bool NewtonIter(pFunc pf_original, pFunc pf_first_deriv, double dEstimateRoot, const double& dParamX, const double& dParamY, double& dRoot);
};


#endif /* CurveElement_hpp */
