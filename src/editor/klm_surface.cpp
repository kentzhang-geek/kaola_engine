#include "include/kaola_engine/glheaders.h"

#ifndef _ArcBall_h
#define _ArcBall_h

#include <stdlib.h>

// 仅在Debug模式下，启用断言
#ifdef _DEBUG
# include "assert.h"
#else
# define assert(x) { }
#endif


//2维点
typedef union Tuple2f_t
{
    struct
    {
        GLfloat X, Y;
    } s;

    GLfloat T[2];
} Tuple2fT;

//3维点
typedef union Tuple3f_t
{
    struct
    {
        GLfloat X, Y, Z;
    } s;

    GLfloat T[3];
} Tuple3fT;

//4维点
typedef union Tuple4f_t
{
    struct
    {
        GLfloat X, Y, Z, W;
    } s;

    GLfloat T[4];
} Tuple4fT;


//3x3矩阵
typedef union Matrix3f_t
{
    struct
    {
        //column major
        union { GLfloat M00; GLfloat XX; GLfloat SX; };
        union { GLfloat M10; GLfloat XY;             };
        union { GLfloat M20; GLfloat XZ;             };
        union { GLfloat M01; GLfloat YX;             };
        union { GLfloat M11; GLfloat YY; GLfloat SY; };
        union { GLfloat M21; GLfloat YZ;             };
        union { GLfloat M02; GLfloat ZX;             };
        union { GLfloat M12; GLfloat ZY;             };
        union { GLfloat M22; GLfloat ZZ; GLfloat SZ; };
    } s;
    GLfloat M[9];
} Matrix3fT;

//4x4矩阵
typedef union Matrix4f_t
{
    struct
    {
        //column major
        union { GLfloat M00; GLfloat XX; GLfloat SX; };
        union { GLfloat M10; GLfloat XY;             };
        union { GLfloat M20; GLfloat XZ;             };
        union { GLfloat M30; GLfloat XW;             };
        union { GLfloat M01; GLfloat YX;             };
        union { GLfloat M11; GLfloat YY; GLfloat SY; };
        union { GLfloat M21; GLfloat YZ;             };
        union { GLfloat M31; GLfloat YW;             };
        union { GLfloat M02; GLfloat ZX;             };
        union { GLfloat M12; GLfloat ZY;             };
        union { GLfloat M22; GLfloat ZZ; GLfloat SZ; };
        union { GLfloat M32; GLfloat ZW;             };
        union { GLfloat M03; GLfloat TX;             };
        union { GLfloat M13; GLfloat TY;             };
        union { GLfloat M23; GLfloat TZ;             };
        union { GLfloat M33; GLfloat TW; GLfloat SW; };
    } s;
    GLfloat M[16];
} Matrix4fT;


//定义类型的别名
#define Point2fT    Tuple2fT
#define Quat4fT     Tuple4fT
#define Vector2fT   Tuple2fT
#define Vector3fT   Tuple3fT
#define FuncSqrt    sqrtf
# define Epsilon 1.0e-5

//2维点相加
inline
static void Point2fAdd(Point2fT* NewObj, const Tuple2fT* t1)
{
    assert(NewObj && t1);

    NewObj->s.X += t1->s.X;
    NewObj->s.Y += t1->s.Y;
}

//2维点相减
inline
static void Point2fSub(Point2fT* NewObj, const Tuple2fT* t1)
{
    assert(NewObj && t1);

    NewObj->s.X -= t1->s.X;
    NewObj->s.Y -= t1->s.Y;
}

//3维点矢积
inline
static void Vector3fCross(Vector3fT* NewObj, const Vector3fT* v1, const Vector3fT* v2)
{
    Vector3fT Result;

    assert(NewObj && v1 && v2);

    Result.s.X = (v1->s.Y * v2->s.Z) - (v1->s.Z * v2->s.Y);
    Result.s.Y = (v1->s.Z * v2->s.X) - (v1->s.X * v2->s.Z);
    Result.s.Z = (v1->s.X * v2->s.Y) - (v1->s.Y * v2->s.X);

    *NewObj = Result;
}

