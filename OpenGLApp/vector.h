//****************************************************************************//
// vector.h                                                                   //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_VECTOR_H
#define CAL_VECTOR_H

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//
#include "Vector3.h"

//****************************************************************************//
// Forward declarations                                                       //
//****************************************************************************//

//class Quaternion;
//class CalMatrix;

//****************************************************************************//
// Class declaration                                                          //
//****************************************************************************//

 /*****************************************************************************/
/** The plane class.
  *****************************************************************************/


class CAL3D_API CalPlane
{
   public:
      float a,b,c,d;
      
      // These methods are made only to calculate the bounding boxes,
      // don't use them in you program
      
      float eval(Vector3f &p);
	  float dist(Vector3f &p);
      void setPosition(Vector3f &p);
      void setNormal(Vector3f &p);
};

 /*****************************************************************************/
/** The bounding box class.
  *****************************************************************************/


class CAL3D_API CalBoundingBox
{
   public:
     CalPlane plane[6];
     
     void computePoints(Vector3f *p);
   
};



#endif

//****************************************************************************//
