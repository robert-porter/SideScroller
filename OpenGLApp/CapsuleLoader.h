#pragma once

#include "Shape.h"
#include "TinyXML2.h"
#include <string>


//void CreateFixtureDefFromFile(tinyxml2::XMLElement *ele, FixtureDef &f)
//{
//	tinyxml2::XMLElement *shapeEle = ele->FirstChildElement("Shape");
	//f.shape = CreateShapeFromFile(shapeEle);
//}

inline Shape *CreateShapeFromFile(tinyxml2::XMLElement *ele)	//calls new
{
	Shape *s = 0;
	std::string stype = ele->Attribute("Type");

	if(stype == "Circle") {
		s = new Circle;
		Circle *c = (Circle *)s;
		tinyxml2::XMLElement *localPosElement = ele->FirstChildElement("localPos");
		localPosElement->FirstChildElement("x")->QueryFloatText(&c->localPos.x);
		localPosElement->FirstChildElement("y")->QueryFloatText(&c->localPos.y);
		ele->FirstChildElement("radius")->QueryFloatText(&c->radius);
	} else if(stype == "Capsule")	{
		s = new Capsule;
		Capsule *c = (Capsule *)s;
		tinyxml2::XMLElement *localPos0Element = ele->FirstChildElement("localPos0");
		localPos0Element->FirstChildElement("x")->QueryFloatText(&c->localPos0.x);
		localPos0Element->FirstChildElement("y")->QueryFloatText(&c->localPos0.y);
		tinyxml2::XMLElement *localPos1Element = ele->FirstChildElement("localPos1");
		localPos1Element->FirstChildElement("x")->QueryFloatText(&c->localPos1.x);
		localPos1Element->FirstChildElement("y")->QueryFloatText(&c->localPos1.y);
		ele->FirstChildElement("radius")->QueryFloatText(&c->radius);
	} else if(stype == "AABB") {
		s = new AABox;
	}
	return s;
}

inline void SaveShapeToFile(tinyxml2::XMLPrinter &printer, Shape *shape) {	
	printer.OpenElement("Shape");

	if(shape->GetType() == ShapeType::eCapsule)	{
		Capsule *capsule = (Capsule *)shape;
		printer.PushAttribute("Type", "Capsule");
		printer.OpenElement("localPos0");
		printer.OpenElement("x");
		printer.PushText(capsule->localPos0.x);
		printer.CloseElement();
		printer.OpenElement("y");
		printer.PushText(capsule->localPos0.y);
		printer.CloseElement();
		printer.CloseElement();
		printer.OpenElement("localPos1");
		printer.OpenElement("x");
		printer.PushText(capsule->localPos1.x);
		printer.CloseElement();
		printer.OpenElement("y");
		printer.PushText(capsule->localPos1.y);
		printer.CloseElement();
		printer.CloseElement();
		printer.OpenElement("radius");
		printer.PushText(capsule->radius);
		printer.CloseElement();
	} else	if(shape->GetType() == ShapeType::eCircle) {
		Circle *circle = (Circle *)shape;
		printer.PushAttribute("Type", "Circle");
		printer.OpenElement("localPos");
		printer.OpenElement("x");
		printer.PushText(circle->localPos.x);
		printer.CloseElement();
		printer.OpenElement("y");
		printer.PushText(circle->localPos.y);
		printer.CloseElement();
		printer.CloseElement();
		printer.OpenElement("radius");
		printer.PushText(circle->radius);
		printer.CloseElement();
	}
	printer.CloseElement();
}
