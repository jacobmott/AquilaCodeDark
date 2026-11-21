#include "SAquilaMouseLook.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SViewport.h"

namespace
{
bool IsLookButton(FKey Key)
{
    return Key == EKeys::LeftMouseButton || Key == EKeys::RightMouseButton;
}
}

FReply SAquilaMouseLook::OnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event)
{
    if (Event.IsTouchEvent() || !IsLookButton(Event.GetEffectingButton())) return FReply::Unhandled();
    if (bDragging) return FReply::Handled();
    bDragging = true;
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
    OnDrag.ExecuteIfBound(Event.GetCursorDelta());
    return FReply::Handled();
}

FReply SAquilaMouseLook::OnMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& Event)
{
    const FKey Button = Event.GetEffectingButton();
    if (!bDragging || Event.IsTouchEvent() || !IsLookButton(Button)) return FReply::Unhandled();
    const FKey Other = Button == EKeys::LeftMouseButton ? EKeys::RightMouseButton : EKeys::LeftMouseButton;
    if (Event.IsMouseButtonDown(Other)) return FReply::Handled();
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