//3维点点积
inline
static GLfloat Vector3fDot(const Vector3fT* NewObj, const Vector3fT* v1)
{
    assert(NewObj && v1);

    return  (NewObj->s.X * v1->s.X) +
            (NewObj->s.Y * v1->s.Y) +
            (NewObj->s.Z * v1->s.Z);
}

//3维点的长度的平方
inline
static GLfloat Vector3fLengthSquared(const Vector3fT* NewObj)
{
    assert(NewObj);

    return  (NewObj->s.X * NewObj->s.X) +
            (NewObj->s.Y * NewObj->s.Y) +
            (NewObj->s.Z * NewObj->s.Z);
}

//3维点的长度
inline
static GLfloat Vector3fLength(const Vector3fT* NewObj)
{
    assert(NewObj);

    return FuncSqrt(Vector3fLengthSquared(NewObj));
}

//设置3x3矩阵为0矩阵
inline
static void Matrix3fSetZero(Matrix3fT* NewObj)
{
    NewObj->s.M00 = NewObj->s.M01 = NewObj->s.M02 =
    NewObj->s.M10 = NewObj->s.M11 = NewObj->s.M12 =
    NewObj->s.M20 = NewObj->s.M21 = NewObj->s.M22 = 0.0f;
}

//设置4x4矩阵为0矩阵
inline
static void Matrix4fSetZero(Matrix4fT* NewObj)
{
    NewObj->s.M00 = NewObj->s.M01 = NewObj->s.M02 =
    NewObj->s.M10 = NewObj->s.M11 = NewObj->s.M12 =
    NewObj->s.M20 = NewObj->s.M21 = NewObj->s.M22 =
    NewObj->s.M30 = NewObj->s.M31 = NewObj->s.M32 = 0.0f;
}

//设置3x3矩阵为单位矩阵
inline
static void Matrix3fSetIdentity(Matrix3fT* NewObj)
{
    Matrix3fSetZero(NewObj);

    NewObj->s.M00 =
    NewObj->s.M11 =
    NewObj->s.M22 = 1.0f;
}

//设置4x4矩阵为单位矩阵
inline
static void Matrix4fSetIdentity(Matrix4fT* NewObj)
{
    Matrix4fSetZero(NewObj);

    NewObj->s.M00 = 1.0f;
    NewObj->s.M11 = 1.0f;
    NewObj->s.M22 = 1.0f;
    NewObj->s.M33=1.0f;
}

//从四元数设置旋转矩阵
inline
static void Matrix3fSetRotationFromQuat4f(Matrix3fT* NewObj, const Quat4fT* q1)
{
    GLfloat n, s;
    GLfloat xs, ys, zs;
    GLfloat wx, wy, wz;
    GLfloat xx, xy, xz;
    GLfloat yy, yz, zz;

    assert(NewObj && q1);

    n = (q1->s.X * q1->s.X) + (q1->s.Y * q1->s.Y) + (q1->s.Z * q1->s.Z) + (q1->s.W * q1->s.W);
    s = (n > 0.0f) ? (2.0f / n) : 0.0f;

    xs = q1->s.X * s;  ys = q1->s.Y * s;  zs = q1->s.Z * s;
    wx = q1->s.W * xs; wy = q1->s.W * ys; wz = q1->s.W * zs;
    xx = q1->s.X * xs; xy = q1->s.X * ys; xz = q1->s.X * zs;
    yy = q1->s.Y * ys; yz = q1->s.Y * zs; zz = q1->s.Z * zs;

    NewObj->s.XX = 1.0f - (yy + zz); NewObj->s.YX =         xy - wz;  NewObj->s.ZX =         xz + wy;
    NewObj->s.XY =         xy + wz;  NewObj->s.YY = 1.0f - (xx + zz); NewObj->s.ZY =         yz - wx;
    NewObj->s.XZ =         xz - wy;  NewObj->s.YZ =         yz + wx;  NewObj->s.ZZ = 1.0f - (xx + yy);
}

