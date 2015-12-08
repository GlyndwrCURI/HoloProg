#include "stdafx.h"
//Note the following 'Globals' are not really global at all... they have file scope and are 
// global only to this file.  There is no globally shared data with the rest of the program.

const double PIdiv180 = M_PI / 180;             //Multiplying by PIdiv180 converts Degrees to Radians
const double I80divPI = (double)180.0 / M_PI;   //Multiplying by 180divPI converts Radians to Degrees

static double glRefBeamTilt_A4;
static double glLightCoordsD_C4;
static double glLightCoordsOmega_D4;
static double glHologSizeX_H4;
static double glHologSizeY_I4;

//Positions of Light:
static double glXl_E4;
static double glYl_F4;
static double glZl_G4;

//The above variables need to be setup only once per Motor Position Generation.  An initial call to this function does that:
// (Using the 'double' datatype for NoofCols & NoofLines is ok, & intentional)
void SetUpGlobals(double RefBeamTilt, double LightCoords, double LightCoordsOmega, double NoofCols, double HorzDistanceBetweenPixelCentersCMS, double NoofLines, double VertDistanceBetweenPixelCentersCMS){
 glRefBeamTilt_A4      = RefBeamTilt;      //= 60.0;
 glLightCoordsD_C4     = LightCoords;      //=150.0;
 glLightCoordsOmega_D4 = LightCoordsOmega; //60.0;
 //glHologSizeX_H4       = (NoofCols  - 1) * HorzDistanceBetweenPixelCentersCMS;//must be cms
 //glHologSizeY_I4       = (NoofLines - 1) * VertDistanceBetweenPixelCentersCMS;//must be cms
 glHologSizeX_H4       = NoofCols * HorzDistanceBetweenPixelCentersCMS; //must be cms
 glHologSizeY_I4       = NoofLines * VertDistanceBetweenPixelCentersCMS;//must be cms

//Compute Position of Light:
 glXl_E4 = (glHologSizeX_H4 / 2) + glLightCoordsD_C4 * sin(glLightCoordsOmega_D4 * PIdiv180);
 glYl_F4 = glHologSizeY_I4 / 2;
 glZl_G4 = glLightCoordsD_C4 * cos(glLightCoordsOmega_D4 * PIdiv180);
}

double GetWxInDegreesFromSpreadSheet(double CellB7, double CellC7){ //B7 & C7 are dblHpositionInCms & dblVpositionInCms
 double CellD7,CellE7,/*CellF7,CellG7,CellH7,*/CellI7,/*CellJ7,*/CellK7,CellL7,/*CellM7,*/CellN7,/*CellO7,*/CellP7/*,CellQ7*/;

 CellD7 = sqrt( pow((glXl_E4 - CellB7),2.0) + pow((glYl_F4 - CellC7),2.0) + pow(glZl_G4,2.0) );
 //CellD7 = sqrt((glXl_E4 - CellB7) * (glXl_E4 - CellB7) 
   //          + (glYl_F4 - CellC7) * (glYl_F4 - CellC7)
     //        + (glZl_G4 * glZl_G4) );
 CellE7 = I80divPI * acos(glZl_G4 / CellD7);
 //CellF7 = (glYl_F4 - CellC7) / CellD7 / sin(CellE7 * PIdiv180);//No Dependents!
 //CellG7 = (glXl_E4 - CellB7) / CellD7 / sin(CellE7 * PIdiv180);//No Dependents!
 //CellH7 = I80divPI * atan2(CellF7/CellG7);                     //No Dependents!
 CellI7 = (glXl_E4 - CellB7) / CellD7;
 //CellJ7 = (glYl_F4 - CellC7) / CellD7;          J is not used when calculating Wx
 CellK7 =  glZl_G4 / CellD7;
 CellL7 = I80divPI * acos(CellI7 * sin(glRefBeamTilt_A4 * PIdiv180) + CellK7 * cos(glRefBeamTilt_A4 * PIdiv180) );
 //CellM7 = CellJ7 / sin(CellL7 * PIdiv180);                                                                                                   //One or Other, depending on
 CellN7 = (CellI7 - cos(CellL7 * PIdiv180) * sin(glRefBeamTilt_A4 * PIdiv180)) / (sin(CellL7 * PIdiv180) * cos(glRefBeamTilt_A4 * PIdiv180));  // if we're calculating Wx or Wy

 //CellO7 = I80divPI * atan2(CellM7,CellN7);   //No Dependents!

 CellP7 = tan(CellL7 * PIdiv180) * CellN7;     //One or Other, depending on
 //CellQ7 = tan(CellL7 * PIdiv180) * CellM7;   // if we're calculating Wx or Wy

 return I80divPI * atan(CellP7);    //This is column R
}

double GetWyInDegreesFromSpreadSheet(double CellB7, double CellC7){ //B7 & C7 are dblHpositionInCms & dblVpositionInCms
 double CellD7,CellE7,/*CellF7,CellG7,CellH7,*/CellI7,CellJ7,CellK7,CellL7,CellM7,/*CellN7,CellO7,CellP7,*/CellQ7;

 CellD7 = sqrt( pow((glXl_E4 - CellB7),2.0) + pow((glYl_F4 - CellC7),2.0) + pow(glZl_G4,2.0) );
 //CellD7 = sqrt((glXl_E4 - CellB7) * (glXl_E4 - CellB7) 
   //          + (glYl_F4 - CellC7) * (glYl_F4 - CellC7)
     //        + (glZl_G4 * glZl_G4) );
 CellE7 = I80divPI * acos(glZl_G4 / CellD7);
 //CellF7 = (glYl_F4 - CellC7) / CellD7 / sin(CellE7 * PIdiv180);//No Dependents!
 //CellG7 = (glXl_E4 - CellB7) / CellD7 / sin(CellE7 * PIdiv180);//No Dependents!
 //CellH7 = I80divPI * atan2(CellF7/CellG7);                     //No Dependents!
 CellI7 = (glXl_E4 - CellB7) / CellD7;
 CellJ7 = (glYl_F4 - CellC7) / CellD7;
 CellK7 =  glZl_G4 / CellD7;
 CellL7 = I80divPI * acos(CellI7 * sin(glRefBeamTilt_A4 * PIdiv180) + CellK7 * cos(glRefBeamTilt_A4 * PIdiv180) );
 CellM7 = CellJ7 / sin(CellL7 * PIdiv180);                                                                                                     //One or Other, depending on
 //CellN7 = (CellI7 - cos(CellL7 * PIdiv180) * sin(glRefBeamTilt_A4 * PIdiv180)) / (sin(CellL7 * PIdiv180) * cos(glRefBeamTilt_A4 * PIdiv180));// if we're calculating Wx or Wy

 //CellO7 = I80divPI * atan2(CellM7,CellN7);   //No Dependents!

 //CellP7 = tan(CellL7 * PIdiv180) * CellN7;   //One or Other, depending on
 CellQ7 = tan(CellL7 * PIdiv180) * CellM7;     // if we're calculating Wx or Wy

 return I80divPI * atan(CellQ7);    //This is column S
}

//Note (for row 7) Column O and Column H have no dependents!
//Also, Columns F and G are precedents to Column H, but as this is their only dependent
//(which itself isn't used), there was no point in calculating F and G either.