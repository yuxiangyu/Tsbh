#pragma once
#include "rtsscore_global.h"
#include <string>

class RTSSCORE_EXPORT zLayer 
{
public:
	zLayer();

	~zLayer(void);
	//ͼ����
	std::string name;
	//·��
	std::string filePath;
	//ͼ������0:wms,1:����Ӱ��2:dem, 3:��бģ�ͣ�4:3dtiles, 5:shp, 6:wmts, 7: ������Ƭxyz��ʽ
	int layerType;
	//Ӧ������
	int applyType;
	//�Ƿ�չʾ
	bool showing;
	//��Ⱦ����0Ϊ��ά��1Ϊ2ά
	int type;
	//����ͼ����
	std::string layers;
	//bool isFirst;
	void setShowing(bool show);
	//void setIsFirst(bool isFirst);
	virtual void show();

};

