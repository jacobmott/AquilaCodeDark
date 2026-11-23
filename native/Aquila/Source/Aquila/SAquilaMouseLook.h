#pragma once

#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SLeafWidget.h"

// Receives desktop drags in open space. The command buttons sit above it.
class SAquilaMouseLook : public SLeafWidget
{
public:
    DECLARE_DELEGATE_TwoParams(FOnDrag, FVector2D, FKey);
    DECLARE_DELEGATE_OneParam(FOnPick, FVector2D);
    DECLARE_DELEGATE_OneParam(FOnZoom, float);
    SLATE_BEGIN_ARGS(SAquilaMouseLook) {}
        SLATE_EVENT(FOnDrag, OnDrag)
        SLATE_EVENT(FOnPick, OnPick)
        SLATE_EVENT(FOnZoom, OnZoom)
    SLATE_END_ARGS()

    void Construct(const FArguments& Args) { OnDrag = Args._OnDrag; OnPick = Args._OnPick; OnZoom = Args._OnZoom; }
    bool IsDragging() const { return bDragging; }
    virtual FReply OnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event) override;
    virtual FReply OnMouseButtonDoubleClick(const FGeometry& Geometry, const FPointerEvent& Event) override;
    virtual FReply OnMouseMove(const FGeometry& Geometry, const FPointerEvent& Event) override;
    virtual FReply OnMouseWheel(const FGeometry& Geometry, const FPointerEvent& Event) override;
    virtual FReply OnMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& Event) override;
    virtual void OnMouseCaptureLost(const FCaptureLostEvent& Event) override;
    virtual FCursorReply OnCursorQuery(const FGeometry& Geometry, const FPointerEvent& Event) const override;
    virtual FVector2D ComputeDesiredSize(float) const override { return FVector2D::ZeroVector; }
    virtual int32 OnPaint(const FPaintArgs&, const FGeometry&, const FSlateRect&, FSlateWindowElementList&,
        int32 Layer, const FWidgetStyle&, bool) const override { return Layer; }

private:
    FOnDrag OnDrag;
    FOnPick OnPick;
    FOnZoom OnZoom;
    FIntPoint CursorAtPress = FIntPoint::ZeroValue;
    FVector2D PressUV = FVector2D::ZeroVector;
    FVector2D PendingDelta = FVector2D::ZeroVector;
    FKey PressButton;
    bool bMoved = false;
    bool bDragging = false;
};
