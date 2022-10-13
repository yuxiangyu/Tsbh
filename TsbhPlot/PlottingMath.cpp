#include "PlottingMath.h"

std::vector<osg::Vec2> Math::calculateVector(osg::Vec2 v, float a, float d)
{
	//����Ŀ��������ͷ��x����
	float x_1, x_2;
	//����Ŀ��������ͷ��y����
	float y_1, y_2;
	//����Ŀ��������һ��һ��
	osg::Vec2 v_l;
	osg::Vec2 v_r;

	//�����׼����v��ģ
	float d_v = sqrtf(v.x()*v.x() + v.y()* v.y());
	//��׼������б��Ϊ0ʱ��yֵ������Ϊ������������Ҫ�ر���
	if (v.y() == 0) {
		//����x,��������ֵ
		x_1 = x_2 = d_v * d * cosf(a) / v.x();
		//����v.x�������ж�Ŀ������������֮��
		if (v.x() > 0) {
			// ����y
			y_1 = sqrtf(d*d - x_1*x_1);
			y_2 = -y_1;
		}
		else if (v.x() < 0) {
			// ����y
			y_2 = sqrtf(d*d - x_1*x_1);
			y_1 = -y_2;
		}
		v_l.set(x_1, y_1);
		v_r.set(x_2, y_2);
	}
	else { //��Ϊ��������
		   //ת��Ϊy=nx+m��ʽ
		float n = -v.x() / v.y();
		float m = d*d_v*cosf(a) / v.y();
		//x*x + y*y = d*d
		//ת��Ϊa*x*x + b*x + c = 0
		float a = 1 + n*n;
		float b = 2 * n*m;
		float c = m*m - d*d;
		// ����x, ��������ֵ
		x_1 = (-b - sqrtf(b*b - 4 * a*c)) / (2 * a);
		x_2 = (-b + sqrtf(b*b - 4 * a*c)) / (2 * a);
		//����y
		y_1 = n*x_1 + m;
		y_2 = n*x_2 + m;

		//����������ʱ
		if (v.y() >= 0) {
			v_l.set(x_1, y_1);
			v_r.set(x_2, y_2);
		}
		else if (v.y() < 0) { //����������ʱ
			v_l.set(x_2, y_2);
			v_r.set(x_1, y_1);
		}
	}
	return std::vector<osg::Vec2>{v_l, v_r};
}

osg::Vec2 Math::calculateIntersection(osg::Vec2 v_1, osg::Vec2 v_2, osg::Vec2 point1, osg::Vec2 point2)
{
	//���彻�������
	float x, y;
	//�������v_1��v_2ƽ��
	if (v_1.y()*v_2.x() - v_1.x()*v_2.y() == 0) {
		//ƽ��Ҳ���������
		//ͬ��
		if (v_1.x()*v_2.x() > 0 || v_1.y()* v_2.y() > 0) {
			//ͬ��ֱ��ȡ��������е�
			x = (point1.x() + point2.x()) / 2;
			y = (point1.y() + point2.y()) / 2;
		}
		else { //����
			   //�������ֱ�ӷ��غ���ĵ�λ��
			x = point2.x();
			y = point2.y();
		}
	}
	else {
		x = (v_1.x()*v_2.x()*(point2.y() - point1.y()) + point1.x()*v_1.y()*v_2.x() - point2.x()*v_2.y()*v_1.x()) / (v_1.y()*v_2.x() - v_1.x()*v_2.y());
		if (v_1.x() != 0) {
			y = (x - point1.x())*v_1.y() / v_1.x() + point1.y();
		}
		else { //������v_1.x��v_2.xͬʱΪ0
			y = (x - point2.x())*v_2.y() / v_2.x() + point2.y();
		}
	}
	return osg::Vec2(x, y);
}

osg::Vec2 Math::calculateAngularBisector(osg::Vec2 v1, osg::Vec2 v2)
{
	//�����ƽ���ߵ�˼����ȡ���������ĵ�λ������Ȼ�����
	float d1 = sqrtf(v1.x()*v1.x() + v1.y()*v1.y());
	float d2 = sqrtf(v2.x()*v2.x() + v2.y()*v2.y());
	return osg::Vec2(v1.x() / d1 + v2.x() / d2, v1.y() / d1 + v2.y() / d2);
}

