#include "SysConst.h"
#include <math.h>
#include <osg/CoordinateSystemNode>
#include <osg/Vec3>

using namespace std;

//const string CSysConst::IP ="192.168.21.99";

const double CSysConst::ORB_MIU = 3.986004418e14;

    // 地球自转平均角速度
const double CSysConst::ORB_OMEGA_E = 7.29211581560071e-5;

//地球静止轨道的半长轴
const double CSysConst::ORB_Eth_A = 42164169.78720187;

    // 地球公转平均角速度
const double CSysConst::ORB_OMEGA_Er = 0.000000199105416;

    // 黄赤交角，单位为弧度
const double CSysConst::ORB_I_ECLIP = 0.40927970959267;

    // 地球赤道半径
const double CSysConst::ORB_R_Ee = 6378136.49;

    //地球平均半径
const double CSysConst::ORB_R_Em = 6371004.0;
//地球极半径
const double CSysConst::ORB_R_Po = 6356908.8;

    // 太阳黄经平均变化率
const double CSysConst::ORB_W_LAMDA = 0.000000199105416;

    // 真空中的光速
const double CSysConst::ORB_c = 299792458.0;

    // 海平面处引力常数
const double CSysConst::ORB_g = 9.80665;

    // 圆周率
const double CSysConst::ORB_PI = 3.1415926535897932384626;

const double CSysConst::ORB_2PI = 6.2831853071795864769252;

	// 临界倾角轨道轨道倾角
const double CSysConst::Inc_Critical = 1.106538745764405;

    // 轨道计算迭代精度
const double CSysConst::ORB_TOLERC = 1e-8;

    // 太阳同步轨道平均进动角速度
const double CSysConst::ORB_MANGLVEL = 0.0000001991063802746144;

    // J2常数项
const double CSysConst::WGS_J2 = 1.082626683553e-3;

    // J3常数项
const double CSysConst::WGS_J3 = -2.5321531e-6;

    // J4常数项
const double CSysConst::WGS_J4 = -1.6109876e-6;

    // J22常数项
const double CSysConst::WGS_J22 = -1.08083e-6;

//太阳同步轨道最小倾角
const double CSysConst::ORB_SUNSYN_minINC = 1.681620301757153;

//轨道最小高度
const double CSysConst::ORB_minHeight = 200000.f;

//每天最多圈数
const double CSysConst::ORB_maxCir = 16.247497720492111;

//太阳同步轨道最大轨道高度
const double CSysConst::ORB_SunSyn_maxHeight = 5.981664732114194e+006f;

    // 角度和弧度之间转化的常数
const double CSysConst::ORB_PI180 = 0.0174532925199432957692367;

const double CSysConst::J2000D = 2451545.0;

// epoch B1950: 22:09 UT on 21 Dec 1949
const double CSysConst::B1950 = 2433282.423;

// Difference in seconds between Terrestrial Time and International
// Atomic Time
const double CSysConst::dTA = 32.184;

const double CSysConst::AsToR = 4.848136811095359936E-6;

//const Matrix3 CSysConst::ecef2ogre = Matrix3(0,0,1,1,0,0,0,1,0);
///*const Ogre::Quaternion CSysConst::ogre2j2000 = Ogre::Quaternion();*/
//
	const double CSysConst::DaySeconds = 86400.f;
//
//const Vector2 CSysConst::EndMark2D = Vector2(10000000000,10000000000);
//
//const Vector3 CSysConst::EndMark3D = Vector3(0,0,0);
//
//const Vector2 CSysConst::Vector2NULL = Vector2(0,0);
//
//const std::vector<Vector3> CSysConst::NULLVV3(3,EndMark3D);
//
//const UINT CSysConst::IteratorNo = 1000000;
const int CSysConst::MAX_TABLEROWCOUNT=200;

