#pragma once
#include "rtsscore_global.h"
#include <string>

class RTSSCORE_EXPORT zLayer 
{
public:
	zLayer();

	~zLayer(void);
	//图层名
	std::string name;
	//路径
	std::string filePath;
	//图层类型0:wms,1:正射影像，2:dem, 3:倾斜模型，4:3dtiles, 5:shp, 6:wmts, 7: 本地瓦片xyz格式
	int layerType;
	//应用类型
	int applyType;
	//是否展示
	bool showing;
	//渲染类型0为三维，1为2维
	int type;
	//服务图层名
	std::string layers;
	//bool isFirst;
	void setShowing(bool show);
	//void setIsFirst(bool isFirst);
	virtual void show();

};

