#pragma once

#include "Widgets/SLeafWidget.h"

class SAquilaFiringRibbon : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SAquilaFiringRibbon) {}
        SLATE_ATTRIBUTE(FString, Description)
        SLATE_ATTRIBUTE(double, Time)
        SLATE_ATTRIBUTE(bool, ToRight)
    SLATE_END_ARGS()
    void Construct(const FArguments& Args);
    struct FFrame { FString Text; double X = 0; bool bFinalHold = false; };
    FFrame FrameAt(double Seconds, bool bToRight) const;
    double Duration() const { return Cycle; }
    virtual int32 OnPaint(const FPaintArgs&, const FGeometry&, const FSlateRect&, FSlateWindowElementList&,
        int32, const FWidgetStyle&, bool) const override;
    virtual FVector2D ComputeDesiredSize(float) const override { return FVector2D(200,30); }
private:
    struct FClause { FString Text; double Width, ReadSeconds; };
    TAttribute<FString> Description;
    TAttribute<double> Time;
    TAttribute<bool> ToRight;
    FSlateFontInfo Font;
    mutable TArray<FClause> Clauses;
    mutable FString Previous;
    mutable double LaneWidth = 0, Cycle = 0;
    void Layout(double Width) const;
};