std::vector<osg::Vec2> Math::calculateIntersectionFromTwoCorner(osg::Vec2 pointS, osg::Vec2 pointE, float a_S, float a_E)
{
	//��ʼ�㡢�����㡢��������������㣬�γ�һ��������
	//б�ߣ���ʼ�㵽�����㣩������Ϊ
	osg::Vec2 v_SE(pointE.x() - pointS.x(), pointE.y() - pointS.y());
	//������ʼ�㡢����ĵ�λ����
	osg::Vec2 v_SI_l, v_SI_r;
	std::vector<osg::Vec2> v_SI_lr = calculateVector(v_SE, a_S, 1);
	v_SI_l = v_SI_lr[0];
	v_SI_r = v_SI_lr[1];

	//��������㡢����ĵ�λ����
	osg::Vec2 v_EI_l, v_EI_r;
	std::vector<osg::Vec2> v_EI_lr = calculateVector(v_SE, osg::PI - a_S, 1);
	v_EI_l = v_EI_lr[0];
	v_EI_r = v_EI_lr[1];

	//����ߵĽ���
	osg::Vec2 pointI_l = calculateIntersection(v_SI_l, v_EI_l, pointS, pointE);
	// �����ұߵĽ���
	osg::Vec2 pointI_r = calculateIntersection(v_SI_r, v_EI_r, pointS, pointE);
	return std::vector<osg::Vec2>{pointI_l, pointI_r};
}

void Math::inciseBezier(const std::vector<osg::Vec2> &pSrcPt, int j, std::vector<osg::Vec2> &pDstPt) {
	std::vector<osg::Vec2> buffer[3];
	int i;
	osg::Vec2 p;
	double p_x, p_y;
	for (i = 0; i < 3; i++) {
		p_x = (pSrcPt[j + i].x() + pSrcPt[j + i + 1].x()) / 2;
		p_y = (pSrcPt[j + i].y() + pSrcPt[j + i + 1].y()) / 2;
		p.set(p_x, p_y);
		buffer[0].push_back(p);
	}
	for (i = 0; i < 2; i++) {
		p_x = (buffer[0][i].x() + buffer[0][i + 1].x()) / 2;
		p_y = (buffer[0][i].y() + buffer[0][i + 1].y()) / 2;
		p.set(p_x, p_y);
		buffer[1].push_back(p);
	}
	buffer[2].push_back(osg::Vec2((buffer[1][0].x() + buffer[1][1].x()) / 2, (buffer[1][0].y() + buffer[1][1].y()) / 2));
	//��������ĸ����ֳ�7����
	pDstPt.push_back(pSrcPt[j]);
	pDstPt.push_back(buffer[0][0]);
	pDstPt.push_back(buffer[1][0]);
	pDstPt.push_back(buffer[2][0]);
	pDstPt.push_back(buffer[1][1]);
	pDstPt.push_back(buffer[0][2]);
	pDstPt.push_back(pSrcPt[j + 3]);
}

float Math::getBezierGap(std::vector<osg::Vec2> &pSrcPt, int j) {
	float gap = 0;
	for (int i = 1; i < 4; i++) {
		if (fabs(pSrcPt[j + i].x() - pSrcPt[j + i - 1].x()) > gap)
			gap = fabs(pSrcPt[j + i].x() - pSrcPt[j + i - 1].x());
		if (fabs(pSrcPt[j + i].y() - pSrcPt[j + i - 1].y()) > gap)
			gap = fabs(pSrcPt[j + i].y() - pSrcPt[j + i - 1].y());
	}
	return gap;
}

std::vector<osg::Vec2> Math::createBezier(const std::vector<osg::Vec2>& points, float precision, int part)
{
	if (part)
		return createBezier3(points, part);

	std::vector<osg::Vec2> bezierPts;
	for (int m = 0; m < points.size(); m++) {
		bezierPts.push_back(points[m]);
	}
	//��ȡ����������
	int i, k, j=0;	//��ʼ��j=0��add by gkk 20220216
	bool bExit;
	int count = bezierPts.size();
	std::vector<osg::Vec2> ptBuffer;
	bool ok = true;
	while (ok) {
		bExit = true;
		//�������ֽ��ǰ�4����Ϊһ����еģ�����С��4������ٲ����зֽ�
		for (int i = 0; i < count - 3; i += 3) {
			//�������������зֽ�
			//�ж�bezierPts[i]��bezierPts[i+4]�Ƿ�ﵽ����
			if (getBezierGap(bezierPts, i) > precision) {
				bExit = false;
				//��δ�ﵽ���ȵ�bezierPts[i]��bezierPts[i+4]���м��㣬�õ��µ�ptBuffer������
				inciseBezier(bezierPts, i, ptBuffer);
				//ȥ����ʹ�ù���2�����Ƶ�
				bezierPts.erase(bezierPts.begin() + i + 1, bezierPts.begin() + i + 1 + 2);
				//�����μ���õ���5���µĵ���뵽bezierPts[i]λ��֮�󣬵õ��µ�bezierPts������
				for (k = 0; k < 5; k++) {
					bezierPts.insert(bezierPts.begin() + i + 1 + k, ptBuffer[k + 1]);
				}
				//bezierPts[i]��bezierPts[i+4]û�дﵽ���ȣ����Բ���������i��ع��ʼ
				i -= 3;
				count = bezierPts.size();
			}
			if (bExit)
				break;
		}
		//�Էֽ�ó�����bezierPts����������Ż�����ȥ��ͬ�ĵ�
		while (j < count - 1) {
			if (bezierPts[j] == bezierPts[j + 1]) {
				bezierPts.erase(bezierPts.begin() + j + 1, bezierPts.begin() + j + 1 + 1);
				count--;
			}
			j++;
		}
		ok = false;
	}
	//���ֽ���ɵ��µ�bezierPts������ת��ΪGeoLine���󲢷���
	return bezierPts;
}

