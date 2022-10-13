#ifndef PLOTTINGMATH_H
#define PLOTTINGMATH_H

#include <math.h>
#include <osg/Math>
#include <osg/Vec2>
#include <osg/Vec3>
#include <vector>
#include <list>

namespace Math {
	/**
	* ����ͻ�׼����v�н�Ϊa������Ϊd��Ŀ����������������������һ��һ�ң�
	* @param v ��׼����
	* @param a Ŀ�������ͻ�׼�����ļнǣ�Ĭ��Ϊ90�ȣ�����ĵ�λʹ�û���
	* @param d Ŀ�������ĳ��ȣ���ģ��Ĭ��Ϊ1������λ����
	* @return ����Ŀ���������飨������������һ��һ�ң�
	*/
	std::vector<osg::Vec2> calculateVector(osg::Vec2 v, float a = osg::PI_2, float d = 1.0);
	/**
	* ��������ֱ�ߵĽ���
	* ͨ��������˼����м��㣬��Ҫ�ṩ���������Լ�����ֱ���ϸ���һ����
	* @param v_1   ֱ��1������
	* @param v_2   ֱ��2������
	* @param points1   ֱ��1�ϵ�����һ��
	* @param points2   ֱ��2�ϵ�����һ��
	* @return ���ؽ���
	*/
	osg::Vec2 calculateIntersection(osg::Vec2 v_1, osg::Vec2 v_2, osg::Vec2 point1, osg::Vec2 point2);

	/**
	* �������������Ľ�ƽ��������
	* @param v1 ����1
	* @param v2 ����2
	* @return ���ؽ�ƽ��������
	*/
	osg::Vec2 calculateAngularBisector(osg::Vec2 v1, osg::Vec2 v2);

	/**
	* ͨ�������εĵױ����˵������Լ��ױ����нǣ����������������
	* @param pointS �ױߵ�һ����
	* @param pointE �ױߵڶ�����
	* @param a_S �ױߺ͵�һ�������ڵ���һ���ߵļн�
	* @param a_E �ױߺ͵ڶ��������ڵ���һ���ߵļн�
	* @return ���ض��㣨�����ϴ�������ֵ��
	*/
	std::vector<osg::Vec2> calculateIntersectionFromTwoCorner(osg::Vec2 pointS, osg::Vec2 pointE, float a_S, float a_E);
	/**
	* @brief inciseBezier
	* @param pSrcPt
	* @param j
	* @param pDstPt
	*/
	void inciseBezier(const std::vector<osg::Vec2>& pSrcPt, int j, std::vector<osg::Vec2>& pDstPt);
	/**
	* @brief getBezierGap
	* @param pSrcPt
	* @param j
	* @return
	*/
	float getBezierGap(std::vector<osg::Vec2>& pSrcPt, int j);
	/**
	* @brief createBezier
	* @param points
	* @param precision
	* @param part
	* @return
	*/
	std::vector<osg::Vec2> createBezier(const std::vector<osg::Vec2>& points, float precision = 0, int part = 20);
	/**
	* @brief createBezier2
	* @param points
	* @param part
	* @return
	*/
	std::vector<osg::Vec2> createBezier2(std::vector<osg::Vec2>& points, int part = 20);
	/**
	* @brief createBezier3
	* @param points
	* @param part
	* @return
	*/
	std::vector<osg::Vec2> createBezier3(const std::vector<osg::Vec2>& points, int part = 20);