//3x3矩阵相乘
inline
static void Matrix3fMulMatrix3f(Matrix3fT* NewObj, const Matrix3fT* m1)
{
    Matrix3fT Result;

    assert(NewObj && m1);

    Result.s.M00 = (NewObj->s.M00 * m1->s.M00) + (NewObj->s.M01 * m1->s.M10) + (NewObj->s.M02 * m1->s.M20);
    Result.s.M01 = (NewObj->s.M00 * m1->s.M01) + (NewObj->s.M01 * m1->s.M11) + (NewObj->s.M02 * m1->s.M21);
    Result.s.M02 = (NewObj->s.M00 * m1->s.M02) + (NewObj->s.M01 * m1->s.M12) + (NewObj->s.M02 * m1->s.M22);

    Result.s.M10 = (NewObj->s.M10 * m1->s.M00) + (NewObj->s.M11 * m1->s.M10) + (NewObj->s.M12 * m1->s.M20);
    Result.s.M11 = (NewObj->s.M10 * m1->s.M01) + (NewObj->s.M11 * m1->s.M11) + (NewObj->s.M12 * m1->s.M21);
    Result.s.M12 = (NewObj->s.M10 * m1->s.M02) + (NewObj->s.M11 * m1->s.M12) + (NewObj->s.M12 * m1->s.M22);

    Result.s.M20 = (NewObj->s.M20 * m1->s.M00) + (NewObj->s.M21 * m1->s.M10) + (NewObj->s.M22 * m1->s.M20);
    Result.s.M21 = (NewObj->s.M20 * m1->s.M01) + (NewObj->s.M21 * m1->s.M11) + (NewObj->s.M22 * m1->s.M21);
    Result.s.M22 = (NewObj->s.M20 * m1->s.M02) + (NewObj->s.M21 * m1->s.M12) + (NewObj->s.M22 * m1->s.M22);

    *NewObj = Result;
}

//4x4矩阵相乘
inline
static void Matrix4fSetRotationScaleFromMatrix4f(Matrix4fT* NewObj, const Matrix4fT* m1)
{
    assert(NewObj && m1);

    NewObj->s.XX = m1->s.XX; NewObj->s.YX = m1->s.YX; NewObj->s.ZX = m1->s.ZX;
    NewObj->s.XY = m1->s.XY; NewObj->s.YY = m1->s.YY; NewObj->s.ZY = m1->s.ZY;
    NewObj->s.XZ = m1->s.XZ; NewObj->s.YZ = m1->s.YZ; NewObj->s.ZZ = m1->s.ZZ;
}

