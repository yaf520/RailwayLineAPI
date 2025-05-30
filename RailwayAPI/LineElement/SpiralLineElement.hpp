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
    typedef double (SpiralLineElement::*pFunc)(double, double, double);
    
public:
    SpiralLineElement();
    virtual ~SpiralLineElement() {}
    
public:
    double dEnterR;       //曲线起始半径
    double dExitR;        //曲线结束半径
    double dC;            //缓和曲线参数
    bool bTurnLeft;       //转向方向
    
private:
    bool m_bEnter;          //入or出
    Point2d m_pntBase;      //计算基准点
    double m_dBaseTanAngle; //计算基准切线角
    double m_dHideLen;      //非完整缓和曲线隐藏长度
    
public:
    bool TrsCmlDistToNE(double dCml, double dDist, double& dX, double& dY, double& dAngle) override;
    
    uint32_t TrsNEToCmlDist(double dX, double dY, double arrCml[s_nMaxArrCount], double arrDist[s_nMaxArrCount], double arrAngle[s_nMaxArrCount]) override;
    
    bool TrsCmlToHeight(double dCml, double& dHeight, double& dAngle) override { return false;};
    
    uint32_t IntersectWithLine(double dAngle, double dX, double dY, Point2d arrCrossPos[s_nMaxArrCount]) override;
    
    tagExportLineElement* ExportHorCurve(double dStartCml, double dEndCml, double dDist, double dCurveStep) override;
    
protected:
    //相对里程->相对坐标
    Point2d TrsCmlToNE_Relative(double dCml) override;
    //相对里程->相对角度
    inline double TrsCmlToAngle_Relative(double dCml) override { return dCml * dCml / 2.0 / dC; }
    
protected:
    //相对里程->圆心坐标
    Point2d TrsCmlToCenter_Relative(double dCml);

public:
    void InitData() override;
    
    void AdjustData(const Point2d& pos) override;
   
    ///求相对坐标
private:
    inline double relative_x(double l, int n){
        return (pow(-1, n) * pow(1.0 / dC, 2 * n) * pow(l, 4 * n + 1) / (BaseCalFun::Factorial(2 * n) * (4 * n + 1) * pow(2, 2 * n)));
    }
    inline double relative_y(double l, int n){
        return (pow(-1, n) * pow(1.0 / dC, 2 * n + 1) * pow(l, 4 * n + 3) / (BaseCalFun::Factorial(2 * n + 1) * (4 * n + 3) * pow(2, 2 * n + 1)));
    }
    inline double relative_dx(double l, int n){
        return (pow(-1, n) * pow(1.0 / dC, 2 * n) * pow(l, 4 * n) * (4 * n + 1) / (BaseCalFun::Factorial(2 * n) * (4 * n + 1) * pow(2, 2 * n)));
    }
    inline double relative_dy(double l, int n){
        return (pow(-1, n) * pow(1.0 / dC, 2 * n + 1) * pow(l, 4 * n + 2) * (4 * n + 3) / (BaseCalFun::Factorial(2 * n + 1) * (4 * n + 3) * pow(2, 2 * n + 1)));
    }
    inline double relative_d2x(double l, int n){
        return (n == 0 ? 0 : (pow(-1, n) * pow(1.0 / dC, 2 * n) * pow(l, 4 * n - 1) * (4 * n + 1) * 4 * n / (BaseCalFun::Factorial(2 * n) * (4 * n + 1) * pow(2, 2 * n))));
    }
    inline double relative_d2y(double l, int n){
        return (pow(-1, n) * pow(1.0 / dC, 2 * n + 1) * pow(l, 4 * n + 1) * (4 * n + 3) * (4 * n + 2) / (BaseCalFun::Factorial(2 * n + 1) * (4 * n + 3) * pow(2, 2 * n + 1)));
    }
    
private:
    ///原函数
    double f_original_proj(double x0, double dParamX, double dParamY);
    ///原函数
    double f_original_cross(double x0, double k, double b);
    ///一阶导函数
    double f_first_deriv_proj(double x0, double dParamX, double dParamY);
    ///一阶导函数
    double f_first_deriv_cross(double x0, double k, double b);
    ///二阶导函数
    double f_second_deriv_proj(double x0, double dParamX, double dParamY);
    ///二阶导函数
    double f_second_deriv_cross(double x0, double k, double b);
    

    
private:
    ///预估根
    uint32_t EstimateRoot(pFunc pf_original, pFunc pf_first_deriv, pFunc pf_second_deriv, double dParamX, double dParamY, double arrEstimateRoot[s_nMaxArrCount]);
    ///牛顿迭代法
    bool NewtonIter(pFunc pf_original, pFunc pf_first_deriv, double dEstimateRoot, double dParamX, double dParamY, double& dRoot);
};


#endif /* CurveElement_hpp */
