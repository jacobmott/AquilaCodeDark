#include "AquilaFlightPawn.h"
#include "AquilaGameMode.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Brushes/SlateColorBrush.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SCanvas.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Engine/GameViewportClient.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

namespace
{
const FLinearColor PanelColor(0.015f, 0.032f, 0.051f, 0.96f);
const FLinearColor Bright(0.87f, 0.94f, 0.98f);
const FLinearColor Cyan(0.38f, 0.84f, 0.90f);

const TCHAR* WeaponName(EAquilaWeapon W)
{
    return AquilaWeapons::Models[int32(W)].Name;
}

const TCHAR* ArmorName(EAquilaArmor Armor)
{
    const auto Plate = AquilaDefenses::Plate(Armor);
    return Plate ? AquilaDefenses::Models[int32(Plate.GetValue())].Name : TEXT("Bare hull");
}

FString BatterySummary(const FAquilaShipLoadout& Fit)
{
    if (!Fit.Rack) return {};
    const FAquilaTurret* First = nullptr;
    int32 Total = 0;
    bool bMixed = false;
    for (const auto& T : Fit.Rack->Stock)
    {
        if (T.Mount == INDEX_NONE) continue;
        ++Total;
        if (!First) First = &T;
        bMixed |= T.Weapon != First->Weapon || FCString::Strcmp(T.RoundsName(), First->RoundsName()) != 0;
    }
    if (!First) return TEXT("Empty mounts");
    if (bMixed) return FString::Printf(TEXT("%d turrets · Mixed loadout"), Total);
    return FString::Printf(TEXT("%d × %s · %s"), Total, WeaponName(First->Weapon), First->RoundsName());
}

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
                    Game()->StartDuel();
                    ResetSessionView();
                    return FReply::Handled();
                }))]
            + SVerticalBox::Slot().AutoHeight().Padding(0, 10, 0, 0)[Action(INVTEXT("Boarding Test"),
                FOnClicked::CreateLambda([this]() {
                    Game()->TravelToBoarding(true);
                    return FReply::Handled();
                }), INVTEXT("Board Glacier Tongue or Delta Tree."))]]];
}