	/**
	* Method: createCloseCardinal
	* �����պ�Cardinal�Ŀ��Ƶ㡣
	* ��������ĵ�����������Ӧ��Cardinal���Ƶ㣬��ʹ�ñ���������3������������Cardinal���Ƶ��Բ���պ����ߡ�
	*
	* Parameters:
	* points -{Array(<SuperMap.Geometry.Point>)} ����Ĵ�����ĳ�ʼ�㴮��
	* Returns:
	* {Array(<SuperMap.Geometry.Point>)} �������Ӧ��Cardinal���Ƶ㡣
	*
	* (code)
	* var points = [];
	* points.push(new SuperMap.Geometry.Point(-50,30));
	* points.push(new SuperMap.Geometry.Point(-30,50));
	* points.push(new SuperMap.Geometry.Point(2,60));
	* points.push(new SuperMap.Geometry.Point(8,20));
	*
	* var cardinal = SuperMap.Geometry.LineString.createCloseCardinal(points);
	* (end)
	*/

	std::vector<osg::Vec2> createCloseCardinal(std::vector<osg::Vec2>& points);

	/**
	* Method: calculateMidpoint
	* ���������������ɵ��߶εĵ��е�
	*
	* Parameters:
	* pointA - {<SuperMap.Geometry.Point>} ��һ����
	* pointB -  {<SuperMap.Geometry.Point>} �ڶ�����
	*
	* Returns:
	* {<SuperMap.Geometry.Point>} �����е�
	*/

	osg::Vec2 calculateMidpoint(const osg::Vec2& pointA, const osg::Vec2& pointB);

	/**
	* Method: calculateDistance
	* ���������ľ���
	*
	* Parameters:
	* pointA - {<SuperMap.Geometry.Point>} ��һ����
	* pointB -  {<SuperMap.Geometry.Point>} �ڶ�����
	*
	* Returns:
	* {<SuperMap.Geometry.Point>} ���������ľ���ֵ
	*/
	float calculateDistance(const osg::Vec2& pointA, const osg::Vec2& pointB);

	/**
	* Method: calculateArc
	* ����Բ�ġ��뾶����X������Ǻ��յ�Ǽ���Բ����
	*
	* Parameters:
	* center - {<SuperMap.Geometry.Point>} Բ��
	* radius - {Number}�뾶
	* startAngle - {Number}����,��ΧΪ0��2�С�
	* endAngle - {Number}�յ��,��ΧΪ0��2�С�
	* direction - {Number}����㵽�յ�ķ�����ֵΪ1����ʱ�룬��ֵΪ-1��˳ʱ�롣Ĭ��Ϊ1������ʱ�롣
	* sides - {Number}Բ������Բ�ĵ�����Ĭ��Ϊ360������ÿ1��һ���㡣
	*
	* Returns:
	* {Array(<SuperMap.Geometry.Point>)} Բ���ϵĵ�����
	*/

	std::vector<osg::Vec2> calculateArc(const osg::Vec2& center, float radius, float startAngle, float endAngle, float direction, float sides = 360);

	/**
	* Method: calculateAngle
	* ����Բ��һ�����ڰ뾶��ֱ����X��ļнǣ�����Ի�����ʽ��ʾ����Χ��+�е� +2�С�
	*/
	float calculateAngle(const osg::Vec2& pointA, const osg::Vec2& centerPoint);

	typedef std::vector<osg::Vec2> LineString;
	typedef std::vector<LineString> MultiLineString;

	/**
	* Method: calculateArrowLines
	* ����������������������ļ�ͷ��������ֱ�ߣ�
	*
	* Parameters:
	* startP - {<SuperMap.Geometry.Point>} ���������
	* endP -  {<SuperMap.Geometry.Point>} �������յ�
	* ratio -  {Number} ֱ�߳������ͷ���ȵı�ֵ��Ĭ��Ϊ10��
	* angle - {Number} ��ͷ����ֱ����ֱ�ߵļнǣ���ΧΪ0���У�Ĭ��ΪMath.PI/6��
	*
	* Returns:
	* {Array(<SuperMap.Geometry.LineString>)} �����е�
	*/

	MultiLineString calculateArrowLines(const osg::Vec2& startP, const osg::Vec2& endP, float ratio = 10, float angle = osg::PI / 6);

} // namespace Math


#endif