//进行矩阵的奇异值分解，旋转矩阵被保存到rot3和rot4中，返回矩阵的缩放因子
inline
static GLfloat Matrix4fSVD(const Matrix4fT* NewObj, Matrix3fT* rot3, Matrix4fT* rot4)
{
    GLfloat s, n;

    assert(NewObj);

    s = FuncSqrt(
            ( (NewObj->s.XX * NewObj->s.XX) + (NewObj->s.XY * NewObj->s.XY) + (NewObj->s.XZ * NewObj->s.XZ) +
              (NewObj->s.YX * NewObj->s.YX) + (NewObj->s.YY * NewObj->s.YY) + (NewObj->s.YZ * NewObj->s.YZ) +
              (NewObj->s.ZX * NewObj->s.ZX) + (NewObj->s.ZY * NewObj->s.ZY) + (NewObj->s.ZZ * NewObj->s.ZZ) ) / 3.0f );

    if (rot3)
    {
        rot3->s.XX = NewObj->s.XX; rot3->s.XY = NewObj->s.XY; rot3->s.XZ = NewObj->s.XZ;
        rot3->s.YX = NewObj->s.YX; rot3->s.YY = NewObj->s.YY; rot3->s.YZ = NewObj->s.YZ;
        rot3->s.ZX = NewObj->s.ZX; rot3->s.ZY = NewObj->s.ZY; rot3->s.ZZ = NewObj->s.ZZ;

        n = 1.0f / FuncSqrt( (NewObj->s.XX * NewObj->s.XX) +
                             (NewObj->s.XY * NewObj->s.XY) +
                             (NewObj->s.XZ * NewObj->s.XZ) );
        rot3->s.XX *= n;
        rot3->s.XY *= n;
        rot3->s.XZ *= n;

        n = 1.0f / FuncSqrt( (NewObj->s.YX * NewObj->s.YX) +
                             (NewObj->s.YY * NewObj->s.YY) +
                             (NewObj->s.YZ * NewObj->s.YZ) );
        rot3->s.YX *= n;
        rot3->s.YY *= n;
        rot3->s.YZ *= n;

        n = 1.0f / FuncSqrt( (NewObj->s.ZX * NewObj->s.ZX) +
                             (NewObj->s.ZY * NewObj->s.ZY) +
                             (NewObj->s.ZZ * NewObj->s.ZZ) );
        rot3->s.ZX *= n;
        rot3->s.ZY *= n;
        rot3->s.ZZ *= n;
    }

    if (rot4)
    {
        if (rot4 != NewObj)
        {
            Matrix4fSetRotationScaleFromMatrix4f(rot4, NewObj);
        }


        n = 1.0f / FuncSqrt( (NewObj->s.XX * NewObj->s.XX) +
                             (NewObj->s.XY * NewObj->s.XY) +
                             (NewObj->s.XZ * NewObj->s.XZ) );
        rot4->s.XX *= n;
        rot4->s.XY *= n;
        rot4->s.XZ *= n;

        n = 1.0f / FuncSqrt( (NewObj->s.YX * NewObj->s.YX) +
                             (NewObj->s.YY * NewObj->s.YY) +
                             (NewObj->s.YZ * NewObj->s.YZ) );
        rot4->s.YX *= n;
        rot4->s.YY *= n;
        rot4->s.YZ *= n;

        n = 1.0f / FuncSqrt( (NewObj->s.ZX * NewObj->s.ZX) +
                             (NewObj->s.ZY * NewObj->s.ZY) +
                             (NewObj->s.ZZ * NewObj->s.ZZ) );
        rot4->s.ZX *= n;
        rot4->s.ZY *= n;
        rot4->s.ZZ *= n;
    }

    return s;
}

//从3x3矩阵变为4x4的旋转矩阵
inline
static void Matrix4fSetRotationScaleFromMatrix3f(Matrix4fT* NewObj, const Matrix3fT* m1)
{
    assert(NewObj && m1);

    NewObj->s.XX = m1->s.XX; NewObj->s.YX = m1->s.YX; NewObj->s.ZX = m1->s.ZX;
    NewObj->s.XY = m1->s.XY; NewObj->s.YY = m1->s.YY; NewObj->s.ZY = m1->s.ZY;
    NewObj->s.XZ = m1->s.XZ; NewObj->s.YZ = m1->s.YZ; NewObj->s.ZZ = m1->s.ZZ;
}

//4x4矩阵的与标量的乘积
inline
static void Matrix4fMulRotationScale(Matrix4fT* NewObj, GLfloat scale)
{
    assert(NewObj);

    NewObj->s.XX *= scale; NewObj->s.YX *= scale; NewObj->s.ZX *= scale;
    NewObj->s.XY *= scale; NewObj->s.YY *= scale; NewObj->s.ZY *= scale;
    NewObj->s.XZ *= scale; NewObj->s.YZ *= scale; NewObj->s.ZZ *= scale;
}

