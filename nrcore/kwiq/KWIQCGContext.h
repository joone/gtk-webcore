#ifndef KWIQCGContext_h
#define KWIQCGContext_h

#include <gdk/gdk.h>

class CGContext
{
public:
    GdkDrawable* drawable;
    GdkGC* gc;
    GdkRegion* clip;

    virtual ~CGContext() {};
    virtual void addClip(GdkRectangle* rect) = 0;

    virtual void saveGraphicsState() = 0;
    virtual void restoreGraphicsState() = 0;

};

typedef CGContext* CGContextRef;

enum NSCompositingOperation {
    NSCompositeClear = 0,
    NSCompositeCopy,
    NSCompositeSourceOver ,
    NSCompositeSourceIn,
    NSCompositeSourceOut, 
    NSCompositeSourceAtop, 
    NSCompositeDestinationOver,
    NSCompositeDestinationIn,
    NSCompositeDestinationOut,
    NSCompositeDestinationAtop,
    NSCompositeXOR,
    NSCompositePlusDarker,
    NSCompositeHighlight,
    NSCompositePlusLighter
};

#endif