std::vector<osg::Vec2> Math::createBezier3(const std::vector<osg::Vec2>& points, int part) {
	//��ȡ����ֵĵ�
	std::vector<osg::Vec2> bezierPts;

	float scale = 0.05;
	if (part > 0) {
		scale = 1.0 / (float)part;
	}
	for (int i = 0; i < points.size() - 3; ) {
		//��ʼ��
		osg::Vec2 pointS = points[i];
		//��һ�����Ƶ�
		osg::Vec2 pointC1 = points[i + 1];
		//�ڶ������Ƶ�
		osg::Vec2 pointC2 = points[i + 2];
		//������
		osg::Vec2 pointE = points[i + 3];
		bezierPts.push_back(pointS);
		for (float t = 0.0; t < 1.0; ) {
			//���α��������߹�ʽ
			float x = (1 - t)*(1 - t)*(1 - t)*pointS.x() + 3 * t*(1 - t)*(1 - t)*pointC1.x() + 3 * t*t*(1 - t)*pointC2.x() + t*t*t*pointE.x();
			float y = (1 - t)*(1 - t)*(1 - t)*pointS.y() + 3 * t*(1 - t)*(1 - t)*pointC1.y() + 3 * t*t*(1 - t)*pointC2.y() + t*t*t*pointE.y();
			bezierPts.push_back(osg::Vec2(x, y));
			t += scale;
		}
		i += 3;
		if (i > points.size()) {
			bezierPts.push_back(pointS);
		}
	}
	//��Ҫ�ж�һ�����һ�����Ƿ����
	osg::Vec2 poRE = bezierPts[bezierPts.size() - 1];
	osg::Vec2 popE = points[points.size() - 1];
	if (poRE != popE) {
		bezierPts.push_back(popE);
	}
	return bezierPts;
}


//#include <osgModeling/Bezier>
//std::vector<osg::Vec2> Math::createBezier(std::vector<osg::Vec2> &points, float precision, int part)
//{

//    std::vector<osg::Vec2> bezierPts;
//    osg::ref_ptr<osg::Vec3Array> vecPoint = new  osg::Vec3Array;
//    for (auto& i = points.begin(); i != points.end(); i++) {
//        vecPoint->push_back(osg::Vec3(i->x(), i->y(), 0));
//    }
//    osg::ref_ptr<osgModeling::BezierCurve> bezierCurve = new osgModeling::BezierCurve(vecPoint.get());
//    osg::Vec3Array* bezierPath = bezierCurve->getPath();
//    for (auto& i = bezierPath->begin(); i != bezierPath->end(); i++) {
//        bezierPts.push_back(osg::Vec2(i->x(), i->y()));
//    }
//    return bezierPts;
//}

