#include "ToolProperty.h"
#include <QDebug>

ToolProperty::ToolProperty()
{
}

ToolProperty::~ToolProperty()
{
}

void ToolProperty::setNodeStyle(osgEarth::Style style)
{
	m_NodeStyle = style;
}

void ToolProperty::setToolName(QString name)
{
	m_name = name;
}

void ToolProperty::setToolType(ToolType type)
{
	m_type = type;
}

void ToolProperty::setNodeobjectId(unsigned int id)
{
	m_objectid = id;
}

//void ToolProperty::setNodeInfos(NodeInfo info)
//{
//	m_info = info;
//}

void ToolProperty::getStyleXX(int & r, int & g, int & b, int & a)
{
	//
	if (ToolType::LINE == m_type) {////��������
		osgEarth::Color osgColor = m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->color();
		//
		//qDebug() << "zx_ r:" << osgColor.r() << "g:" << osgColor.g() << "b:" << osgColor.b() << "a:" << osgColor.a();
		//
		r = int(osgColor.r() * 255) % 256;
		g = int(osgColor.g() * 255) % 256;
		b = int(osgColor.b() * 255) % 256;
		a = int(osgColor.a() * 99) % 100;
	}
	else if (ToolType::POLYGON == m_type) {////���ƶ����
		osgEarth::Color osgColor = m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->color();
		//
		//qDebug() << "dbx_r:" << osgColor.r() << "g:" << osgColor.g() << "b:" << osgColor.b() << "a:" << osgColor.a();
		//
		r = int(osgColor.r() * 255) % 256;
		g = int(osgColor.g() * 255) % 256;
		b = int(osgColor.b() * 255) % 256;
		a = int(osgColor.a() * 99) % 100;
	}
	else if (ToolType::CIRCLE == m_type) {////����Բ��
		osgEarth::Color osgColor = m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->color();
		//
		//qDebug() << "y_r:" << osgColor.r() << "g:" << osgColor.g() << "b:" << osgColor.b() << "a:" << osgColor.a();
		//
		r = int(osgColor.r() * 255) % 256;
		g = int(osgColor.g() * 255) % 256;
		b = int(osgColor.b() * 255) % 256;
		a = int(osgColor.a() * 99) % 100;
	}
	else if (ToolType::RECTANGLE == m_type) {////���ƾ���
		osgEarth::Color osgColor = m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->color();
		//
		//qDebug() << "jx_r:" << osgColor.r() << "g:" << osgColor.g() << "b:" << osgColor.b() << "a:" << osgColor.a();
		//
		r = int(osgColor.r() * 255) % 256;
		g = int(osgColor.g() * 255) % 256;
		b = int(osgColor.b() * 255) % 256;
		a = int(osgColor.a() * 99) % 100;
	}
	else if (ToolType::TEXT == m_type) {////��������
		osgEarth::Color osgColor = m_NodeStyle.getOrCreate<osgEarth::TextSymbol>()->halo()->color();
		//
		//qDebug() << "wz_r:" << osgColor.r() << "g:" << osgColor.g() << "b:" << osgColor.b() << "a:" << osgColor.a();
		//
		r = int(osgColor.r() * 255) % 256;
		g = int(osgColor.g() * 255) % 256;
		b = int(osgColor.b() * 255) % 256;
		a = int(osgColor.a() * 99) % 100;
	}
	else if (ToolType::IMAGE == m_type) {////����ͼƬ
		osgEarth::Color osgColor = m_NodeStyle.getOrCreate<osgEarth::TextSymbol>()->halo()->color();
		//
		//qDebug() << "tp_r:" << osgColor.r() << "g:" << osgColor.g() << "b:" << osgColor.b() << "a:" << osgColor.a();
		//
		r = int(osgColor.r() * 255) % 256;
		g = int(osgColor.g() * 255) % 256;
		b = int(osgColor.b() * 255) % 256;
		a = int(osgColor.a() * 99) % 100;
	}
	else if (ToolType::GEOGETHER == m_type)
	{//���ƾۺ���
		osgEarth::Color osgColor = m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->color();
		//
		//qDebug() << "tp_r:" << osgColor.r() << "g:" << osgColor.g() << "b:" << osgColor.b() << "a:" << osgColor.a();
		//
		r = int(osgColor.r() * 255) % 256;
		g = int(osgColor.g() * 255) % 256;
		b = int(osgColor.b() * 255) % 256;
		a = int(osgColor.a() * 99) % 100;
	}
	else if (ToolType::GEOPOSITION == m_type)
	{//��λ
		osgEarth::Color osgColor = m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->color();
		//
		//qDebug() << "tp_r:" << osgColor.r() << "g:" << osgColor.g() << "b:" << osgColor.b() << "a:" << osgColor.a();
		//
		r = int(osgColor.r() * 255) % 256;
		g = int(osgColor.g() * 255) % 256;
		b = int(osgColor.b() * 255) % 256;
		a = int(osgColor.a() * 99) % 100;
	}
	else {
		r = 0;
		g = 0;
		b = 0;
		a = 0;
	}
}

