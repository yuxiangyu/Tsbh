#ifndef SYSCONST_H_HEADER_INCLUDED_B5107351
#define SYSCONST_H_HEADER_INCLUDED_B5107351
//**********************************************************
//ϵͳ����
//**********************************************************
//#include "Ogre.h"

#include "rtsscore_global.h"
#include <string>
#include <vector>

class  RTSSCORE_EXPORT CSysConst
{
public:
	//static const std::string IP;
	// ������������
	static const double ORB_MIU;

	// ������תƽ�����ٶ�
	static const double ORB_OMEGA_E;
		
	//����ֹ����볤��
	static const double ORB_Eth_A;

	// ����תƽ�����ٶ�
	static const double ORB_OMEGA_Er;

	// �Ƴཻ�ǣ���λΪ����
	static const double ORB_I_ECLIP;

	// �������뾶
	static const double ORB_R_Ee;

	//##ModelId=4AEFF541035B
	static const double ORB_R_Em;
	//���뾶
	static const double ORB_R_Po;

	// ̫���ƾ�ƽ���仯��
	static const double ORB_W_LAMDA;

	// ����еĹ���
	static const double ORB_c;

	// ��ƽ�洦��������
	static const double ORB_g;

	// Բ����
	static const double ORB_PI;

	//2��Բ����
	static const double ORB_2PI;

	//�ٽ���ǹ��������
	static const double Inc_Critical;

	// ��������������
	static const double ORB_TOLERC;

	// ̫��ͬ�����ƽ���������ٶ�
	static const double ORB_MANGLVEL;

	// J2������
	static const double WGS_J2;

	// J3������
	static const double WGS_J3;

	// J4������
	static const double WGS_J4;

	// J22������
	//##ModelId=4AEFF5420128
	static const double WGS_J22;

	//̫��ͬ�������С���
	static const double ORB_SUNSYN_minINC;

	//�����С�߶�
	static const double ORB_minHeight;

	//��С�߶ȶ�Ӧÿ�����Ȧ��
	static const double ORB_maxCir;

	//̫��ͬ�����������߶�
	static const double ORB_SunSyn_maxHeight;

	// �ǶȺͻ���֮��ת���ĳ���
	static const double ORB_PI180;

	//��ԪJ2000����������
	static const double J2000D;

	//��ԪB1950����������
	static const double B1950;

	//���Ĵ�½ʱ�͹���ԭ��ʱ��ʱ���
	static const double dTA;

	//���뻯��������
	static const double AsToR;

	//����һ���������ϵ��OGRE��������ϵ����ת����
	//static const Ogre::Matrix3 ecef2ogre;

	//ÿ�������
	static const double DaySeconds;

	////��־�����Ľ���
	//static const Ogre::Vector2 EndMark2D;
	//static const Ogre::Vector3 EndMark3D;

	////��������
	//static const UINT			IteratorNo;

	////������
	//static const std::vector<Ogre::Vector3> NULLVV3;

	//static const Ogre::Vector2 Vector2NULL;
	static const int MAX_TABLEROWCOUNT;
	//��γ�ȵ�ī��������ת��
	static void llToMokatuo(double lon,double lat,float &x,float &y);
	static void MokatuoToll(double x, double y, float &lon, float &lat);
	static void lonlatToMercator(double lon, double lat, double &mercx, double &mercy);
	static void mercatorTolonlat(double mercx, double mercy, double &lon, double &lat);
	static std::vector<std::string> vStringSplit(std::string& s,std::string delim);
};
// namespace CSTK
#endif /* SYSCONST_H_HEADER_INCLUDED_B5107351 */