std::vector<osg::Vec2> Math::createBezier2(std::vector<osg::Vec2> &points, int part) {
	//��ȡ����ֵĵ�
	std::vector<osg::Vec2> bezierPts;
	float scale = 0.05;
	if (part > 0)
		scale = 1.0 / (float)part;
	if (points.size() == 2) {
		bezierPts.push_back(points[points.size() - 1]);
		return bezierPts;
	}
	for (int i = 0; i < points.size() - 2;) {
		//��ʼ��
		osg::Vec2 pointS = points[i];
		//���Ƶ�
		osg::Vec2 pointC = points[i + 1];
		//������
		osg::Vec2 pointE = points[i + 2];
		bezierPts.push_back(pointS);
		for (float t = 0; t < 1.0; ) {
			//���α��������߹�ʽ
			float x = (1 - t)*(1 - t)*pointS.x() + 2 * t*(1 - t)*pointC.x() + t*t*pointE.x();
			float y = (1 - t)*(1 - t)*pointS.y() + 2 * t*(1 - t)*pointC.y() + t*t*pointE.y();
			bezierPts.push_back(osg::Vec2(x, y));
			t += scale;
		}
		i += 2;
		if (i > points.size()) {
			bezierPts.push_back(pointS);
		}
	}
	// ��Ҫ�ж�һ�����һ�����Ƿ����
	osg::Vec2 poRE = bezierPts[bezierPts.size() - 1];
	osg::Vec2 popE = points[points.size() - 1];
	if (poRE != popE) {
		bezierPts.push_back(popE);
	}
	return bezierPts;
}

std::vector<osg::Vec2> Math::createCloseCardinal(std::vector<osg::Vec2> &points)
{
	if (points.empty() || points.size() < 3) {
		return points;
	}
	//��ȡ��㣬��Ϊ�յ㣬�Ապ����ߡ�
	osg::Vec2 lastP = points[0];
	points.push_back(lastP);

	//���崫��ĵ����飬���ڵ��������루ÿ�����㣩�����������Ƶ�
	std::vector<osg::Vec2> cPoints = points;
	//���������Ϳ��Ƶ������
	std::vector<osg::Vec2> cardinalPoints;
	cardinalPoints.resize(256);

	//��������������
	//��Щ����������ϲ���ľ�����ֵ
	//��������ϵ����ȡֵ��0<t<0.5
	float t = 0.4;
	//Ϊ�˵�����ϵ�����ӣ�ȡֵ��0<b<1
	float b = 0.5;
	//�����ƣ���һ�����ڵ���0��������������ǳ�������һ��ֱ��ʱ�����ټ�����
	float e = 0.005;

	//����ĵ�������������������n����Ϊ2
	int n = cPoints.size() - 1;
	//�ӿ�ʼ�����������ڶ��������е����ڶ������ڼ�����㣨�յ㣩�Ĳ�ֵ���Ƶ�
	osg::Vec2 p0, p1, p2;
	for (int k = 0; k <= n - 1; k++) {
		//������㣨�յ㣩�����ҿ��Ƶ�

		if (k == n - 1) {
			//�������������
			p0 = cPoints[n - 1];
			p1 = cPoints[0];
			p2 = cPoints[1];
		}
		else {
			p0 = cPoints[k];
			p1 = cPoints[k + 1];
			p2 = cPoints[k + 2];
		}

		//����p1������Ƶ���ҿ��Ƶ�
		osg::Vec2 p1l, p1r;
		//ͨ��p0��p1��p2����p1��������Ƶ�p1l���ֿ��Ƶ�p1r
		//��������p0_p1��p1_p2
		osg::Vec2 p0_p1(p1.x() - p0.x(), p1.y() - p0.y());
		osg::Vec2 p1_p2(p2.x() - p1.x(), p2.y() - p1.y());
		//������ģ
		float d01 = sqrtf(p0_p1.x() * p0_p1.x() + p0_p1.y() * p0_p1.y());
		float d12 = sqrtf(p1_p2.x() * p1_p2.x() + p1_p2.y() * p1_p2.y());
		//������λ��
		osg::Vec2 p0_p1_1(p0_p1.x() / d01, p0_p1.y() / d01);
		osg::Vec2 p1_p2_1(p1_p2.x() / d12, p1_p2.y() / d12);
		//��������p0_p1��p1_p2�ļн�ƽ��������
		osg::Vec2 p0_p1_p2(p0_p1_1.x() + p1_p2_1.x(), p0_p1_1.y() + p1_p2_1.y());
		//�������� p0_p1_p2 ��ģ
		float d012 = sqrtf(p0_p1_p2.x() * p0_p1_p2.x() + p0_p1_p2.y() * p0_p1_p2.y());
		//��λ������p0_p1_p2
		osg::Vec2 p0_p1_p2_1(p0_p1_p2.x() / d012, p0_p1_p2.y() / d012);
		//�ж�p0��p1��p2�Ƿ��ߣ������ж�����p0_p1��p1_p2�ļнǵ����Һ�1�Ĳ�ֵС��e����Ϊ���㹲��
		float cosE_p0p1p2 = (p0_p1_1.x() * p1_p2_1.x() + p0_p1_1.y() * p1_p2_1.y()) / 1.0;
		if (fabs(1.0 - cosE_p0p1p2) < e) {
			//����p1l������
			p1l.x() = p1.x() - p1_p2_1.x() * d01 * t;
			p1l.y() = p1.y() - p1_p2_1.y() * d01 * t;
			//����p1r������
			p1r.x() = p1.x() + p0_p1_1.x() * d12 * t;
			p1r.y() = p1.y() + p0_p1_1.y() * d12 * t;
		}
		else { //�ǹ���
			   //����p1l������
			p1l.x() = p1.x() - p0_p1_p2_1.x() * d01 * t;
			p1l.y() = p1.y() - p0_p1_p2_1.y() * d01 * t;
			//����p1r������
			p1r.x() = p1.x() + p0_p1_p2_1.x() * d12 * t;
			p1r.y() = p1.y() + p0_p1_p2_1.y() * d12 * t;
		}

		//��¼��㣨�յ㣩�����Ҳ�ֵ���Ƶ㼰�����ڶ������Ƶ�
		if (k == n - 1) {
			cardinalPoints[0] = p1;
			cardinalPoints[1] = p1r;
			cardinalPoints[(n - 2) * 3 + 2 + 3] = p1l;
			cardinalPoints[(n - 2) * 3 + 2 + 4] = cPoints[n];
		}
		else {
			//��¼�����������Ƶ�
			cardinalPoints[k * 3 + 2 + 0] = p1l;
			cardinalPoints[k * 3 + 2 + 1] = p1;
			cardinalPoints[k * 3 + 2 + 2] = p1r;
		}
	}
	//���˵���ֵ�ĵ�
#if 0    //N by gkk 20220223 ,�����ǳ�������ֻ�ܰ󶨵���ֵ������
	for (auto& it = cardinalPoints.begin(); it != cardinalPoints.end();) {
		if (*it == osg::Vec2())
			it = cardinalPoints.erase(it);
		else
			++it;
	}
#endif
	for (auto it = cardinalPoints.begin(); it != cardinalPoints.end();) {
		if (*it == osg::Vec2())
			it = cardinalPoints.erase(it);
		else
			++it;
	}

	return cardinalPoints;
}

