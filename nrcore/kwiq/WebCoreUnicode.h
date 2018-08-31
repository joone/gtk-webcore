/*
 * Copyright (C) 2003 Apple Computer, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */
#ifndef WebCoreUnicode_h
#define WebCoreUnicode_h


typedef unsigned short UniChar;
typedef unsigned int UChar32;


#ifdef __cplusplus
extern "C" {
#endif

    typedef enum
    {
        DirectionL = 0, 	// Left Letter 
        DirectionR,	// Right Letter
        DirectionEN,	// European Number
        DirectionES,	// European Separator
        DirectionET,	// European Terminator (post/prefix e.g. $ and %)
        DirectionAN,	// Arabic Number
        DirectionCS,	// Common Separator 
        DirectionB, 	// Paragraph Separator (aka as PS)
        DirectionS, 	// Segment Separator (TAB)
        DirectionWS, 	// White space
        DirectionON,	// Other Neutral

	// types for explicit controls
        DirectionLRE, 
        DirectionLRO, 

        DirectionAL, 	// Arabic Letter (Right-to-left)

        DirectionRLE, 
        DirectionRLO, 
        DirectionPDF, 

        DirectionNSM, 	// Non-spacing Mark
        DirectionBN	// Boundary neutral (type of RLE etc after explicit levels)
    } WebCoreUnicodeDirection;


    extern int (*WebCoreUnicodeDigitValueFunction)(UChar32 c);
    extern WebCoreUnicodeDirection (*WebCoreUnicodeDirectionFunction)(UChar32 c);
    extern bool (*WebCoreUnicodeMirroredFunction)(UChar32 c);
    extern UChar32 (*WebCoreUnicodeMirroredCharFunction)(UChar32 c);
    extern UChar32 (*WebCoreUnicodeLowerFunction)(UChar32 c);
    extern UChar32 (*WebCoreUnicodeUpperFunction)(UChar32 c);

#ifdef __cplusplus
}
#endif


#endif