void ToolProperty::setStyleXX(int r, int g, int b, int a)
{
	//
	if (ToolType::LINE == m_type) {////��������
		m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->color() = osgEarth::Color(
			r / 255.0,
			g / 255.0,
			b / 255.0,
			a / 99.0
		);
	}
	else if (ToolType::POLYGON == m_type) {////���ƶ����
		m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->color() = osgEarth::Color(
			r / 255.0,
			g / 255.0,
			b / 255.0,
			a / 99.0
		);
	}
	else if (ToolType::CIRCLE == m_type) {////����Բ��
		m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->color() = osgEarth::Color(
			r / 255.0,
			g / 255.0,
			b / 255.0,
			a / 99.0
		);
	}
	else if (ToolType::RECTANGLE == m_type) {////���ƾ���
		m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->color() = osgEarth::Color(
			r / 255.0,
			g / 255.0,
			b / 255.0,
			a / 99.0
		);
	}
	else if (ToolType::TEXT == m_type) {////��������
		m_NodeStyle.getOrCreate<osgEarth::TextSymbol>()->halo()->color() = osgEarth::Color(
			r / 255.0,
			g / 255.0,
			b / 255.0,
			a / 99.0
		);
	}
	else if (ToolType::IMAGE == m_type) {////����ͼƬ
		m_NodeStyle.getOrCreate<osgEarth::TextSymbol>()->halo()->color() = osgEarth::Color(
			r / 255.0,
			g / 255.0,
			b / 255.0,
			a / 99.0
		);
	}
	else if (ToolType::GEOGETHER == m_type) {////���ƾۺ���
		m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->color() = osgEarth::Color(
			r / 255.0,
			g / 255.0,
			b / 255.0,
			a / 99.0
		);
	}
	else if (ToolType::GEOPOSITION == m_type) {////���Ƶ�λ
		m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->color() = osgEarth::Color(
			r / 255.0,
			g / 255.0,
			b / 255.0,
			a / 99.0
		);
	}
	else {
		;//
	}
}