//设置旋转矩阵
inline
static void Matrix4fSetRotationFromMatrix3f(Matrix4fT* NewObj, const Matrix3fT* m1)
{
    GLfloat scale;

    assert(NewObj && m1);

    scale = Matrix4fSVD(NewObj, NULL, NULL);

    Matrix4fSetRotationScaleFromMatrix3f(NewObj, m1);
    Matrix4fMulRotationScale(NewObj, scale);
}



typedef class ArcBall_t
{
protected:
    //把二维点映射到三维点
    inline
    void _mapToSphere(const Point2fT* NewPt, Vector3fT* NewVec) const;

public:
    //构造/析构函数
    ArcBall_t(GLfloat NewWidth, GLfloat NewHeight);
    ~ArcBall_t() { };

    //设置边界
    inline
    void    setBounds(GLfloat NewWidth, GLfloat NewHeight)
    {
        assert((NewWidth > 1.0f) && (NewHeight > 1.0f));

        //设置长宽的调整因子
        this->AdjustWidth  = 1.0f / ((NewWidth  - 1.0f) * 0.5f);
        this->AdjustHeight = 1.0f / ((NewHeight - 1.0f) * 0.5f);
    }

    //鼠标点击
    void    click(const Point2fT* NewPt);

    //鼠标拖动计算旋转
    void    drag(const Point2fT* NewPt, Quat4fT* NewRot);

    //更新鼠标状态
    void    upstate();
    //void    mousemove(WPARAM wParam,LPARAM lParam);

protected:
    Vector3fT   StVec;          //保存鼠标点击的坐标
    Vector3fT   EnVec;          //保存鼠标拖动的坐标
    GLfloat     AdjustWidth;    //宽度的调整因子
    GLfloat     AdjustHeight;   //长度的调整因子
public:
    Matrix4fT   Transform;      //计算变换
    Matrix3fT   LastRot;        //上一次的旋转
    Matrix3fT   ThisRot;        //这次的旋转
    float zoomRate;
    float lastZoomRate;

    bool        isDragging;     // 是否拖动
    bool        isRClicked;     // 是否右击鼠标
    bool        isClicked;      // 是否点击鼠标
    bool        isZooming;    //是否正在缩放
    Point2fT    LastPt;
    Matrix4fT origTransform;
    Point2fT    MousePt;        // 当前的鼠标位置

} ArcBallT;

#endif


/** KempoApi: The Turloc Toolkit *****************************/
/** *    *                                                  **/
/** **  **  Filename: ArcBall.cpp                           **/
/**   **    Version:  Common                                **/
/**   **                                                    **/
/**                                                         **/
/**  Arcball class for mouse manipulation.                  **/
/**                                                         **/
/**                                                         **/
/**                                                         **/
/**                                                         **/
/**                              (C) 1999-2003 Tatewake.com **/
/**   History:                                              **/
/**   08/17/2003 - (TJG) - Creation                         **/
/**   09/23/2003 - (TJG) - Bug fix and optimization         **/
/**   09/25/2003 - (TJG) - Version for NeHe Basecode users  **/
/**                                                         **/
/*************************************************************/
//
#include <math.h>

//轨迹球参数:
//直径                    2.0f
//半径                    1.0f
//半径平方                1.0f


void ArcBall_t::_mapToSphere(const Point2fT* NewPt, Vector3fT* NewVec) const
{
    Point2fT TempPt;
    GLfloat length;

    //复制到临时变量
    TempPt = *NewPt;

    //把长宽调整到[-1 ... 1]区间
    TempPt.s.X  =        (TempPt.s.X * this->AdjustWidth)  - 1.0f;
    TempPt.s.Y  = 1.0f - (TempPt.s.Y * this->AdjustHeight);

    //计算长度的平方
    length      = (TempPt.s.X * TempPt.s.X) + (TempPt.s.Y * TempPt.s.Y);

    //如果点映射到球的外面
    if (length > 1.0f)
    {
        GLfloat norm;

        //缩放到球上
        norm    = 1.0f / FuncSqrt(length);

        //设置z坐标为0
        NewVec->s.X = TempPt.s.X * norm;
        NewVec->s.Y = TempPt.s.Y * norm;
        NewVec->s.Z = 0.0f;
    }
        //如果在球内
    else
    {
        //利用半径的平方为1,求出z坐标
        NewVec->s.X = TempPt.s.X;
        NewVec->s.Y = TempPt.s.Y;
        NewVec->s.Z = FuncSqrt(1.0f - length);
    }
}

