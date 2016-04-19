/*  Departamento de Informatica, PUC-Rio, INF1761 Computer Graphhics
*
*   @file color.h TAD: Generic colorimetry computarioms (interface).
*   @author Marcelo Gattass and others
*
*   @date
*         Last version:   06/2014.
*
*   @version 4.0
*
*   @Copyright/License
*   DI PUC-Rio Educational Software
*   All the products under this license are free software: they can be used for both academic and commercial purposes at absolutely no cost.
*   There are no paperwork, no royalties, no GNU-like "copyleft" restrictions, either. Just download and use it.
*   They are licensed under the terms of the MIT license reproduced below, and so are compatible with GPL and also qualifies as Open Source software.
*   They are not in the public domain, PUC-Rio keeps their copyright. The legal details are below.
*
*   The spirit of this license is that you are free to use the libraries for any purpose at no cost without having to ask us.
*   The only requirement is that if you do use them, then you should give us credit by including the copyright notice below somewhere in your product or its documentation.
*
*   Copyright © 2010-2014 DI PUC-Rio Educational Software
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software
*   without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sub license, and/or sell copies of the Software, and to permit
*   persons to whom the Software is furnished to do so, subject to the following conditions:
*
*   The above copyright notice and this permission notice shall be included in all copies or suavlantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*/

#ifndef _COR_H_
#define _COR_H_

enum illuminant { A, C, D50, D55, D65, D75, F2, F7, F11, I100, n_illuminants };

/* Simple query functions **************************************************************************/

/* gets CIE 1931 color matching functions: x_bar, y_bar and z_bar for a given lambda */
int corGetCMFCIExyz(float lambda, float* x_bar, float* y_bar, float* z_bar);

/* gets radiace of a Illuminant A, C, D50, D55, D65 or D75 for a given lambda , [380,780] nm */
float corGetRadianceA(float lambda);
float corGetRadianceC(float lambda);
float corGetRadianceD65(float lambda);
float corGetRadianceD55(float lambda);
float corGetRadianceD65(float lambda);
float corGetRadianceD75(float lambda);

/* gets radiace of a Illuminant A, C, D50, D55, D65, D75 or I100 for a given lambda , [380,780] nm */
float corGetIllum(float lambda, int reference_light);


/* Color Systems: Color Conversion Functions  **************************************************** */
/* CIEXYZ to CIERGB  (Values 0...1) */
int corCIEXYZtoCIERGB(float X, float Y, float Z, float* R, float* G, float* B);

/* CIEXYZ to CIExyY  (Values 0...1) */
int corCIEXYZtoCIExyY(float X, float Y, float Z, float* x, float* y);

/* CIExyY to CIEXYZ  (Values Y 0...1) */
int corCIExyYtoXYZ(float x, float y, float Y, float* X, float* Z);

/* CIEXYZ to sRGB  (Values Y 0...1,  R,G,B 0...1) */
int corCIEXYZtosRGB(float X, float Y, float Z, float* R, float* G, float* B, int reference_light);

/* sRGB to CIEXYZ  (Values Y 0...1,  R,G,B 0...1) */
int corsRGBtoCIEXYZ(float R, float G, float B, float* X, float* Y, float* Z, int reference_light);

/* CIEXYZ to CIELab  (Values Y,L 0...1) */
int corCIEXYZtoLab(float X, float Y, float Z, float* L, float* a, float* b, int  reference_light);

/* Lab to CIEXYZ  (Values Y,L 0...1)  */
int corCIELabtoXYZ(float L, float a, float b, float* X, float* Y, float* Z, int reference_light);

/* convert from CIELab to sRGB */
int corCIELabtosRGB(float L, float a, float b, float* R, float* G, float* B, int reference_light);

/* convert from RGB to HSL and vice-versa (values = from 0 to 1) */
void corRGBtoHSL(float R, float G, float B, float* H, float* S, float* L);
void corHSLtoRGB(float H, float S, float L, float* R, float* G, float* B);


/* convert from RGB to HSV and vice-versa (values = from 0 to 1) */
void corRGBtoHSV(float R, float G, float B, float* H, float* S, float* V);
void corHSVtoRGB(float H, float S, float V, float* R, float* G, float* B);



/* * color difference formula CIE2000 *********************************************************/
float corDelta_2000_Lab(float L1, float a1, float b1, float L2, float a2, float b2);


/* Spectral Functions *************************************************************************/

/* gets the chromaticity of a pure light source (single lambda) lambda between [380,780] nm */
int corGetCIExyfromLambda(float lambda, float* x, float* y);

/* get CIEXYZ coordinate of a given reflective spectrum defined by the array reflectance[401], i.e.  [380nm,780nm] @ 1nm*/
int corCIEXYZfromReflectance401(float reflectance[401], float* X, float* Y, float* Z, int reference_light);

/* get CIEXYZ coordinate of a given reflective spectrum defined by n_points ranging from lambda_min at an interval delta_lambda */
/* here reflactance is an return parameter  as it is X,Y and Z */
int corCIEXYZfromSurfaceReflectance(float lambda_min, int n_points, float delta_lambda, float* reflectance, float* X, float* Y, float* Z, int reference_light);

/* Computes the most saturated CIE XYZ color from an spectrum centred at lambda_bar that has the luminance Y_bar */
int getCIEXYZGamutBorder(float Y_bar, float lambda_bar, float* X, float* Y, float* Z, float* betha, int reference_light);

#endif