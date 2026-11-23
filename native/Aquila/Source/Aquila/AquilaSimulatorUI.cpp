#include "AquilaFlightPawn.h"
#include "AquilaGameMode.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

namespace
{
const FLinearColor PanelColor(0.015f, 0.032f, 0.051f, 0.96f);
const FLinearColor Bright(0.87f, 0.94f, 0.98f);
const FLinearColor Cyan(0.38f, 0.84f, 0.90f);

TSharedRef<STextBlock> Label(TAttribute<FText> Text, int32 Size, bool bHeading = false)
{
    return SNew(STextBlock).Text(Text)
        .Font(FCoreStyle::GetDefaultFontStyle(bHeading ? "Bold" : "Regular", Size))
        .ColorAndOpacity(bHeading ? Cyan : Bright);
}

TSharedRef<SButton> Action(TAttribute<FText> Text, FOnClicked Click, const FText& Tip = FText::GetEmpty())
{
    return SNew(SButton).HAlign(HAlign_Center).VAlign(VAlign_Center).ContentPadding(FMargin(13, 8))
        .ButtonColorAndOpacity(FLinearColor(0.10f, 0.25f, 0.31f)).ToolTipText(Tip).OnClicked(Click)[Label(Text, 15)];
}
}

TSharedRef<SWidget> AAquilaFlightPawn::BuildModeMenu()
{
    return SNew(SBorder).BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush")).BorderBackgroundColor(PanelColor).Padding(24)
        .Visibility_Lambda([this]() { return Game() && Game()->Session() == EAquilaSession::Menu ? EVisibility::Visible : EVisibility::Collapsed; })
        [SNew(SBox).WidthOverride(350)[SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight()[Label(INVTEXT("Aquila"), 36, true)]
            + SVerticalBox::Slot().AutoHeight().Padding(0, 16, 0, 0)[Action(INVTEXT("Play Match"),
                FOnClicked::CreateLambda([this]() {
                    Game()->PlayMatch();
                    ResetSessionView();
                    return FReply::Handled();
                }))]
            + SVerticalBox::Slot().AutoHeight().Padding(0, 10, 0, 0)[Action(INVTEXT("Combat Simulator"),
                FOnClicked::CreateLambda([this]() {
                    Game()->StartDuel(false);
                    ResetSessionView();
                    return FReply::Handled();
                }))]
            + SVerticalBox::Slot().AutoHeight().Padding(0, 10, 0, 0)[Label(INVTEXT("Try a duel outside your saved match."), 14)]]];
}

TSharedRef<SWidget> AAquilaFlightPawn::BuildDuelPanel()
{
    auto Ammo = [this](bool bPiercing)
    {
        return Action(TAttribute<FText>::CreateLambda([this, bPiercing]() {
            const bool bCurrent = Game() && Game()->Campaign.bPiercing == bPiercing;
            if (bPiercing) return bCurrent ? INVTEXT("Replay piercing") : INVTEXT("Try piercing");
            return bCurrent ? INVTEXT("Replay standard") : INVTEXT("Try standard");
        }), FOnClicked::CreateLambda([this, bPiercing]() {
            Game()->StartDuel(bPiercing);
            ResetSessionView();
            return FReply::Handled();
        }), INVTEXT("Restart both ships with this ammunition. Both rounds are available in the match Tactical panel."));
    };
    auto Health = [this](bool bSentinel)
    {
        return Label(TAttribute<FText>::CreateLambda([this, bSentinel]() {
            if (!Game()) return FText::GetEmpty();
            const auto& C = Game()->Campaign;
            return FText::FromString(FString::Printf(TEXT("%s    Shield %.1f    Hull %.1f"),
                bSentinel ? TEXT("Sentinel") : TEXT("Resolute"),
                bSentinel ? C.EnemyShield : C.Shield, bSentinel ? C.EnemyHull : C.Hull));
        }), 16);
    };
    return SNew(SBorder).BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush")).BorderBackgroundColor(PanelColor).Padding(FMargin(16, 10))
        .Visibility_Lambda([this]() { return Game() && Game()->Session() == EAquilaSession::Simulator ? EVisibility::Visible : EVisibility::Collapsed; })
        [SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight()[SNew(SHorizontalBox)
                + SHorizontalBox::Slot().AutoWidth()[Label(INVTEXT("Combat Simulator"), 18, true)]
                + SHorizontalBox::Slot().FillWidth(1).HAlign(HAlign_Right).VAlign(VAlign_Center)[Label(TAttribute<FText>::CreateLambda([this]() {
                    if (!Game()) return FText::GetEmpty();
                    const auto& C = Game()->Campaign;
                    return FText::FromString(FString::Printf(TEXT("%s rounds   ·   %.0f s"),
                        C.bPiercing ? TEXT("Piercing") : TEXT("Standard"), C.BattleTime));
                }), 14)]]
            + SVerticalBox::Slot().AutoHeight().Padding(0, 6)[SNew(SHorizontalBox)
                + SHorizontalBox::Slot().FillWidth(1).Padding(0,0,16,0)[Health(false)]
                + SHorizontalBox::Slot().AutoWidth()[Health(true)]]
            + SVerticalBox::Slot().AutoHeight()[Label(TAttribute<FText>::CreateLambda([this]() {
                if (!Game()) return FText::GetEmpty();
                const auto& C = Game()->Campaign;
                if (C.Hull <= 0) return C.bArmorBlocked ? INVTEXT("Resolute disabled. Sentinel’s armor held.") : INVTEXT("Resolute disabled.");
                if (C.EnemyHull <= 0) return INVTEXT("Sentinel disabled.");
                if (C.bArmorBlocked) return INVTEXT("Sentinel’s armor stopped the standard rounds.");
                if (C.EnemyHull < 100) return INVTEXT("Piercing rounds are reaching Sentinel’s hull.");
                return INVTEXT("Automatic turrets engaging Sentinel.");
            }), 14)]
            + SVerticalBox::Slot().AutoHeight().Padding(0, 8, 0, 0)[SNew(SHorizontalBox)
                + SHorizontalBox::Slot().AutoWidth().Padding(0,0,5,0)[Ammo(false)]
                + SHorizontalBox::Slot().AutoWidth().Padding(0,0,5,0)[Ammo(true)]
                + SHorizontalBox::Slot().AutoWidth().Padding(0,0,5,0)[Action(TAttribute<FText>::CreateLambda([this]() {
                    return Game() && Game()->bFastDuel ? INVTEXT("Speed 8×") : INVTEXT("Speed 1×");
                }), FOnClicked::CreateLambda([this]() { Game()->bFastDuel = !Game()->bFastDuel; return FReply::Handled(); }))]
                + SHorizontalBox::Slot().AutoWidth().Padding(0,0,5,0)[Action(TAttribute<FText>::CreateLambda([this]() {
                    return Game() && Game()->bPaused ? INVTEXT("Resume") : INVTEXT("Pause");
                }), FOnClicked::CreateLambda([this]() { Game()->Command(8); return FReply::Handled(); }))]
                + SHorizontalBox::Slot().AutoWidth()[Action(INVTEXT("Main menu"), FOnClicked::CreateLambda([this]() {
                    Game()->ReturnToMenu();
                    ResetSessionView();
                    return FReply::Handled();
                }))]]];
}
