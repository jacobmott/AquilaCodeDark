#include "SAquilaMouseLook.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SViewport.h"

namespace
{
bool IsLookButton(FKey Key)
{
    return Key == EKeys::LeftMouseButton || Key == EKeys::RightMouseButton || Key == EKeys::MiddleMouseButton;
}
}

FReply SAquilaMouseLook::OnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event)
{
    if (Event.IsTouchEvent() || !IsLookButton(Event.GetEffectingButton())) return FReply::Unhandled();
    if (bDragging) { bMoved = true; return FReply::Handled(); }
    bDragging = true;
    bMoved = false;
    PressButton = Event.GetEffectingButton();
    PendingDelta = FVector2D::ZeroVector;
    PressUV = Geometry.AbsoluteToLocal(Event.GetScreenSpacePosition()) / Geometry.GetLocalSize();
    CursorAtPress = FVector2D(Event.GetScreenSpacePosition()).IntPoint();
    FReply Reply = FReply::Handled().UseHighPrecisionMouseMovement(AsShared());
    if (const auto Viewport = FSlateApplication::Get().GetGameViewport())
        Reply.SetUserFocus(Viewport.ToSharedRef(), EFocusCause::Mouse);
    return Reply;
}

FReply SAquilaMouseLook::OnMouseButtonDoubleClick(const FGeometry& Geometry, const FPointerEvent& Event)
{
    return OnMouseButtonDown(Geometry, Event);
}

FReply SAquilaMouseLook::OnMouseMove(const FGeometry& Geometry, const FPointerEvent& Event)
{
    if (!bDragging || Event.IsTouchEvent()) return FReply::Unhandled();
    PendingDelta += Event.GetCursorDelta();
    if (bMoved || PendingDelta.Size() >= FSlateApplication::Get().GetDragTriggerDistance())
    {
        bMoved = true;
        OnDrag.ExecuteIfBound(PendingDelta, PressButton);
        PendingDelta = FVector2D::ZeroVector;
    }
    return FReply::Handled();
}

FReply SAquilaMouseLook::OnMouseWheel(const FGeometry& Geometry, const FPointerEvent& Event)
{
    OnZoom.ExecuteIfBound(Event.GetWheelDelta());
    return FReply::Handled();
}

FReply SAquilaMouseLook::OnMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& Event)
{
    const FKey Button = Event.GetEffectingButton();
    if (!bDragging || Event.IsTouchEvent() || Button != PressButton) return FReply::Unhandled();
    if (!bMoved && Button == EKeys::LeftMouseButton) OnPick.ExecuteIfBound(PressUV);
    bDragging = false;
    return FReply::Handled().ReleaseMouseCapture().SetMousePos(CursorAtPress);
}

void SAquilaMouseLook::OnMouseCaptureLost(const FCaptureLostEvent& Event)
{
    bDragging = false;
    SLeafWidget::OnMouseCaptureLost(Event);
}

FCursorReply SAquilaMouseLook::OnCursorQuery(const FGeometry& Geometry, const FPointerEvent& Event) const
{
    return bDragging ? FCursorReply::Cursor(EMouseCursor::None) : FCursorReply::Unhandled();
}