void ToolProperty::getStyleTCS(int& r, int& g, int& b, int& a)
{
	//
	if (ToolType::LINE == m_type) {////��������
		osgEarth::Color osgColor = m_NodeStyle.getOrCreate<osgEarth::PointSymbol>()->fill()->color();
		//
		qDebug() << "zx_ r:" << osgColor.r() << "g:" << osgColor.g() << "b:" << osgColor.b() << "a:" << osgColor.a();
		//
		r = int(osgColor.r() * 255) % 256;
		g = int(osgColor.g() * 255) % 256;
		b = int(osgColor.b() * 255) % 256;
		a = int(osgColor.a() * 99) % 100;
	}
	else if (ToolType::POLYGON == m_type) {////���ƶ����
		osgEarth::Color osgColor = m_NodeStyle.getOrCreate<osgEarth::PolygonSymbol>()->fill()->color();
		//
		qDebug() << "dbx_r:" << osgColor.r() << "g:" << osgColor.g() << "b:" << osgColor.b() << "a:" << osgColor.a();
		//
		r = int(osgColor.r() * 255) % 256;
		g = int(osgColor.g() * 255) % 256;
		b = int(osgColor.b() * 255) % 256;
		a = int(osgColor.a() * 99) % 100;
	}
	else if (ToolType::CIRCLE == m_type) {////����Բ��
		osgEarth::Color osgColor = m_NodeStyle.getOrCreate<osgEarth::PolygonSymbol>()->fill()->color();
		//
		qDebug() << "y_r:" << osgColor.r() << "g:" << osgColor.g() << "b:" << osgColor.b() << "a:" << osgColor.a();
		//
		r = int(osgColor.r() * 255) % 256;
		g = int(osgColor.g() * 255) % 256;
		b = int(osgColor.b() * 255) % 256;
		a = int(osgColor.a() * 90) % 100;
	}
	else if (ToolType::RECTANGLE == m_type) {////���ƾ���
		osgEarth::Color osgColor = m_NodeStyle.getOrCreate<osgEarth::PolygonSymbol>()->fill()->color();
		//
		qDebug() << "jx_r:" << osgColor.r() << "g:" << osgColor.g() << "b:" << osgColor.b() << "a:" << osgColor.a();
		//
		r = int(osgColor.r() * 255) % 256;
		g = int(osgColor.g() * 255) % 256;
		b = int(osgColor.b() * 255) % 256;
		a = int(osgColor.a() * 90) % 100;
	}
	else if (ToolType::TEXT == m_type) {////��������
		osgEarth::Color osgColor = m_NodeStyle.getOrCreate<osgEarth::TextSymbol>()->fill()->color();
		//
		qDebug() << "wz_r:" << osgColor.r() << "g:" << osgColor.g() << "b:" << osgColor.b() << "a:" << osgColor.a();
		//
		r = int(osgColor.r() * 255) % 256;
		g = int(osgColor.g() * 255) % 256;
		b = int(osgColor.b() * 255) % 256;
		a = int(osgColor.a() * 99) % 100;
	}
	else if (ToolType::IMAGE == m_type) {////����ͼƬ
		osgEarth::Color osgColor = m_NodeStyle.getOrCreate<osgEarth::TextSymbol>()->fill()->color();
		//
		qDebug() << "tp_r:" << osgColor.r() << "g:" << osgColor.g() << "b:" << osgColor.b() << "a:" << osgColor.a();
		//
		r = int(osgColor.r() * 255) % 256;
		g = int(osgColor.g() * 255) % 256;
		b = int(osgColor.b() * 255) % 256;
		a = int(osgColor.a() * 90) % 100;
	}
	else if (ToolType::GEOGETHER == m_type) {////���ƾۺ���
		osgEarth::Color osgColor = m_NodeStyle.getOrCreate<osgEarth::PolygonSymbol>()->fill()->color();
		//
		qDebug() << "tp_r:" << osgColor.r() << "g:" << osgColor.g() << "b:" << osgColor.b() << "a:" << osgColor.a();
		//
		r = int(osgColor.r() * 255) % 256;
		g = int(osgColor.g() * 255) % 256;
		b = int(osgColor.b() * 255) % 256;
		a = int(osgColor.a() * 99) % 100;
	}
	else if (ToolType::GEOPOSITION == m_type) {////���Ƶ�λ
		osgEarth::Color osgColor = m_NodeStyle.getOrCreate<osgEarth::PolygonSymbol>()->fill()->color();
		//
		qDebug() << "tp_r:" << osgColor.r() << "g:" << osgColor.g() << "b:" << osgColor.b() << "a:" << osgColor.a();
		//
		r = int(osgColor.r() * 255) % 256;
		g = int(osgColor.g() * 255) % 256;
		b = int(osgColor.b() * 255) % 256;
		a = int(osgColor.a() * 99) % 100;
	}
	else {
		r = 0;
		g = 0;
		b = 0;
		a = 0;
	}
}