osg::Vec2 Math::calculateMidpoint(const osg::Vec2 &pointA, const osg::Vec2 &pointB)
{
	return osg::Vec2((pointA.x() + pointB.x()) / 2, (pointA.y() + pointB.y()) / 2);
}

float Math::calculateDistance(const osg::Vec2 &pointA, const osg::Vec2 &pointB)
{
	return sqrtf(powf(pointA.x() - pointB.x(), 2) + powf(pointA.y() - pointB.y(), 2));
}

std::vector<osg::Vec2> Math::calculateArc(const osg::Vec2& center, float radius, float startAngle, float endAngle, float direction, float sides)
{
	if (!direction || (direction != 1 && direction != -1))
		direction = -1;
	//    if(!sides)
	//        sides=360;
	float step = osg::PI / sides / 2.0;
	float stepDir = step*direction;
	float length = fabs(endAngle - startAngle);
	std::vector<osg::Vec2> points;
	for (float radians = startAngle, i = 0.0; i < length; i += step) {
		osg::Vec2 circlePoint(cosf(radians) * radius + center.x(), sinf(radians) * radius + center.y());
		points.push_back(circlePoint);
		radians += stepDir;
		radians = radians < 0 ? (radians + 2 * osg::PI) : radians;
		radians = radians > 2 * osg::PI ? (radians - 2 * osg::PI) : radians;
	}
	return points;
}

float Math::calculateAngle(const osg::Vec2 &pointA, const osg::Vec2 &centerPoint) {
	float angle = atan2f((pointA.y() - centerPoint.y()), (pointA.x() - centerPoint.x()));
	if (angle < 0) {
		angle += 2 * osg::PI;
	}
	return angle;
}

Math::MultiLineString Math::calculateArrowLines(const osg::Vec2 &startP, const osg::Vec2 &endP, float ratio, float angle) {
	MultiLineString arrowLines;
	float dictance = calculateDistance(startP, endP);
	osg::Vec2 vector = startP - endP;
	LineString vectorArrows = calculateVector(vector, angle, dictance / ratio);
	osg::Vec2 arrowLineP_l(vectorArrows[0].x() + endP.x(), vectorArrows[0].y() + endP.y());
	osg::Vec2 arrowLineP_r(vectorArrows[1].x() + endP.x(), vectorArrows[1].y() + endP.y());
	arrowLines.push_back(LineString{ endP, arrowLineP_l });
	arrowLines.push_back(LineString{ endP, arrowLineP_r });
	return arrowLines;
}