void CSysConst::llToMokatuo(double lon,double lat,float &x,float &y)
{
	//x = lon*CSysConst::ORB_PI180*CSysConst::ORB_R_Ee;

	//y=lat*CSysConst::ORB_PI/180;
	//y= CSysConst::ORB_R_Po / 2 * log((1.0 + sin(y)) / (1.0 - sin(y)));
	const double MERC_MINX = -20037508;
	// const double MERC_MINY = -20037508.34278925;
	//const double MERC_MINY = -10001966;
	const double MERC_MINY = -10018754.1714;
	const double MERC_MAXX = 20037508;
	// const double MERC_MAXY =  20037508.34278925;
	//const double MERC_MAXY = 10001966;
	const double MERC_MAXY = 10018754.1714;
	const double MERC_WIDTH = MERC_MAXX - MERC_MINX;
	const double MERC_HEIGHT = MERC_MAXY - MERC_MINY;
	//y =  log(tan((90 + lat) *CSysConst::ORB_PI / 360)) / (CSysConst::ORB_PI / 180);
	////y =  y * CSysConst::ORB_R_Ee*CSysConst::ORB_PI/ 180;

	//// double lon = osg::clampBetween(points[i].x(), -180.0, 180.0);
	//         //  double lat = osg::clampBetween(points[i].y(), -90.0, 90.0);
	double xr = (lon*CSysConst::ORB_PI180 + CSysConst::ORB_PI) / (2.0*CSysConst::ORB_PI);
	double yr = (lat*CSysConst::ORB_PI180 + 90 * CSysConst::ORB_PI180) / (180 * CSysConst::ORB_PI180);
	//double sinLat=sin(lat*CSysConst::ORB_PI180);
	/*if(lat>0)
	sinLat = sin((lat-5)*CSysConst::ORB_PI180);
	else
	sinLat = sin((lat+5)*CSysConst::ORB_PI180);*/
	// double oneMinusSinLat = 1-sinLat;
	/*if ( oneMinusSinLat != 0.0 )
	{*/
	// double yr = ((0.5 * log( (1+sinLat)/oneMinusSinLat )) +CSysConst::ORB_PI) / (2.0*+CSysConst::ORB_PI);
	x = osg::clampBetween(MERC_MINX + (xr * MERC_WIDTH), MERC_MINX, MERC_MAXX);
	y = osg::clampBetween(MERC_MINY + (yr* MERC_HEIGHT), MERC_MINY, MERC_MAXY);
}

void CSysConst::MokatuoToll(double x, double y, float &lon, float &lat)
{
	const double MERC_MINX = -CSysConst::ORB_PI;
	// const double MERC_MINY = -20037508.34278925;
	const double MERC_MINY = MERC_MINX / 2.0;
	const double MERC_MAXX = CSysConst::ORB_PI;
	// const double MERC_MAXY =  20037508.34278925;
	const double MERC_MAXY = CSysConst::ORB_PI / 2.0;
	const double MERC_WIDTH = MERC_MAXX - MERC_MINX;
	const double MERC_HEIGHT = MERC_MAXY - MERC_MINY;

	double xr = (x + 20037508) / (2.0 * 20037508);
	double yr = (y + 10018754.1714) / (2.0 * 10018754.1714);

	lon = osg::clampBetween(MERC_MINX + (xr * MERC_WIDTH), MERC_MINX, MERC_MAXX);
	lon = lon / CSysConst::ORB_PI180;
	lat = osg::clampBetween(MERC_MINY + (yr* MERC_HEIGHT), MERC_MINY, MERC_MAXY);
	lat = lat / CSysConst::ORB_PI180;
}

std::vector<std::string> CSysConst::vStringSplit(std::string& s, std::string delim )
{
	std::vector<std::string> elems;
	size_t pos = 0;
	size_t len = s.length();
	size_t delim_len = delim.length();
	if (delim_len == 0) return elems;
	while (pos < len)
	{
		int find_pos = s.find(delim, pos);
		if (find_pos < 0)
		{
			elems.push_back(s.substr(pos, len - pos));
			break;
		}
		elems.push_back(s.substr(pos, find_pos - pos));
		pos = find_pos + delim_len;
	}
	return elems;
}

void CSysConst::lonlatToMercator(double lon, double lat, double &mercx, double &mercy)
{

	mercx = lon * 20037508.34 / 180;
	mercy = std::log(std::tan((90 + lat) * CSysConst::ORB_PI / 360)) / (CSysConst::ORB_PI / 180);
	mercy = mercy * 20037508.34 / 180;
}

void CSysConst::mercatorTolonlat(double mercx, double mercy, double &lon, double &lat)
{

	lon = mercx / 20037508.34 * 180;
	lat = mercy / 20037508.34 * 180;

	lat = 180 / CSysConst::ORB_PI * (2 * std::atan(std::exp(lat*CSysConst::ORB_PI / 180)) - CSysConst::ORB_PI / 2);
}
