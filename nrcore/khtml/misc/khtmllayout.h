/*
    This file is part of the KDE libraries

    Copyright (C) 1999 Lars Knoll (knoll@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.


    This widget holds some useful definitions needed for layouting Elements
*/
#ifndef HTML_LAYOUT_H
#define HTML_LAYOUT_H

/*
 * this namespace contains definitions for various types needed for
 * layouting.
 */
namespace khtml
{

    const int UNDEFINED = -1;

    // alignment
    enum VAlign { VNone=0, Bottom, VCenter, Top, Baseline };
    enum HAlign { HDefault, Left, HCenter, Right, HNone = 0 };

    /*
     * %multiLength and %Length
     */
    enum LengthType { Variable = 0, Relative, Percent, Fixed, Static, Intrinsic, MinIntrinsic };
    struct Length
    {
#if !KWIQ
	Length() { *((Q_UINT32 *)this) = 0; }
        Length(LengthType t) { type = t; value = 0; quirk = false; }
        Length(int v, LengthType t, bool q=false) : value(v), type(t), quirk(q) {}
        Length(const Length &o)
	    { *((Q_UINT32 *)this) = *((Q_UINT32 *)&o); }

        Length& operator=(const Length& o)
            { *((Q_UINT32 *)this) = *((Q_UINT32 *)&o); return *this; }
        bool operator==(const Length& o) const
            { return *((Q_UINT32 *)this) == *((Q_UINT32 *)&o); }
        bool operator!=(const Length& o) const
            { return *((Q_UINT32 *)this) != *((Q_UINT32 *)&o); }
#else   // Modifications to make things more portable (work with ARM)
	// let compiler generate
	//   Length& Length(const Length&) {...}
	//   Length& operator=(const Length&) {...}
	// According to quick tests, they're actually atleast as efficient and they work for arm too	
	Length(): value(0),type(Variable),quirk(0) { };
        Length(LengthType t) : value(0),type(t),quirk(false) {};
        Length(int v, LengthType t, bool q=false) : value(v), type(t), quirk(q) {}
	
	bool operator==(const Length& o) const
	{ return (type == o.type && value==o.value && quirk==o.quirk);}
	bool operator!=(const Length& o) const
	{ return !(*this==o); }
#endif

	int length() const { return value; }
      
	/*
	 * works only for Fixed and Percent, returns -1 otherwise
	 */
	int width(int maxWidth) const
	    {
		switch(type)
		{
		case Fixed:
		    return value;
		case Percent:
		    return maxWidth*value/100;
		case Variable:
		    return maxWidth;
		default:
		    return -1;
		}
	    }
	/*
	 * returns the minimum width value which could work...
	 */
	int minWidth(int maxWidth) const
	    {
		switch(type)
		{
		case Fixed:
		    return value;
		case Percent:
		    return maxWidth*value/100;
		case Variable:
		default:
		    return 0;
		}
	    }
        bool isVariable() const { return (type == Variable); }
        bool isRelative() const { return (type == Relative); }
        bool isPercent() const { return (type == Percent); }
        bool isFixed() const { return (type == Fixed); }
        bool isStatic() const { return (type == Static); }

        int value : 28;
        LengthType type : 3;
        bool quirk : 1;
#if !KWIQ
    };
#else
    } Q_PACKED;
#endif

};

#endif
