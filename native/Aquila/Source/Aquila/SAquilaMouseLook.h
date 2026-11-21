#pragma once

#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SLeafWidget.h"

// Receives desktop drags in open space. The command buttons sit above it.
class SAquilaMouseLook : public SLeafWidget
{
public:
    DECLARE_DELEGATE_OneParam(FOnDrag, FVector2D);
    SLATE_BEGIN_ARGS(SAquilaMouseLook) {}
        SLATE_EVENT(FOnDrag, OnDrag)
    SLATE_END_ARGS()

    void Construct(const FArguments& Args) { OnDrag = Args._OnDrag; }
    bool IsDragging() const { return bDragging; }
    virtual FReply OnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event) override;
    virtual FReply OnMouseButtonDoubleClick(const FGeometry& Geometry, const FPointerEvent& Event) override;
    virtual FReply OnMouseMove(const FGeometry& Geometry, const FPointerEvent& Event) override;
    virtual FReply OnMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& Event) override;
    virtual void OnMouseCaptureLost(const FCaptureLostEvent& Event) override;
    virtual FCursorReply OnCursorQuery(const FGeometry& Geometry, const FPointerEvent& Event) const override;
    virtual FVector2D ComputeDesiredSize(float) const override { return FVector2D::ZeroVector; }
    virtual int32 OnPaint(const FPaintArgs&, const FGeometry&, const FSlateRect&, FSlateWindowElementList&,
        int32 Layer, const FWidgetStyle&, bool) const override { return Layer; }

private:
    FOnDrag OnDrag;
    FIntPoint CursorAtPress = FIntPoint::ZeroValue;
    bool bDragging = false;
};
