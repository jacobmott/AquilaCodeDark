#include "AquilaFiringRibbon.h"
#include "AquilaFlightPawn.h"
#include "Fonts/FontMeasure.h"
#include "Framework/Application/SlateApplication.h"
#include "Rendering/DrawElements.h"
#include "Rendering/SlateRenderer.h"
#include "Styling/CoreStyle.h"
#include "Widgets/SCanvas.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Brushes/SlateColorBrush.h"

namespace { constexpr double Slide = .49; }

void SAquilaFiringRibbon::Construct(const FArguments& Args)
{
    Description = Args._Description;
    Time = Args._Time;
    ToRight = Args._ToRight;
    Font = FCoreStyle::GetDefaultFontStyle("Regular",16);
    SetClipping(EWidgetClipping::ClipToBoundsAlways);
    ForceVolatile(true);
}

void SAquilaFiringRibbon::Layout(double Width) const
{
    const FString Copy = Description.Get();
    if (Previous == Copy && FMath::IsNearlyEqual(LaneWidth,Width,.5)) return;
    LaneWidth = Width;
    Previous = Copy;
    Clauses.Reset();
    Cycle = 0;
    const auto Measure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
    auto Add = [&](const FString& Line) {
        if (Line.IsEmpty()) return;
        const double Hold = FMath::Max(.68, Line.Len() / 19.);
        Clauses.Add({Line,Measure->Measure(Line,Font).X,Hold});
        Cycle += 2*Slide + Hold;
    };
    TArray<FString> Words;
    Copy.ParseIntoArrayWS(Words);
    FString Line;
    for (const auto& Word : Words)
    {
        const FString Next = Line.IsEmpty() ? Word : Line + TEXT(" ") + Word;
        if (!Line.IsEmpty() && Measure->Measure(Next,Font).X > Width-8) { Add(Line); Line = Word; }
        else Line = Next;
        if (Word.EndsWith(TEXT("."))) { Add(Line); Line.Reset(); }
    }
    Add(Line);
    if (Clauses.Num()) Cycle += 1.;
}

SAquilaFiringRibbon::FFrame SAquilaFiringRibbon::FrameAt(double Seconds, bool bToRight) const
{
    if (Cycle <= 0) return {};
    double T = FMath::Fmod(Seconds,Cycle);
    for (int32 I=0; I<Clauses.Num(); ++I)
    {
        const auto& C = Clauses[I];
        const bool bLast = I == Clauses.Num()-1;
        const double End = 2*Slide + C.ReadSeconds;
        if (T >= End && !bLast) { T -= End; continue; }
        // Whole phrases move outward on both sides. Words and glyphs always retain natural reading order.
        const double Start = bToRight ? -C.Width : LaneWidth;
        const double Middle = (LaneWidth-C.Width)*.5;
        const double Exit = bLast ? (bToRight ? LaneWidth-C.Width-3. : 3.) : (bToRight ? LaneWidth : -C.Width);
        double X = Middle;
        if (T < Slide) X = FMath::Lerp(Start,Middle,FMath::SmoothStep(0.,1.,T/Slide));
        else if (T > Slide+C.ReadSeconds) X = FMath::Lerp(Middle,Exit,FMath::SmoothStep(0.,1.,(T-Slide-C.ReadSeconds)/Slide));
        return {C.Text,X,bLast && T >= End};
    }
    return {};
}

int32 SAquilaFiringRibbon::OnPaint(const FPaintArgs&, const FGeometry& G, const FSlateRect&, FSlateWindowElementList& Out,
    int32 Layer, const FWidgetStyle&, bool) const
{
    Layout(G.GetLocalSize().X);
    const auto Frame = FrameAt(Time.Get(),ToRight.Get());
    if (Frame.Text.IsEmpty()) return Layer;
    FSlateDrawElement::MakeLines(Out,Layer,G.ToPaintGeometry(),{FVector2f(0,24),FVector2f(LaneWidth,24)},
        ESlateDrawEffect::None,FLinearColor(.18f,.51f,.59f,.66f),true,1.);
    const auto TextAt = [&](double X, double Y) {
        return G.ToPaintGeometry(G.GetLocalSize(),FSlateLayoutTransform(FVector2f(X,Y)));
    };
    FSlateDrawElement::MakeText(Out,Layer+1,TextAt(Frame.X+1,2),Frame.Text,Font,ESlateDrawEffect::None,FLinearColor::Black);
    FSlateDrawElement::MakeText(Out,Layer+2,TextAt(Frame.X,1),Frame.Text,Font,ESlateDrawEffect::None,FLinearColor(.87f,.95f,1.f));
    return Layer+2;
}