TSharedRef<SWidget> AAquilaFlightPawn::BuildDuelPanel()
{
    static const FProgressBarStyle MeterStyle = FProgressBarStyle()
        .SetBackgroundImage(FSlateColorBrush(FLinearColor(0.012f,0.024f,0.036f)))
        .SetFillImage(FSlateColorBrush(FLinearColor::White));
    static const FNumberFormattingOptions PercentFormat = FNumberFormattingOptions().SetMaximumFractionalDigits(1);
    auto ShieldShape = [this](bool bMesh)
    {
        return SNew(SButton).ContentPadding(FMargin(10, 4)).HAlign(HAlign_Center)
            .ButtonColorAndOpacity_Lambda([this, bMesh]() -> FSlateColor {
                return bDuelMeshShield == bMesh ? FLinearColor(0.17f, 0.66f, 0.73f) : FLinearColor(0.10f, 0.22f, 0.27f);
            })
            .ToolTipText(bMesh ? INVTEXT("The ship’s own mesh, scaled to 108%.") : INVTEXT("A smooth oval around the whole ship."))
            .OnClicked_Lambda([this, bMesh]() { SetDuelMeshShield(bMesh); return FReply::Handled(); })
            [Label(bMesh ? INVTEXT("Ship mesh") : INVTEXT("Oval"), 14)];
    };
    auto Health = [this](bool bSentinel)
    {
        auto Meter = [this, bSentinel](bool bShield)
        {
            auto Level = [this, bSentinel, bShield]() -> float
            {
                if (!Game()) return 0;
                const auto& C = Game()->Campaign;
                if (bSentinel) return bShield ? C.EnemyShield : C.EnemyHull;
                return bShield ? C.Shield : C.Hull;
            };
            const FLinearColor Color = bShield ? FLinearColor(0.13f,0.77f,1.f) : FLinearColor(1.f,0.47f,0.19f);
            return SNew(SHorizontalBox)
                + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)[SNew(SBox).WidthOverride(58)
                    [Label(bShield ? INVTEXT("Shield") : INVTEXT("Hull"), 13)]]
                + SHorizontalBox::Slot().FillWidth(1).VAlign(VAlign_Center)[SNew(SBox).HeightOverride(8)
                    [SNew(SProgressBar).Style(&MeterStyle).BarFillStyle(EProgressBarFillStyle::Scale).BorderPadding(FVector2D::ZeroVector)
                        .FillColorAndOpacity(Color).Percent_Lambda([Level]() -> TOptional<float> { return Level() / 100.f; })]]
                + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(6,0,0,0)[SNew(SBox).WidthOverride(56).HAlign(HAlign_Right)
                    [Label(TAttribute<FText>::CreateLambda([Level]() { return FText::AsPercent(Level() / 100., &PercentFormat); }), 13)]];
        };
        return SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight()[Label(bSentinel ? INVTEXT("Glacier Tongue") : INVTEXT("Delta Tree"), 15, true)]
            + SVerticalBox::Slot().AutoHeight()[Label(TAttribute<FText>::CreateLambda([this, bSentinel]() {
                if (!Game()) return FText::GetEmpty();
                const auto& Fit = Game()->Campaign.Loadout.ForShip(bSentinel);
                return FText::FromString(BatterySummary(Fit));
            }), 14)]
            + SVerticalBox::Slot().AutoHeight()[Label(TAttribute<FText>::CreateLambda([this, bSentinel]() {
                if (!Game()) return FText::GetEmpty();
                const auto& Fit = Game()->Campaign.Loadout.ForShip(bSentinel);
                const auto& Stats = Game()->Campaign.DuelStats;
                const TCHAR* Shield = Fit.bShield ? TEXT("Standard shield") : TEXT("No shield");
                if (Fit.bShield && Fit.bReserveCapacitor && Stats)
                    Shield = Stats->bReserveSpent[bSentinel] ? TEXT("Shield · Reserve spent") : TEXT("Shield · Reserve ready");
                return FText::FromString(FString::Printf(TEXT("%s  ·  %s"), ArmorName(Fit.Armor), Shield));
            }), 13)]
            + SVerticalBox::Slot().AutoHeight().Padding(0,2)[Meter(true)]
            + SVerticalBox::Slot().AutoHeight()[Meter(false)]
            + SVerticalBox::Slot().AutoHeight().Padding(0,3,0,0)[Label(TAttribute<FText>::CreateLambda([this, bSentinel]() {
                if (!Game() || !Game()->Campaign.DuelStats) return FText::GetEmpty();
                const auto& C = Game()->Campaign;
                const auto& T = C.DuelStats->Shots[bSentinel ? 1 : 0];
                return FText::FromString(FString::Printf(TEXT("Hits %d  ·  Blocked %d  ·  Intercepted %d"), T.Hits, T.Blocked, T.Intercepted));
            }), 13)];
    };
    return SNew(SBorder).BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush")).BorderBackgroundColor(PanelColor).Padding(FMargin(16, 10))
        .Visibility_Lambda([this]() { return Game() && Game()->Session() == EAquilaSession::Simulator && !Game()->bFittingOpen ?
            EVisibility::Visible : EVisibility::Collapsed; })
        [SNew(SBox).MinDesiredWidth(770)[SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight()[SNew(SHorizontalBox)
                + SHorizontalBox::Slot().FillWidth(1).VAlign(VAlign_Center)[Label(INVTEXT("Combat Simulator"), 18, true)]
                + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0,0,6,0)[Label(INVTEXT("Shield shape"), 14)]
                + SHorizontalBox::Slot().AutoWidth().Padding(0,0,3,0)[ShieldShape(true)]
                + SHorizontalBox::Slot().AutoWidth()[ShieldShape(false)]
                + SHorizontalBox::Slot().FillWidth(1).HAlign(HAlign_Right).VAlign(VAlign_Center)[Label(TAttribute<FText>::CreateLambda([this]() {
                    if (!Game()) return FText::GetEmpty();
                    const auto& C = Game()->Campaign;
                    return FText::FromString(FString::Printf(TEXT("%.0f s"), C.BattleTime));
                }), 14)]]
            + SVerticalBox::Slot().AutoHeight().Padding(0, 6)[SNew(SHorizontalBox)
                + SHorizontalBox::Slot().FillWidth(1).Padding(0,0,22,0)[Health(false)]
                + SHorizontalBox::Slot().FillWidth(1)[Health(true)]]
            + SVerticalBox::Slot().AutoHeight()[Label(TAttribute<FText>::CreateLambda([this]() {
                if (!Game()) return FText::GetEmpty();
                const auto& C = Game()->Campaign;
                if (C.Hull <= 0 && C.EnemyHull <= 0) return INVTEXT("Both ships destroyed.");
                if (C.Hull <= 0 || C.EnemyHull <= 0)
                {
                    const bool bSentinelLost = C.EnemyHull <= 0;
                    const bool bIncoming = C.Rounds.ContainsByPredicate([bSentinelLost](const FAquilaRound& R) { return R.bEnemy == bSentinelLost; });
                    if (bSentinelLost) return bIncoming ? INVTEXT("Glacier Tongue destroyed. Incoming rounds still in flight.") : INVTEXT("Glacier Tongue destroyed.");
                    return bIncoming ? INVTEXT("Delta Tree destroyed. Incoming rounds still in flight.") : INVTEXT("Delta Tree destroyed.");
                }
                const bool ResoluteArmed = C.Loadout.Resolute.Rack->Installed() > 0;
                const bool SentinelArmed = C.Loadout.Sentinel.Rack->Installed() > 0;
                if (!ResoluteArmed && !SentinelArmed) return INVTEXT("Both ships have empty mounts.");
                if (C.bArmorBlocked && C.bEnemyArmorBlocked) return INVTEXT("Stalemate. Neither side has an effective hull attack.");
                if (!ResoluteArmed) return INVTEXT("Delta Tree has no turrets installed.");
                if (!SentinelArmed) return INVTEXT("Glacier Tongue has no turrets installed.");
                if (C.bArmorBlocked || C.bEnemyArmorBlocked)
                {
                    const bool Enemy = C.bEnemyArmorBlocked;
                    const TCHAR* Shooter = Enemy ? TEXT("Glacier Tongue") : TEXT("Delta Tree");
                    FString Ammo;
                    for (const auto& Gun : C.Loadout.ForShip(Enemy).Rack->Stock)
                    {
                        if (Gun.Mount == INDEX_NONE) continue;
                        const FString Name = FString(Gun.RoundsName()).ToLower();
                        if (Ammo.IsEmpty()) Ammo = Name;
                        else if (Ammo != Name) { Ammo = TEXT("rounds"); break; }
                    }
                    return FText::FromString(FString::Printf(TEXT("%s stops %s’s %s."),
                        ArmorName(C.Loadout.ForShip(!Enemy).Armor), Shooter, *Ammo));
                }
                return INVTEXT("Both ships engaging.");
            }), 14)]
            + SVerticalBox::Slot().AutoHeight().Padding(0, 8, 0, 0)[SNew(SHorizontalBox)
                + SHorizontalBox::Slot().AutoWidth().Padding(0,0,5,0)[Action(TAttribute<FText>::CreateLambda([this]() {
                    return Game() && Game()->Campaign.Hull<=0 ? INVTEXT("Replay aboard") : INVTEXT("Watch from bridge");
                }), FOnClicked::CreateLambda([this]() {
                    if (Game()->Campaign.Hull<=0) StartDuelAboard();
                    else EnterDuelBridge();
                    return FReply::Handled();
                }), INVTEXT("N switches ships while aboard. Tab returns to the arena or the last surviving bridge."))]
                + SHorizontalBox::Slot().AutoWidth().Padding(0,0,5,0)[Action(INVTEXT("Configure ship"), FOnClicked::CreateLambda([this]() {
                    bMoveHere = false;
                    ClearFiringSelection();
                    SelectedPart = bFitEnemy ? 14 : 6;
                    Game()->bFittingOpen = true;
                    return FReply::Handled();
                }))]
                + SHorizontalBox::Slot().AutoWidth().Padding(0,0,5,0)[Action(INVTEXT("Replay duel"), FOnClicked::CreateLambda([this]() {
                    Game()->StartDuel(Game()->Campaign.Loadout);
                    ResetSessionView();
                    return FReply::Handled();
                }))]
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
                }))]]]];
}

