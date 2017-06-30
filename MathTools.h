/*
Copyright (c) 2003-2005
Rui Ferreira, Ricardo Barreira, Nuno Cerqueira, Rui Martins
University of Porto
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the University of Porto nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _MATH_H_
#define _MATH_H_

class MathTools;

#include <math.h>
#include "defines.h"

#define PI (3.1415926)

/** \brief Classe com diversas ferramentas matemáticas
 *
 * Classe com diversas ferramentas matemáticas.
 */
class MathTools
{
 public:
  
  /** \brief Converte um angulo em graus para radianos */
  static double deg2rad(double deg)
    {
      return normalize((deg*PI)/180);
    }
  
  /** \brief Converte um angulo em radianos para graus */
  static double rad2deg(double rad)
    {
      return normalize_180_to_180( (rad*180)/PI );
    }
  
  /** \brief Normaliza um angulo em radianos */
  static double normalize(double rad)
    {
      while (rad>=PI)
	rad-=2*PI;
      while (rad<-PI)
	rad+=2*PI;
      return rad;
    }
  
  /** \brief Normaliza um angulo em graus entre -180 e 180 */
  static double normalize_180_to_180(double deg)
    { 
      while (deg>=180)
	deg-=360;
      while (deg<-180)
	deg+=360;
      return deg;
    }

  
  /** \brief Normaliza um ang em graus entre 0 e 360 */
  static double normalize_0_to_360(double deg)
    {
      while (deg>=360)
	deg-=360;
      while (deg<0)
	deg+=360;
      return deg;
    }
  
  /** \brief Calcula o angulo da linha entre 2 pontos */
  static double calc_angle(double x1, double y1, double x2, double y2)
    {
      double ret = atan2(y2-y1, x2-x1);
      ret = rad2deg(ret);
      return normalize_180_to_180(ret);
    } 
  
  /** \brief Calcula a distância entre 2 pontos */
  static double distance(const double x1, const double y1,
			 const double x2, const double y2)
    {
      return hypot((x1-x2),(y1-y2));
    }
  
  /** \brief Verifica se um angulo está entre 2 angulos */
  static double angle_diff(double a1,double a2)
    {
	    double dif = a1-a2;
	    while(dif<-180)
		    dif+=360;
	    while(dif>180)
		    dif-=360;
	    return dif;
    }

  /** \brief Verifica se um angulo está entre 2 angulos */
  static bool angle_between(double angle, double a1,double a2)
    {
      while(a2<a1)
	a2+=360;
      
      while(angle>a1)
	angle-=360;
      
      while(1)
	{
	  if (angle >=a1 && angle<=a2)
	    return true;
	  else if (angle >a1)
	    return false;
	  else angle+=360;
	}
      
      return false;		 
    }
  
  /** \brief Retorna o maior de dois valores */
  static double max(double a, double b)
    {
      return (a>b)?(a):(b);
    }
  
  /** \brief Retorna o menor de dois valores */
  static double min(double a, double b)
    {
      return (a<b)?(a):(b);
    }
};

#endif