void ToolProperty::setStyleTCS(int r, int g, int b, int a)
{
	//
	if (ToolType::LINE == m_type) {////��������
		m_NodeStyle.getOrCreate<osgEarth::PointSymbol>()->fill()->color() = osgEarth::Color(
			r / 255.0,
			g / 255.0,
			b / 255.0,
			a / 99.0
		);
	}
	else if (ToolType::POLYGON == m_type) {////���ƶ����
		m_NodeStyle.getOrCreate<osgEarth::PolygonSymbol>()->fill()->color() = osgEarth::Color(
			r / 255.0,
			g / 255.0,
			b / 255.0,
			a / 99.0
		);
	}
	else if (ToolType::CIRCLE == m_type) {////����Բ��
		m_NodeStyle.getOrCreate<osgEarth::PolygonSymbol>()->fill()->color() = osgEarth::Color(
			r / 255.0,
			g / 255.0,
			b / 255.0,
			a / 99.0
		);
	}
	else if (ToolType::RECTANGLE == m_type) {////���ƾ���
		m_NodeStyle.getOrCreate<osgEarth::PolygonSymbol>()->fill()->color() = osgEarth::Color(
			r / 255.0,
			g / 255.0,
			b / 255.0,
			a / 99.0
		);
	}
	else if (ToolType::TEXT == m_type) {////��������
		m_NodeStyle.getOrCreate<osgEarth::TextSymbol>()->fill()->color() = osgEarth::Color(
			r / 255.0,
			g / 255.0,
			b / 255.0,
			a / 99.0
		);
	}
	else if (ToolType::IMAGE == m_type) {////����ͼƬ
		m_NodeStyle.getOrCreate<osgEarth::TextSymbol>()->fill()->color() = osgEarth::Color(
			r / 255.0,
			g / 255.0,
			b / 255.0,
			a / 99.0
		);
	}
	else if (ToolType::GEOGETHER == m_type) {////���ƾۺ���
		m_NodeStyle.getOrCreate<osgEarth::PolygonSymbol>()->fill()->color() = osgEarth::Color(
			r / 255.0,
			g / 255.0,
			b / 255.0,
			a / 99.0
		);
	}
	else if (ToolType::GEOPOSITION == m_type) {////���Ƶ�λ
		m_NodeStyle.getOrCreate<osgEarth::PolygonSymbol>()->fill()->color() = osgEarth::Color(
			r / 255.0,
			g / 255.0,
			b / 255.0,
			a / 99.0
		);
	}
	else {
		;//
	}
}

void ToolProperty::getStyleXK(float & xk)
{
	//
	if (ToolType::LINE == m_type) {////��������
		xk = m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->width().value();
	}
	else if (ToolType::POLYGON == m_type) {////���ƶ����
		xk = m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->width().value();;
	}
	else if (ToolType::CIRCLE == m_type) {////����Բ��
		xk = m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->width().value();
	}
	else if (ToolType::RECTANGLE == m_type) {////���ƾ���
		xk = m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->width().value();
	}
	else if (ToolType::TEXT == m_type) {////��������
		xk = m_NodeStyle.getOrCreate<osgEarth::TextSymbol>()->halo()->width().value();
	}
	else if (ToolType::IMAGE == m_type) {////����ͼƬ
		xk = m_NodeStyle.getOrCreate<osgEarth::TextSymbol>()->halo()->width().value();
	}
	else if (ToolType::GEOGETHER == m_type) {////���ƾۺ���
		xk = m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->width().value();
	}
	else if (ToolType::GEOPOSITION == m_type) {////���Ƶ�λ
		xk = m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->width().value();
	}
	else {
		xk = 0.0;
	}
}

void ToolProperty::setStyleXK(float xk)
{
	//
	if (ToolType::LINE == m_type) {////��������
		m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->width() = xk;
	}
	else if (ToolType::POLYGON == m_type) {////���ƶ����
		m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->width() = xk;
	}
	else if (ToolType::CIRCLE == m_type) {////����Բ��
		m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->width() = xk;
	}
	else if (ToolType::RECTANGLE == m_type) {////���ƾ���
		m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->width() = xk;
	}
	else if (ToolType::TEXT == m_type) {////��������
		m_NodeStyle.getOrCreate<osgEarth::TextSymbol>()->halo()->width() = xk;
	}
	else if (ToolType::IMAGE == m_type) {////����ͼƬ
		m_NodeStyle.getOrCreate<osgEarth::TextSymbol>()->halo()->width() = xk;
	}
	else if (ToolType::GEOGETHER == m_type) {////���ƾۺ���
		m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->width() = xk;
	}
	else if (ToolType::GEOPOSITION == m_type) {////���Ƶ�λ
		m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->width() = xk;
	}
}