const FAquilaShipLoadout* AAquilaFlightPawn::EditingShip() const
{
    auto* G = Game();
    if (!G || !G->bFittingOpen) return nullptr;
    return &G->Campaign.Loadout.ForShip(bFitEnemy);
}

void AAquilaFlightPawn::EditFitting(TFunctionRef<void(FAquilaShipLoadout&)> Edit)
{
    auto* G = Game();
    if (G && G->bFittingOpen) G->Campaign.EditDuelShip(bFitEnemy, Edit);
}

void AAquilaFlightPawn::EditFittedTurret(TFunctionRef<void(FAquilaTurret&)> Edit)
{
    if (SelectedPart < 0) return;
    EditFitting([this,Edit](FAquilaShipLoadout& Ship) {
        const int32 Item = Ship.Rack->At(SelectedPart);
        if (Item != INDEX_NONE) Edit(Ship.Rack->Stock[Item]);
    });
}

FVector2D AAquilaFlightPawn::FittingPin(int32 Part) const
{
    auto* PC = Cast<APlayerController>(GetController());
    if (!PC || !MountCanvas || !EditingShip()) return FVector2D(-1000);
    const auto* Root = bFitEnemy ? EnemyRoot.Get() : ShipRoot.Get();
    const auto& Layout=AquilaHulls::Layout(bFitEnemy);
    const FVector Local=(Part>=0 ? Layout.Guns[Part] : Part==ArmorPart ? Layout.Armor : Layout.Shield).GetLocation();
    FVector2D Pixel;
    int32 Width, Height;
    PC->GetViewportSize(Width, Height);
    if (!Width || !Height || !PC->ProjectWorldLocationToScreen(Root->GetComponentTransform().TransformPosition(Local), Pixel))
        return FVector2D(-1000);
    return Pixel / FVector2D(Width, Height) * FVector2D(MountCanvas->GetCachedGeometry().GetLocalSize());
}

