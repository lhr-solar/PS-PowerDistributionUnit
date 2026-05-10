// ACS3704x-010B3.c

#include "ACS3704x-010B3.h"

float ACS3704x_010B3_Current_Conversion(float v)
{
    return (v - ACS3704x_010B3_V_QVO) / ACS3704x_010B3_SENSITIVITY;
}

float ACS3704x_010B3_Current_Conversion_QVOCentered(float v)
{
    return v / ACS3704x_010B3_SENSITIVITY;
}