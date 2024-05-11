//
//  BaseCalFun.hpp
//
//  Created by 易傲飞 on 2023/10/27.
//

#ifndef BaseCalFun_hpp
#define BaseCalFun_hpp

#include "BasePoint.hpp"

#define __max(a, b) (((a) > (b)) ? (a) : (b))
#define __min(a, b) (((a) < (b)) ? (a) : (b))

class BaseCalFun
{
public:
    /// 计算直线与X轴的夹角
    /// - Parameters:
    ///   - posStart: 起点坐标
    ///   - posEnd: 终点坐标
    static double CalAngleX(Point2d posStart, Point2d posEnd);
    
    /// 点到直线的投影点坐标
    /// - Parameters:
    ///   - A: 直线外的点
    ///   - O: 直线上的点1
    ///   - B: 直线上的点2
    ///   - dPecent: 百分比
    static Point2d PointToLineProjection(Point2d A, Point2d O, Point2d B, double& dPecent);
    
    /// 计算转向角
    /// - Parameters:
    ///   - p1: 第一个点
    ///   - p2: 第二个点
    ///   - p3: 第三个点
    static double CalTurnAngle(Point2d p1, Point2d p2, Point2d p3);
    
    /// 计算转向角
    /// - Parameters:
    ///   - p1: 第一个点
    ///   - p2: 第二个点
    ///   - p3: 第三个点
    ///   - p4: 第四个点
    static double CalTurnAngle(Point2d p1, Point2d p2, Point2d p3, Point2d p4);
    
    /// 坐标转换
    /// - Parameters:
    ///   - posBase: 基准点
    ///   - posTransfer: 转换点
    ///   - bTurnDir: 转向
    ///   - dAngle: 转向角度
    static Point2d TransferPos(Point2d posBase, Point2d posTransfer, bool bTurnDir, double dAngle);
    
    /// 坐标转换
    /// - Parameters:
    ///   - posBase: 基准点
    ///   - posTransfer: 转换点
    ///   - bTurnDir: 转向
    ///   - dAngle: 转向角度
    static Point2d TransferPosReversal(Point2d posBase, Point2d posTransfer, bool bTurnDir, double dAngle);
    
    static Point2d TransferBasePos(Point2d posTarget, Point2d posRelative, bool bTurnDir, double dAngle);
    
    /// 保持弧度值在0-2pi之间
    /// - Parameter dAngle: 弧度值
    static void KeepAngleIn2PI(double& dAngle);
    
    /// 四舍五入小数点
    /// - Parameters:
    ///   - dValue: 源数值
    ///   - nPrec: 小数位数
    static double Round(const double& dValue, int nPrec = 6);
    
    /// 将外业里程冠号与数值分开
    /// - Parameter dMileValue: 数字部分
    static void SplitMilepost(const char strMilepost[64], double& dMileValue, char strMakr[16]);
    
    /// 将冠号、外业历程转换为要求精度的里程字符串
    /// - Parameters:
    ///   - dMileVal: 冠号
    ///   - nPrec: 精度
    ///   - bDelZero: 是否删0
    static void MileCharacterStr(const char strMilepost[16], double dMileVal, int nPrec, bool bDelZero, char strOut[64]);
    
    static int CalDecimalDigits(double dValue);
    
    /// 求阶乘
    /// - Parameter n: n的阶乘
    static uint32_t Factorial(uint32_t n);
};

#endif /* BaseCalFun_hpp */
