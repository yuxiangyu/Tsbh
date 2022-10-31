#ifndef SYSCONST_H_HEADER_INCLUDED_B5107351
#define SYSCONST_H_HEADER_INCLUDED_B5107351
//**********************************************************
//系统常量
//**********************************************************
//#include "Ogre.h"

#include "rtsscore_global.h"
#include <string>
#include <vector>

class  RTSSCORE_EXPORT CSysConst
{
public:
	//static const std::string IP;
	// 地球引力常数
	static const double ORB_MIU;

	// 地球自转平均角速度
	static const double ORB_OMEGA_E;
		
	//地球静止轨道半长轴
	static const double ORB_Eth_A;

	// 地球公转平均角速度
	static const double ORB_OMEGA_Er;

	// 黄赤交角，单位为弧度
	static const double ORB_I_ECLIP;

	// 地球赤道半径
	static const double ORB_R_Ee;

	//##ModelId=4AEFF541035B
	static const double ORB_R_Em;
	//极半径
	static const double ORB_R_Po;

	// 太阳黄经平均变化率
	static const double ORB_W_LAMDA;

	// 真空中的光速
	static const double ORB_c;

	// 海平面处引力常数
	static const double ORB_g;

	// 圆周率
	static const double ORB_PI;

	//2倍圆周率
	static const double ORB_2PI;

	//临界倾角轨道轨道倾角
	static const double Inc_Critical;

	// 轨道计算迭代精度
	static const double ORB_TOLERC;

	// 太阳同步轨道平均进动角速度
	static const double ORB_MANGLVEL;

	// J2常数项
	static const double WGS_J2;

	// J3常数项
	static const double WGS_J3;

	// J4常数项
	static const double WGS_J4;

	// J22常数项
	//##ModelId=4AEFF5420128
	static const double WGS_J22;

	//太阳同步轨道最小倾角
	static const double ORB_SUNSYN_minINC;

	//轨道最小高度
	static const double ORB_minHeight;

	//最小高度对应每天最多圈数
	static const double ORB_maxCir;

	//太阳同步轨道轨道最大高度
	static const double ORB_SunSyn_maxHeight;

	// 角度和弧度之间转化的常数
	static const double ORB_PI180;

	//历元J2000的儒列日数
	static const double J2000D;

	//历元B1950的儒列日数
	static const double B1950;

	//质心大陆时和过几原子时的时间差
	static const double dTA;

	//角秒化弧度因子
	static const double AsToR;

	//地心一个赤道坐标系到OGRE世界坐标系的旋转矩阵
	//static const Ogre::Matrix3 ecef2ogre;

	//每天的秒数
	static const double DaySeconds;

	////标志向量的结束
	//static const Ogre::Vector2 EndMark2D;
	//static const Ogre::Vector3 EndMark3D;

	////迭代次数
	//static const UINT			IteratorNo;

	////空容器
	//static const std::vector<Ogre::Vector3> NULLVV3;

	//static const Ogre::Vector2 Vector2NULL;
	static const int MAX_TABLEROWCOUNT;
	//经纬度到墨卡托坐标转换
	static void llToMokatuo(double lon,double lat,float &x,float &y);
	static void MokatuoToll(double x, double y, float &lon, float &lat);
	static void lonlatToMercator(double lon, double lat, double &mercx, double &mercy);
	static void mercatorTolonlat(double mercx, double mercy, double &lon, double &lat);
	static std::vector<std::string> vStringSplit(std::string& s,std::string delim);
};
// namespace CSTK
#endif /* SYSCONST_H_HEADER_INCLUDED_B5107351 */