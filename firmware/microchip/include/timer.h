/*********************************************************************
 *
 *                   Timer
 *
 *********************************************************************
 * FileName:        timer.h
 * Processor:       PIC24 /  Daytona
 * Complier:        MPLAB C30/C32
 *
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement:
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its dsPIC30F and PICmicro® Microcontroller is intended 
 * and supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip's dsPIC30F and PICmicro Microcontroller products. 
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * Author               Date      Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *
 ********************************************************************/

#ifndef _MS_TIMER_HEADER_FILE
#define _MS_TIMER_HEADER_FILE

#ifndef __GENERIC_TYPE_DEFS_H_
#include "GenericTypeDefs.h"
#endif

#if defined(__C30__) 
	#define Delay10us(x)			\
	{								\
		unsigned long _dcnt;		\
		_dcnt=x*((unsigned long)(0.00001/(1.0/GetInstructionClock())/6));	\
		while(_dcnt--);				\
	}
	
	#define DelayMs(x)              \
	{                               \
		unsigned long _dcnt;        \
		unsigned long _ms;          \
		_ms = x;                    \
		while (_ms)                 \
		{                           \
		    _dcnt=((unsigned long)(0.001/(1.0/GetInstructionClock())/6));	\
		    while(_dcnt--);			\
		    _ms--;                  \
		}                           \
	}
#elif defined(__PIC32MX__)
void Delay10us(DWORD dwCount);
void DelayMs(WORD ms);
#endif


#endif