void AAquilaFlightPawn::SelectPart(int32 Part)
{
    auto* Fit = EditingShip();
    if (!Fit || !Fit->Rack) return;
    if (bMoveHere && Part >= 0)
    {
        EditFitting([this,Part](FAquilaShipLoadout& Ship) {
            if (Ship.Rack->Place(Ship.Rack->At(Part), SelectedPart)) bMoveHere = false;
        });
        return;
    }
    bMoveHere = false;
    SelectedPart = Part;
    if (Part < 0) { ClearFiringSelection(); return; }
    bInspectPort = true;
    if (PlacementWeapon)
    {
        EditFitting([this,Part](FAquilaShipLoadout& Ship) {
            if (Ship.Rack->Fit(Part, PlacementWeapon.GetValue()) && PlacementAmmo)
            {
                Ship.Rack->Stock[Ship.Rack->At(Part)].Ammo = PlacementAmmo.GetValue();
                PlacementAmmo.Reset();
            }
        });
    }
}

TSharedRef<SWidget> AAquilaFlightPawn::BuildFittingView()
{
    const FLinearColor Idle(.055f,.10f,.14f), Equipped(.03f,.38f,.42f), Selected(.78f,.36f,.07f);
    auto Item = [this]() { return FittedTurret(); };
    SAssignNew(MountCanvas, SCanvas).Visibility(EVisibility::SelfHitTestInvisible);
    auto Choice = [this, Selected, Idle](FVector2D UV, FText Name, TAttribute<EVisibility> Visibility,
        TAttribute<bool> IsFitted, TAttribute<FText> Status, FOnClicked Pick, FText Tip,
        FSimpleDelegate Enter = {}, FSimpleDelegate Leave = {},
        FVector2D HitSize = FVector2D(.22,.25), double CaptionOffset = .022) {
        MountCanvas->AddSlot().HAlign(HAlign_Center).VAlign(VAlign_Center)
            .Position_Lambda([this, UV, CaptionOffset]() { return (UV + FVector2D(0,CaptionOffset)) * FVector2D(MountCanvas->GetCachedGeometry().GetLocalSize()); })
            .Size_Lambda([this, HitSize]() { return HitSize * FVector2D(MountCanvas->GetCachedGeometry().GetLocalSize()); })
            [SNew(SButton).ButtonStyle(FCoreStyle::Get(), "NoBorder").ContentPadding(0).Visibility(Visibility)
                .HAlign(HAlign_Center).VAlign(VAlign_Bottom).Cursor(EMouseCursor::Hand).OnClicked(Pick).OnHovered(Enter).OnUnhovered(Leave).ToolTipText(Tip)
                [SNew(SVerticalBox)
                    + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)[Label(Name, 18, true)]
                    + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)[Label(Status, 13)]
                    + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0,3)[SNew(SBox).WidthOverride(80).HeightOverride(3)
                        [SNew(SBorder).Padding(0).BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                            .BorderBackgroundColor_Lambda([IsFitted, Selected, Idle]() -> FSlateColor { return IsFitted.Get() ? Selected : Idle; })]]]];
    };
    for (const auto& Model : AquilaWeapons::Models)
    {
        const auto Weapon = Model.Weapon;
        Choice(TurretPreviewUV(Weapon), FText::FromString(Model.Name),
            TAttribute<EVisibility>::CreateLambda([this]() { return SelectedPart >= 0 ? EVisibility::Visible : EVisibility::Collapsed; }),
            TAttribute<bool>::CreateLambda([this,Weapon]() { return PlacementWeapon && PlacementWeapon.GetValue() == Weapon; }),
            FText::GetEmpty(), FOnClicked::CreateLambda([this,Weapon]() {
                PlacementAmmo.Reset();
                bInspectPort = false;
                if (PlacementWeapon && PlacementWeapon.GetValue() == Weapon) PlacementWeapon.Reset();
                else PlacementWeapon = Weapon;
                bMoveHere = false;
                return FReply::Handled();
            }), FText::GetEmpty(),
            FSimpleDelegate::CreateLambda([this,Weapon]() { HoveredWeapon = Weapon; }),
            FSimpleDelegate::CreateLambda([this,Weapon]() { if (HoveredWeapon == Weapon) HoveredWeapon.Reset(); }));
    }
    for (const auto& Model : AquilaDefenses::Models)
    {
        const auto Type = Model.Type;
        auto Fitted = TAttribute<bool>::CreateLambda([this,Type]() {
            const auto* F = EditingShip();
            if (!F) return false;
            if (AquilaDefenses::IsPlating(Type)) return AquilaDefenses::Plate(F->Armor) == Type;
            if (Type == EAquilaDefense::ShieldEmitter) return F->bShield;
            return Type == EAquilaDefense::ReserveCapacitor ? F->bReserveCapacitor : !F->bShield;
        });
        Choice(DefensePreviewUV(Type), FText::FromString(Model.Name),
            TAttribute<EVisibility>::CreateLambda([this,Type]() { return DefenseGalleryVisible(Type) ? EVisibility::Visible : EVisibility::Collapsed; }),
            Fitted, TAttribute<FText>::CreateLambda([this,Type,Fitted]() {
                const auto* F = EditingShip();
                if (!F) return FText::GetEmpty();
                if (Type == EAquilaDefense::ReserveCapacitor)
                {
                    if (!F->bShield) return INVTEXT("Fit a standard shield first");
                    return Fitted.Get() ? INVTEXT("Fitted · Click to remove") : INVTEXT("One full recharge per duel");
                }
                if (Fitted.Get()) return INVTEXT("Fitted");
                return Type == EAquilaDefense::EmptySocket ? INVTEXT("Remove shield") : INVTEXT("Click to fit");
            }), FOnClicked::CreateLambda([this,Type]() {
                EditFitting([Type](FAquilaShipLoadout& Ship) {
                    if (AquilaDefenses::IsPlating(Type)) Ship.Armor = static_cast<EAquilaArmor>(Type);
                    else if (Type == EAquilaDefense::ReserveCapacitor) { if (Ship.bShield) Ship.bReserveCapacitor = !Ship.bReserveCapacitor; }
                    else
                    {
                        Ship.bShield = Type == EAquilaDefense::ShieldEmitter;
                        if (!Ship.bShield) Ship.bReserveCapacitor = false;
                    }
                });
                return FReply::Handled();
            }), Type == EAquilaDefense::ReserveCapacitor ? INVTEXT("Restores 100 shield once when depleted. Excess damage uses the new charge.") : FText::GetEmpty());
    }
    for (int32 Part = ShieldPart; Part < FAquilaTurret::MountCount; ++Part)
    {
        if (Part == INDEX_NONE) continue;
        const auto Name = FText::FromString(Part == ArmorPart ? FString(TEXT("Armor")) : Part == ShieldPart ? FString(TEXT("Shield")) :
            FString::Printf(TEXT("%02d"), Part + 1));
        auto Pin = SNew(SButton).ButtonStyle(FCoreStyle::Get(), "NoBorder").ContentPadding(0).HAlign(HAlign_Center)
            .VAlign(Part >= 0 ? VAlign_Bottom : VAlign_Center).Cursor(EMouseCursor::Hand)
            .OnClicked_Lambda([this, Part]() { SelectPart(Part); return FReply::Handled(); });
        const TWeakPtr<SButton> Hover = Pin;
        Pin->SetContent(SNew(SBorder).BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush")).Padding(FMargin(6,1))
            .Visibility_Lambda([this, Part, Hover]() {
                const auto Button = Hover.Pin();
                return Part < 0 || SelectedPart >= 0 || (Button && Button->IsHovered()) ? EVisibility::HitTestInvisible : EVisibility::Hidden;
            })
            .BorderBackgroundColor_Lambda([this, Part, Idle, Equipped, Selected]() -> FSlateColor {
                if (SelectedPart == Part) return Selected;
                auto* F = EditingShip();
                return F && F->Rack && Part >= 0 && F->Rack->At(Part) != INDEX_NONE ? Equipped : Idle;
            })[Label(Name, 14)]);
        MountCanvas->AddSlot().Position_Lambda([this, Part]() { return FittingPin(Part); })
            .Size(Part >= 0 ? FVector2D(48,52) : FVector2D(78,44)).HAlign(HAlign_Center).VAlign(VAlign_Center)[Pin];
    }
    auto ShipTab = [this, Equipped, Idle](bool bEnemy) {
        return SNew(SButton).ContentPadding(FMargin(14,6))
            .ButtonColorAndOpacity_Lambda([this,bEnemy,Equipped,Idle]() -> FSlateColor { return bFitEnemy == bEnemy ? Equipped : Idle; })
            .OnClicked_Lambda([this,bEnemy]() {
                bFitEnemy = bEnemy;
                SelectedPart = bEnemy ? 14 : 6;
                ClearFiringSelection();
                bMoveHere = false;
                return FReply::Handled();
            })[Label(bEnemy ? INVTEXT("Glacier Tongue") : INVTEXT("Delta Tree"), 16)];
    };
    auto Clear = Action(INVTEXT("Clear fitting"), FOnClicked::CreateLambda([this]() {
        EditFitting([](FAquilaShipLoadout& Ship) { Ship.ClearFitting(); });
        ClearFiringSelection();
        bMoveHere = false;
        return FReply::Handled();
    }), INVTEXT("Immediately strip this ship to its bare hull and return its turret bodies to stock."));
    auto Borrow = Action(INVTEXT("Borrow full lab stock"), FOnClicked::CreateLambda([this]() {
        EditFitting([](FAquilaShipLoadout& Ship) { Ship.Rack->Stock.SetNum(FAquilaTurret::MountCount); });
        return FReply::Handled();
    }), INVTEXT("Give this ship 30 test turrets in total."));
    Borrow->SetEnabled(TAttribute<bool>::CreateLambda([this]() { auto* F = EditingShip(); return F && F->Rack && F->Rack->Stock.Num() < FAquilaTurret::MountCount; }));

    auto Details = SNew(SVerticalBox);
    Details->AddSlot().AutoHeight()[Label(TAttribute<FText>::CreateLambda([this]() {
        return FText::FromString(FString::Printf(TEXT("Port %02d"),SelectedPart+1));
    }), 19, true)];
    Details->AddSlot().AutoHeight()[Label(TAttribute<FText>::CreateLambda([this, Item]() {
        const auto* F = EditingShip();
        if (!F || !F->Rack) return FText::GetEmpty();
        if (bMoveHere) return INVTEXT("Click the turret to move.");
        if (const auto* T = Item()) return FText::FromString(FString::Printf(TEXT("%s · %s"), WeaponName(T->Weapon), T->RoundsName()));
        return F->Rack->At(INDEX_NONE) == INDEX_NONE ? INVTEXT("Empty mount · No spare turrets") : INVTEXT("Empty port");
    }), 15)];

    auto Chip = Action(TAttribute<FText>::CreateLambda([Item]() {
        const auto* T = Item(); return T && T->bIntercept ? INVTEXT("Remove interception chip") : INVTEXT("Fit interception chip");
    }), FOnClicked::CreateLambda([this,Item]() {
        if (Item())
        {
            EditFittedTurret([](FAquilaTurret& Gun) { Gun.bIntercept = !Gun.bIntercept; });
            bInspectPort = true;
        }
        return FReply::Handled();
    }),
        INVTEXT("Prioritizes incoming rounds and bolts."));
    Chip->SetVisibility(TAttribute<EVisibility>::CreateLambda([Item]() { return Item() ? EVisibility::Visible : EVisibility::Collapsed; }));
    auto Tools = SNew(SHorizontalBox);
    Tools->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(4,0)[Chip];
    auto Transfer = Action(TAttribute<FText>::CreateLambda([this, Item]() {
        if (Item()) return INVTEXT("Return turret to stock");
        return bMoveHere ? INVTEXT("Cancel move") : INVTEXT("Move a turret here");
    }), FOnClicked::CreateLambda([this, Item]() {
        ClearFiringSelection();
        if (Item()) EditFittedTurret([](FAquilaTurret& Gun) { Gun.Mount = INDEX_NONE; });
        else bMoveHere = !bMoveHere;
        return FReply::Handled();
    }));
    Transfer->SetVisibility(TAttribute<EVisibility>::CreateLambda([this]() { return SelectedPart >= 0 ? EVisibility::Visible : EVisibility::Collapsed; }));
    Transfer->SetEnabled(TAttribute<bool>::CreateLambda([this]() { auto* F = EditingShip(); return F && F->Rack && F->Rack->Installed() > 0; }));
    Tools->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(3,0)[Transfer];
    MountCanvas->AddSlot().HAlign(HAlign_Center).VAlign(VAlign_Top)
        .Position_Lambda([this]() {
            const auto HullCenter = (FittingPin(ArmorPart) + FittingPin(ShieldPart)) * .5;
            return HullCenter + FVector2D(0, MountCanvas->GetCachedGeometry().GetLocalSize().Y * .205);
        })
        .Size_Lambda([this]() { return FVector2D(.49,.145) * FVector2D(MountCanvas->GetCachedGeometry().GetLocalSize()); })
        [SNew(SScaleBox).Stretch(EStretch::ScaleToFit).StretchDirection(EStretchDirection::DownOnly).VAlign(VAlign_Top)
            .Visibility_Lambda([this]() { return SelectedPart >= 0 ? EVisibility::SelfHitTestInvisible : EVisibility::Collapsed; })
            [SNew(SVerticalBox)
                + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)[Details]
                + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0,3)[Tools]]];

    const auto AmmoVisible = TAttribute<EVisibility>::CreateLambda([this]() {
        return AmmoFittingWeapon() ? EVisibility::Visible : EVisibility::Collapsed;
    });
    for (const auto& Style : AquilaAmmunition::Models)
    {
        const auto Kind = Style.Type;
        const auto Chosen = TAttribute<bool>::CreateLambda([this, Kind]() {
            const auto Weapon = AmmoFittingWeapon();
            return Weapon && SampleGun(Weapon.GetValue()).Ammo == Kind;
        });
        Choice(AmmoPreviewUV(Kind), FText::FromString(Style.Name), AmmoVisible, Chosen,
            TAttribute<FText>::CreateLambda([Chosen]() { return Chosen.Get() ? INVTEXT("Selected") : FText::GetEmpty(); }),
            FOnClicked::CreateLambda([this,Kind]() {
                auto* Gun = FittedTurret();
                if (PlacementWeapon && (!Gun || Gun->Weapon != PlacementWeapon.GetValue())) PlacementAmmo = Kind;
                else if (Gun)
                {
                    EditFittedTurret([Kind](FAquilaTurret& Item) { Item.Ammo = Kind; });
                    PlacementAmmo.Reset();
                    bInspectPort = true;
                }
                return FReply::Handled();
            }), FText::FromString(Style.Description), {}, {}, FVector2D(.115,.18), .025);
    }
    AddFiringReadout();
    auto Turrets = Action(INVTEXT("Turrets"), FOnClicked::CreateLambda([this]() { SelectPart(bFitEnemy ? 14 : 6); return FReply::Handled(); }));
    Turrets->SetVisibility(TAttribute<EVisibility>::CreateLambda([this]() { return SelectedPart < 0 ? EVisibility::Visible : EVisibility::Collapsed; }));
    auto Done = Action(INVTEXT("Done placing"), FOnClicked::CreateLambda([this]() { ClearFiringSelection(); return FReply::Handled(); }));
    Done->SetVisibility(TAttribute<EVisibility>::CreateLambda([this]() { return PlacementWeapon ? EVisibility::Visible : EVisibility::Collapsed; }));
    return SNew(SOverlay).Visibility_Lambda([this]() { return EditingShip() ? EVisibility::SelfHitTestInvisible : EVisibility::Collapsed; })
        + SOverlay::Slot()[MountCanvas.ToSharedRef()]
        + SOverlay::Slot().HAlign(HAlign_Left).VAlign(VAlign_Top).Padding(24,14)[SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight()[Label(INVTEXT("Ship fitting"),29,true)]
            + SVerticalBox::Slot().AutoHeight().Padding(0,5)[SNew(SHorizontalBox)
                + SHorizontalBox::Slot().AutoWidth().Padding(0,0,4,0)[ShipTab(false)]
                + SHorizontalBox::Slot().AutoWidth()[ShipTab(true)]
                + SHorizontalBox::Slot().AutoWidth().Padding(10,0,0,0)[Clear]]]
        + SOverlay::Slot().HAlign(HAlign_Center).VAlign(VAlign_Top).Padding(0,14)[SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)[SNew(SHorizontalBox)
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0,0,8,0)[Label(TAttribute<FText>::CreateLambda([this]() {
                if (SelectedPart == ArmorPart)
                {
                    const auto* Fit = EditingShip();
                    return Fit && Fit->Armor == EAquilaArmor::BareHull ? INVTEXT("Bare hull · Choose hull plating") : INVTEXT("Choose hull plating");
                }
                if (SelectedPart == ShieldPart) return INVTEXT("Fit a shield or its enhancement");
                if (bMoveHere) return INVTEXT("Click the fitted turret you want to move.");
                if (PlacementWeapon) return FText::FromString(FString::Printf(TEXT("%s selected · Click a port"), WeaponName(PlacementWeapon.GetValue())));
                return INVTEXT("Choose a turret, then click a port.");
            }), 17)]
            + SHorizontalBox::Slot().AutoWidth()[Done]]
            + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0,4)[SNew(SHorizontalBox)
                + SHorizontalBox::Slot().AutoWidth().Padding(0,0,4,0)[Turrets]
                + SHorizontalBox::Slot().AutoWidth().Padding(0,0,4,0)[Action(INVTEXT("Start duel"), FOnClicked::CreateLambda([this]() {
                    Game()->StartDuel(Game()->Campaign.Loadout); ResetSessionView(); return FReply::Handled();
                }))]
                + SHorizontalBox::Slot().AutoWidth().Padding(0,0,4,0)[Action(INVTEXT("Start duel aboard"), FOnClicked::CreateLambda([this]() {
                    StartDuelAboard(); return FReply::Handled();
                }), INVTEXT("Start at Delta Tree’s windows. N switches to Glacier Tongue. Tab shows the arena."))]
                + SHorizontalBox::Slot().AutoWidth()[Action(INVTEXT("Cancel"), FOnClicked::CreateLambda([this]() {
                    Game()->bFittingOpen = false; ClearFiringSelection(); bMoveHere = false; return FReply::Handled();
                }), INVTEXT("Leave fitting and keep this duel and its equipment."))]]]
        + SOverlay::Slot().HAlign(HAlign_Right).VAlign(VAlign_Top).Padding(24,14)[SNew(SVerticalBox)
            .Visibility_Lambda([this]() { return SelectedPart >= 0 ? EVisibility::Visible : EVisibility::Collapsed; })
            + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Right)[Label(TAttribute<FText>::CreateLambda([this]() {
                const auto* F = EditingShip();
                if (!F || !F->Rack) return FText::GetEmpty();
                const int32 N = F->Rack->Installed();
                return FText::FromString(FString::Printf(TEXT("%d installed  ·  %d in stock"),N,F->Rack->Stock.Num()-N));
            }), 16)]
            + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Right).Padding(0,4)[Borrow]];
}