void ToolProperty::getStyleBJ(float& bj)
{
	//
	if (ToolType::LINE == m_type) {////��������
		bj = 0.0;
	}
	else if (ToolType::POLYGON == m_type) {////���ƶ����
		bj = 0.0;
	}
	else if (ToolType::CIRCLE == m_type) {////����Բ��
		bj = 0.0;
	}
	else if (ToolType::RECTANGLE == m_type) {////���ƾ���
		bj = 0.0;
	}
	else if (ToolType::TEXT == m_type) {////��������
		bj = 0.0;
	}
	else if (ToolType::IMAGE == m_type) {////����ͼƬ
		bj = 0.0;
	}
	else if (ToolType::GEOGETHER == m_type) {////����ͼƬ
		bj = 0.0;
	}
	else if (ToolType::GEOPOSITION == m_type) {////����ͼƬ
		bj = 0.0;
	}
}

void ToolProperty::setStyleBJ(float bj)
{
	//
	if (ToolType::LINE == m_type) {////��������
		bj = 0.0;
	}
	else if (ToolType::POLYGON == m_type) {////���ƶ����
		bj = 0.0;
	}
	else if (ToolType::CIRCLE == m_type) {////����Բ��
		bj = 0.0;
	}
	else if (ToolType::RECTANGLE == m_type) {////���ƾ���
		bj = 0.0;
	}
	else if (ToolType::TEXT == m_type) {////��������
		bj = 0.0;
	}
	else if (ToolType::IMAGE == m_type) {////����ͼƬ
		bj = 0.0;
	}
	else if (ToolType::GEOGETHER == m_type) {////����ͼƬ
		bj = 0.0;
	}
	else if (ToolType::GEOPOSITION == m_type) {////����ͼƬ
		bj = 0.0;
	}
}

void ToolProperty::getStyleXStyle(osgEarth::Stroke::LineCapStyle & XStyle)
{
	//
	if (ToolType::LINE == m_type) {////��������
		XStyle = m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->lineCap().value();
	}
	else if (ToolType::POLYGON == m_type) {////���ƶ����
		XStyle = m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->lineCap().value();
	}
	else if (ToolType::CIRCLE == m_type) {////����Բ��
		XStyle = m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->lineCap().value();
	}
	else if (ToolType::RECTANGLE == m_type) {////���ƾ���
		XStyle = m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->lineCap().value();
	}
	else if (ToolType::GEOGETHER == m_type) {////���ƾۺ���
		XStyle = m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->lineCap().value();
	}
	else if (ToolType::GEOPOSITION == m_type) {////���Ƶ�λ
		XStyle = m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->lineCap().value();
	}
	else if (ToolType::TEXT == m_type) {////��������
		;
	}
	else if (ToolType::IMAGE == m_type) {////����ͼƬ
		;
	}
}

void ToolProperty::setStyleXStyle(osgEarth::Stroke::LineCapStyle XStyle)
{
	//
	if (ToolType::LINE == m_type) {////��������
		m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->lineCap() = XStyle;
	}
	else if (ToolType::POLYGON == m_type) {////���ƶ����
		m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->lineCap() = XStyle;
	}
	else if (ToolType::CIRCLE == m_type) {////����Բ��
		m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->lineCap() = XStyle;
	}
	else if (ToolType::RECTANGLE == m_type) {////���ƾ���
		m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->lineCap() = XStyle;
	}
	else if (ToolType::GEOGETHER == m_type) {////�ۺ���
		m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->lineCap() = XStyle;
	}
	else if (ToolType::GEOPOSITION == m_type) {////���Ƶ�λ
		m_NodeStyle.getOrCreate<osgEarth::LineSymbol>()->stroke()->lineCap() = XStyle;
	}
	else if (ToolType::TEXT == m_type) {////��������
		;
	}
	else if (ToolType::IMAGE == m_type) {////����ͼƬ
		;
	}
}
//{
//	//
//	if (ToolType::LINE == m_type) {////��������
//	}
//	else if (ToolType::POLYGON == m_type) {////���ƶ����
//	}
//	else if (ToolType::CIRCLE == m_type) {////����Բ��
//	}
//	else if (ToolType::RECTANGLE == m_type) {////���ƾ���
//	}
//	else if (ToolType::TEXT == m_type) {////��������
//	}
//	else if (ToolType::IMAGE == m_type) {////����ͼƬ
//	}
//}
//