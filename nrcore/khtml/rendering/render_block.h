/*
 * This file is part of the render object implementation for KHTML.
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003 Apple Computer, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#ifndef RENDER_BLOCK_H
#define RENDER_BLOCK_H

#include <qptrlist.h>

#include "render_flow.h"

namespace DOM {
    class Position;
}

namespace khtml {

class RenderBlock : public RenderFlow
{
public:
    RenderBlock(DOM::NodeImpl* node);
    virtual ~RenderBlock();

    virtual const char *renderName() const;

    // These two functions are overridden for inline-block.
    virtual short lineHeight(bool b, bool isRootLineBox=false) const;
    virtual short baselinePosition(bool b, bool isRootLineBox=false) const;
    
    virtual bool isRenderBlock() const { return true; }
    virtual bool isBlockFlow() const { return (!isInline() || isReplaced()) && !isTable(); }
    virtual bool isInlineFlow() const { return isInline() && !isReplaced(); }
    virtual bool isInlineBlockOrInlineTable() const { return isInline() && isReplaced(); }
    
    virtual bool childrenInline() const { return m_childrenInline; }
    virtual void setChildrenInline(bool b) { m_childrenInline = b; }
    void makeChildrenNonInline(RenderObject* insertionPoint = 0);

    // The height (and width) of a block when you include overflow spillage out of the bottom
    // of the block (e.g., a <div style="height:25px"> that has a 100px tall image inside
    // it would have an overflow height of borderTop() + paddingTop() + 100px.
    virtual int overflowHeight(bool includeInterior=true) const
    { return (!includeInterior && hasOverflowClip()) ? m_height : m_overflowHeight; }
    virtual int overflowWidth(bool includeInterior=true) const
    { return (!includeInterior && hasOverflowClip()) ? m_width : m_overflowWidth; }
    virtual void setOverflowHeight(int h) { m_overflowHeight = h; }
    virtual void setOverflowWidth(int w) { m_overflowWidth = w; }

    virtual bool isSelfCollapsingBlock() const;
    virtual bool isTopMarginQuirk() const { return m_topMarginQuirk; }
    virtual bool isBottomMarginQuirk() const { return m_bottomMarginQuirk; }

    virtual short maxTopMargin(bool positive) const {
        if (positive)
            return m_maxTopPosMargin;
        else
            return m_maxTopNegMargin;
    }
    virtual short maxBottomMargin(bool positive) const {
        if (positive)
            return m_maxBottomPosMargin;
        else
            return m_maxBottomNegMargin;
    }

    void initMaxMarginValues() {
        if (m_marginTop >= 0)
            m_maxTopPosMargin = m_marginTop;
        else
            m_maxTopNegMargin = -m_marginTop;
        if (m_marginBottom >= 0)
            m_maxBottomPosMargin = m_marginBottom;
        else
            m_maxBottomNegMargin = -m_marginBottom;
    }

    virtual void addChildToFlow(RenderObject* newChild, RenderObject* beforeChild);
    virtual void removeChild(RenderObject *oldChild);

    virtual void repaintObjectsBeforeLayout();
    virtual void repaintFloatingDescendants();
    virtual void getAbsoluteRepaintRectIncludingFloats(QRect& bounds, QRect& fullBounds);

    virtual void setStyle(RenderStyle* _style);

    virtual void layout();
    virtual void layoutBlock( bool relayoutChildren );
    void layoutBlockChildren( bool relayoutChildren );
    QRect layoutInlineChildren( bool relayoutChildren );

    void layoutPositionedObjects( bool relayoutChildren );
    void insertPositionedObject(RenderObject *o);
    void removePositionedObject(RenderObject *o);

    // Called to lay out the legend for a fieldset.
    virtual RenderObject* layoutLegend(bool relayoutChildren) { return 0; };
    
    // the implementation of the following functions is in bidi.cpp
    void bidiReorderLine(const BidiIterator &start, const BidiIterator &end, BidiState &bidi );
    RootInlineBox* determineStartPosition(bool fullLayout, BidiIterator &start, BidiState &bidi);
    RootInlineBox* determineEndPosition(RootInlineBox* startBox, BidiIterator& cleanLineStart, int& yPos);
    bool matchedEndLine(const BidiIterator& start, const BidiIterator& endLineStart, 
                        RootInlineBox*& endLine, int& endYPos);
    BidiIterator findNextLineBreak(BidiIterator &start, BidiState &info );
    RootInlineBox* constructLine(const BidiIterator& start, const BidiIterator& end);
    InlineFlowBox* createLineBoxes(RenderObject* obj);
    void computeHorizontalPositionsForLine(RootInlineBox* lineBox, BidiState &bidi);
    void computeVerticalPositionsForLine(RootInlineBox* lineBox);
    void deleteEllipsisLineBoxes();
    void checkLinesForTextOverflow();
    // end bidi.cpp functions
    
    virtual void paint(PaintInfo& i, int tx, int ty);
    void paintObject(PaintInfo& i, int tx, int ty);
    void paintFloats(PaintInfo& i, int _tx, int _ty, bool paintSelection = false);
    void paintEllipsisBoxes(PaintInfo& i, int _tx, int _ty);

    void insertFloatingObject(RenderObject *o);
    void removeFloatingObject(RenderObject *o);

    // called from lineWidth, to position the floats added in the last line.
    void positionNewFloats();
    void clearFloats();
    int getClearDelta(RenderObject *child);
    virtual void markAllDescendantsWithFloatsForLayout(RenderObject* floatToRemove = 0);
    void markPositionedObjectsForLayout();

    virtual bool containsFloats() { return m_floatingObjects!=0; }
    virtual bool containsFloat(RenderObject* o);

    virtual bool hasOverhangingFloats() { return floatBottom() > m_height; }
    void addOverHangingFloats( RenderBlock *block, int xoffset, int yoffset, bool child = false );
    
    int nearestFloatBottom(int height) const;
    int floatBottom() const;
    inline int leftBottom();
    inline int rightBottom();

    virtual int lineWidth(int y) const;
    virtual int lowestPosition(bool includeOverflowInterior=true, bool includeSelf=true) const;
    virtual int rightmostPosition(bool includeOverflowInterior=true, bool includeSelf=true) const;
    virtual int leftmostPosition(bool includeOverflowInterior=true, bool includeSelf=true) const;

    int rightOffset() const;
    int rightRelOffset(int y, int fixedOffset, bool applyTextIndent = true,
                       int *heightRemaining = 0) const;
    int rightOffset(int y) const { return rightRelOffset(y, rightOffset(), true); }

    int leftOffset() const;
    int leftRelOffset(int y, int fixedOffset, bool applyTextIndent = true,
                      int *heightRemaining = 0) const;
    int leftOffset(int y) const { return leftRelOffset(y, leftOffset(), true); }

    virtual bool nodeAtPoint(NodeInfo& info, int x, int y, int tx, int ty,
                             HitTestAction hitTestAction = HitTestAll, bool inside=false);

    bool isPointInScrollbar(int x, int y, int tx, int ty);

    virtual DOM::Position positionForCoordinates(int x, int y);
    
    virtual void calcMinMaxWidth();
    void calcInlineMinMaxWidth();
    void calcBlockMinMaxWidth();

    virtual int getBaselineOfFirstLineBox() const;

    RootInlineBox* firstRootBox() { return static_cast<RootInlineBox*>(m_firstLineBox); }
    RootInlineBox* lastRootBox() { return static_cast<RootInlineBox*>(m_lastLineBox); }

    // Obtains the nearest enclosing block (including this block) that contributes a first-line style to our inline
    // children.
    virtual RenderBlock* firstLineBlock() const;
    virtual void updateFirstLetter();
    
    bool inRootBlockContext() const;
    
    void setLinesAppended(bool b=true) { m_linesAppended = b; }
    bool linesAppended() const { return m_linesAppended; }

    void setHasMarkupTruncation(bool b=true) { m_hasMarkupTruncation = b; }
    bool hasMarkupTruncation() const { return m_hasMarkupTruncation; }

#ifndef NDEBUG
    virtual void printTree(int indent=0) const;
    virtual void dump(QTextStream *stream, QString ind = "") const;
#endif

    // Helper methods for computing line counts and heights for line counts.
    RootInlineBox* lineAtIndex(int i);
    int lineCount();
    int heightForLineCount(int l);
    void clearTruncation();

protected:
    void newLine();
    void removeChildrenFromLineBoxes();

private:
    DOM::Position positionForBox(InlineBox *box, bool start=true) const;
    DOM::Position positionForRenderer(RenderObject *renderer, bool start=true) const;
    
protected:
    struct FloatingObject {
        enum Type {
            FloatLeft,
            FloatRight
        };

        FloatingObject(Type _type) {
            node = 0;
            startY = 0;
            endY = 0;
            type = _type;
            left = 0;
            width = 0;
            noPaint = false;

        }
        RenderObject* node;
        int startY;
        int endY;
        int left;
        int width;
        Type type : 1; // left or right aligned
        bool noPaint : 1;
    };
    
protected:
    QPtrList<FloatingObject>* m_floatingObjects;
    QPtrList<RenderObject>* m_positionedObjects;
    
    bool m_childrenInline : 1;
    bool m_pre            : 1;
    bool m_firstLine      : 1; // used in inline layouting
    EClear m_clearStatus  : 2; // used during layuting of paragraphs
    bool m_topMarginQuirk : 1;
    bool m_bottomMarginQuirk : 1;
    bool m_linesAppended : 1; // Whether or not a block with inline children has had lines appended.
    bool m_hasMarkupTruncation : 1;

    short m_maxTopPosMargin;
    short m_maxTopNegMargin;
    short m_maxBottomPosMargin;
    short m_maxBottomNegMargin;

    // How much content overflows out of our block vertically or horizontally (all we support
    // for now is spillage out of the bottom and the right, which are the common cases).
    // XXX Generalize to work with top and left as well.
    int m_overflowHeight;
    int m_overflowWidth;
};

}; // namespace

#endif // RENDER_BLOCK_H

