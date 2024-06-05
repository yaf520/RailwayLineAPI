//
//  CurveElement.hpp
//
//  Created by 易傲飞 on 2023/10/27.
//

#ifndef CurveElement_hpp
#define CurveElement_hpp

#include "BaseLineElement.hpp"
#include "BaseCalFun.hpp"

///原函数
typedef double (*func)(double, double, double);
///导函数
typedef double (*func_d)(double, double, double);

class CurveElement : public BaseLineElement
{
public:
    CurveElement();
    virtual ~CurveElement() {}
    
public:
    bool m_bTurnLeft;       //转向方向
    double m_dEnterR;       //曲线起始半径
    double m_dExitR;        //曲线结束半径
    
private:
    Point2d m_posBase;      //计算基准点
    double m_dBaseTanAngle; //计算基准切线角
    double m_dHideLen;      //非完整缓和曲线隐藏长度
    double m_dC;            //缓和曲线参数
    bool m_bEnter;          //入or出
    
public:
    bool TrsCmlDistToNE(const double& dCml, const double& dDist, double& dX, double& dY, double& dAngle) override;
    
    bool TrsNEToCmlDist(const double& dX, const double& dY, double& dCml, double& dDist, double& dAngle) override;
    
    bool TrsCmlToHeight(const double& dCml, double& dHeight, double& dAngle) override { return false;};
    
    tagExportLineElement* ExportHorCurve(double dStartCml, double dEndCml, double dDist, double dCurveStep) override;
    
protected:
    //相对里程->相对坐标
    Point2d TrsCmlToNE_Relative(const double& dCml) override;
    //相对里程->相对角度
    double TrsCmlToAngle_Relative(const double& dCml) override;
    
protected:
    //相对里程->圆心坐标
    Point2d TrsCmlToCenter_Relative(const double& dCml);

public:
    void InitData() override;
    
    void AdjustData(const Point2d& pos) override;
    
private:
    ///预估根
    bool EstimateRoot(const double& dParamX, const double& dParamY, double& dRoot);
    ///原函数
    double f_original(double x0, const double& dParamX, const double& dParamY);
    ///一阶导函数
    double f_first_deriv(double x0, const double& dParamX, const double& dParamY);
    ///二阶导函数
    double f_second_deriv(double x0, const double& dParamX, const double& dParamY);
    ///迭代法
    bool Newton_Raphson(double (CurveElement::*pf_original)(double, const double&, const double&), double (CurveElement::*pf_first_deriv)(double, const double&, const double&), double dEstimateRoot, const double& dParamX, const double& dParamY, double& dRoot);
};


#endif /* CurveElement_hpp */