ArcBall_t::ArcBall_t(GLfloat NewWidth, GLfloat NewHeight)
{
    this->StVec.s.X     = 0.0f;
    this->StVec.s.Y     = 0.0f;
    this->StVec.s.Z     = 0.0f;

    this->EnVec.s.X     = 0.0f;
    this->EnVec.s.Y     = 0.0f;
    this->EnVec.s.Z     = 0.0f;


    Matrix4fSetIdentity(&Transform);
    Matrix3fSetIdentity(&LastRot);
    Matrix3fSetIdentity(&ThisRot);

    this->isDragging=false;
    this->isClicked= false;
    this->isRClicked = false;
    this->isZooming = false;
    this->zoomRate = 1;
    this->setBounds(NewWidth, NewHeight);
}

void ArcBall_t::upstate()
{
    if(!this->isZooming && this->isRClicked){                    // 开始拖动
        this->isZooming = true;                                        // 设置拖动为变量为true
        this->LastPt = this->MousePt;
        this->lastZoomRate = this->zoomRate;
    }
    else if(this->isZooming)
    {//正在拖动
        if(this->isRClicked)
        {  //拖动
            Point2fSub(&this->MousePt, &this->LastPt);
            this->zoomRate = this->lastZoomRate + this->MousePt.s.X * this->AdjustWidth * 2;
        }
        else{                                            //停止拖动
            this->isZooming = false;
        }
    }
    else if (!this->isDragging && this->isClicked){                                                // 如果没有拖动
        this->isDragging = true;                                        // 设置拖动为变量为true
        this->LastRot = this->ThisRot;
        this->click(&this->MousePt);
    }
    else if(this->isDragging){
        if (this->isClicked){                                            //如果按住拖动
            Quat4fT     ThisQuat;

            this->drag(&this->MousePt, &ThisQuat);                        // 更新轨迹球的变量
            Matrix3fSetRotationFromQuat4f(&this->ThisRot, &ThisQuat);        // 计算旋转量
            Matrix3fMulMatrix3f(&this->ThisRot, &this->LastRot);
            Matrix4fSetRotationFromMatrix3f(&this->Transform, &this->ThisRot);
        }
        else                                                        // 如果放开鼠标，设置拖动为false
            this->isDragging = false;
    }
}

//按下鼠标,记录当前对应的轨迹球的位置
void    ArcBall_t::click(const Point2fT* NewPt)
{
    this->_mapToSphere(NewPt, &this->StVec);
}

//鼠标拖动,计算旋转四元数
void    ArcBall_t::drag(const Point2fT* NewPt, Quat4fT* NewRot)
{
    //新的位置
    this->_mapToSphere(NewPt, &this->EnVec);

    //计算旋转
    if (NewRot)
    {
        Vector3fT  Perp;

        //计算旋转轴
        Vector3fCross(&Perp, &this->StVec, &this->EnVec);

        //如果不为0
        if (Vector3fLength(&Perp) > Epsilon)
        {
            //记录旋转轴
            NewRot->s.X = Perp.s.X;
            NewRot->s.Y = Perp.s.Y;
            NewRot->s.Z = Perp.s.Z;
            //在四元数中,w=cos(a/2)，a为旋转的角度
            NewRot->s.W= Vector3fDot(&this->StVec, &this->EnVec);
        }
            //是0，说明没有旋转
        else
        {
            NewRot->s.X =
            NewRot->s.Y =
            NewRot->s.Z =
            NewRot->s.W = 0.0f;
        }
    }
}