void AAquilaFlightPawn::AddFiringReadout()
{
    auto Visible = [this]() { return FiringPreview ? EVisibility::HitTestInvisible : EVisibility::Collapsed; };
    auto ToRight = [this]() { return FiringPreview && TurretPreviewUV(FiringPreview->Gun.Weapon).X > .5; };
    MountCanvas->AddSlot().Position_Lambda([this,ToRight]() {
        if (!FiringPreview) return FVector2D::ZeroVector;
        const FVector2D Size = MountCanvas->GetCachedGeometry().GetLocalSize();
        const auto UV = TurretPreviewUV(FiringPreview->Gun.Weapon);
        return FVector2D(ToRight() ? Size.X*(UV.X+.052) : 6., Size.Y*UV.Y-10.);
    }).Size_Lambda([this]() { return FVector2D(MountCanvas->GetCachedGeometry().GetLocalSize().X*.118-6.,30.); })
        [SAssignNew(FiringRibbon,SAquilaFiringRibbon).Visibility_Lambda(Visible)
            .Description_Lambda([this](){ return FiringPreview ? FiringPreview->Description : FString(); })
            .Time_Lambda([this](){ return FiringPreview ? FiringPreview->DescriptionAge : 0.; }).ToRight_Lambda(ToRight)];
    static const FProgressBarStyle Bars = FProgressBarStyle()
        .SetBackgroundImage(FSlateColorBrush(FLinearColor(.07f,.13f,.17f)))
        .SetFillImage(FSlateColorBrush(FLinearColor::White));
    auto Meter = [this](bool bShield) {
        return SNew(SHorizontalBox)
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(1,0,4,0)
                [SNew(STextBlock).Text(bShield ? INVTEXT("Shield") : INVTEXT("Hull")).Font(FCoreStyle::GetDefaultFontStyle("Regular",12))]
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)[SNew(SBox).WidthOverride(53).HeightOverride(5)
                [SNew(SProgressBar).Style(&Bars).BarFillStyle(EProgressBarFillStyle::Scale).BorderPadding(FVector2D::ZeroVector)
                    .FillColorAndOpacity(bShield ? FLinearColor(.12f,.75f,1.f) : FLinearColor(1.f,.48f,.17f))
                    .Percent_Lambda([this,bShield]() -> TOptional<float> {
                        if (!FiringPreview) return 0;
                        const auto& D = *FiringPreview;
                        return bShield ? D.Range.EnemyShield/D.ShieldCapacity : D.Range.EnemyHull/D.HullCapacity;
                    })]];
    };
    MountCanvas->AddSlot().HAlign(HAlign_Center).VAlign(VAlign_Top)
        .Position_Lambda([this]() { return FVector2D(.5,.245) * FVector2D(MountCanvas->GetCachedGeometry().GetLocalSize()); })
        .Size_Lambda([this]() { return FVector2D(MountCanvas->GetCachedGeometry().GetLocalSize().X*.47,60.); })
        [SNew(SVerticalBox).Visibility_Lambda(Visible)
            + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
                [SNew(STextBlock).Font(FCoreStyle::GetDefaultFontStyle("Bold",15))
                    .Text_Lambda([this]() { return FiringPreview ? FText::FromString(FString::Printf(TEXT("%s  ·  %s"),FiringPreview->Gun.RoundsName(),*FiringPreview->Outcome())) : FText::GetEmpty(); })]
            + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0,2)[SNew(SHorizontalBox)
                + SHorizontalBox::Slot().AutoWidth().Padding(0,0,9,0)[Meter(true)]
                + SHorizontalBox::Slot().AutoWidth()[Meter(false)]]];
}
