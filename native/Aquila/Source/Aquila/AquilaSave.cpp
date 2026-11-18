#include "AquilaSave.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

namespace
{
using FJson = TSharedPtr<FJsonObject>;
TArray<TSharedPtr<FJsonValue>> VectorJson(const FVector& V)
{
    return {MakeShared<FJsonValueNumber>(V.X), MakeShared<FJsonValueNumber>(V.Y), MakeShared<FJsonValueNumber>(V.Z)};
}
bool Number(const FJson& J, const TCHAR* Key, double& Value, double Min, double Max)
{
    return J->TryGetNumberField(Key, Value) && FMath::IsFinite(Value) && Value >= Min && Value <= Max;
}
bool Integer(const FJson& J, const TCHAR* Key, int32& Value, int32 Min, int32 Max)
{
    double D;
    if (!Number(J, Key, D, Min, Max) || D != FMath::FloorToDouble(D)) return false;
    Value = static_cast<int32>(D);
    return true;
}
bool Vector(const FJson& J, const TCHAR* Key, FVector& Out, double Limit = 1e12)
{
    const TArray<TSharedPtr<FJsonValue>>* A;
    if (!J->TryGetArrayField(Key, A) || A->Num() != 3) return false;
    double Values[3];
    for (int32 I = 0; I < 3; ++I)
        if (!(*A)[I]->TryGetNumber(Values[I]) || !FMath::IsFinite(Values[I]) || FMath::Abs(Values[I]) > Limit) return false;
    Out = FVector(Values[0], Values[1], Values[2]);
    return true;
}
bool Guid(const FJson& J, const TCHAR* Key, FString& Value, bool bEmptyAllowed = false)
{
    FGuid Parsed;
    return J->TryGetStringField(Key, Value) && ((bEmptyAllowed && Value.IsEmpty()) || FGuid::Parse(Value, Parsed));
}
}

FString FAquilaSave::Encode(const FAquilaCampaign& C)
{
    const FJson J = MakeShared<FJsonObject>();
    J->SetNumberField(TEXT("schemaVersion"), FAquilaCampaign::SchemaVersion);
    J->SetNumberField(TEXT("generationVersion"), FAquilaCampaign::GenerationVersion);
    J->SetStringField(TEXT("campaignId"), C.CampaignId);
    J->SetStringField(TEXT("revisionId"), C.RevisionId);
    J->SetStringField(TEXT("parentRevisionId"), C.ParentRevisionId);
    J->SetNumberField(TEXT("sequence"), C.Sequence);
    J->SetNumberField(TEXT("worldSeed"), C.WorldSeed);
    J->SetNumberField(TEXT("victories"), C.Victories);
    J->SetNumberField(TEXT("expedition"), static_cast<int32>(C.Expedition));
    J->SetNumberField(TEXT("expeditionNumber"), C.ExpeditionNumber);
    J->SetNumberField(TEXT("completedExpeditions"), C.CompletedExpeditions);
    J->SetNumberField(TEXT("salvage"), C.Salvage);
    J->SetNumberField(TEXT("portTurretLevel"), C.PortTurretLevel);
    J->SetArrayField(TEXT("signalPosition"), VectorJson(C.SignalPosition));
    J->SetNumberField(TEXT("expeditionSeconds"), C.ExpeditionSeconds);
    J->SetNumberField(TEXT("surveySeconds"), C.SurveySeconds);
    J->SetNumberField(TEXT("recoverySeconds"), C.RecoverySeconds);
    J->SetArrayField(TEXT("shipPosition"), VectorJson(C.ShipPosition));
    J->SetArrayField(TEXT("enemyPosition"), VectorJson(C.EnemyPosition));
    J->SetNumberField(TEXT("heading"), C.Heading);
    J->SetNumberField(TEXT("shield"), C.Shield);
    J->SetNumberField(TEXT("hull"), C.Hull);
    J->SetNumberField(TEXT("enemyShield"), C.EnemyShield);
    J->SetNumberField(TEXT("enemyHull"), C.EnemyHull);
    J->SetNumberField(TEXT("battleTime"), C.BattleTime);
    J->SetNumberField(TEXT("stepRemainder"), C.StepRemainder);
    J->SetNumberField(TEXT("enemyClock"), C.EnemyClock);
    for (int32 I = 0; I < 2; ++I)
    {
        J->SetNumberField(FString::Printf(TEXT("turretClock%d"), I), C.TurretClock[I]);
        J->SetNumberField(FString::Printf(TEXT("burstCount%d"), I), C.BurstCount[I]);
    }
    J->SetNumberField(TEXT("order"), static_cast<int32>(C.Order));
    J->SetBoolField(TEXT("battle"), C.bBattle);
    J->SetBoolField(TEXT("holdFire"), C.bHoldFire);
    J->SetBoolField(TEXT("piercing"), C.bPiercing);
    J->SetBoolField(TEXT("armorBlocked"), C.bArmorBlocked);
    TArray<TSharedPtr<FJsonValue>> Rounds;
    for (const auto& R : C.Rounds)
    {
        const FJson Shot = MakeShared<FJsonObject>();
        Shot->SetArrayField(TEXT("position"), VectorJson(R.Position));
        Shot->SetArrayField(TEXT("velocity"), VectorJson(R.Velocity));
        Shot->SetNumberField(TEXT("age"), R.Age);
        Shot->SetBoolField(TEXT("enemy"), R.bEnemy);
        Shot->SetBoolField(TEXT("piercing"), R.bPiercing);
        Shot->SetNumberField(TEXT("damageScale"), R.DamageScale);
        Rounds.Add(MakeShared<FJsonValueObject>(Shot));
    }
    J->SetArrayField(TEXT("rounds"), Rounds);
    FString Text;
    FJsonSerializer::Serialize(J.ToSharedRef(), TJsonWriterFactory<>::Create(&Text));
    return Text;
}

bool FAquilaSave::Decode(const FString& Text, FAquilaCampaign& Out, FString& Error)
{
    FJson J;
    Error = TEXT("Invalid save. Original file preserved.");
    if (Text.Len() > 1024 * 1024 || !FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), J) || !J.IsValid()) return false;
    int32 Schema, Generation;
    if (!Integer(J, TEXT("schemaVersion"), Schema, 1, MAX_int32) || !Integer(J, TEXT("generationVersion"), Generation, 1, MAX_int32)) return false;
    if (Schema > FAquilaCampaign::SchemaVersion || Generation != FAquilaCampaign::GenerationVersion)
    {
        Error = TEXT("Newer or unsupported save version. Saving is disabled; original file preserved.");
        return false;
    }
    FAquilaCampaign C;
    int32 Order;
    if (!Guid(J, TEXT("campaignId"), C.CampaignId) || !Guid(J, TEXT("revisionId"), C.RevisionId)
        || !Guid(J, TEXT("parentRevisionId"), C.ParentRevisionId, true)
        || !Integer(J, TEXT("sequence"), C.Sequence, 1, MAX_int32 - 1)
        || !Integer(J, TEXT("worldSeed"), C.WorldSeed, 0, MAX_int32)
        || !Integer(J, TEXT("victories"), C.Victories, 0, MAX_int32 - 1)
        || !Vector(J, TEXT("shipPosition"), C.ShipPosition) || !Vector(J, TEXT("enemyPosition"), C.EnemyPosition)
        || !Number(J, TEXT("heading"), C.Heading, -1e12, 1e12)
        || !Number(J, TEXT("shield"), C.Shield, 0, 100) || !Number(J, TEXT("hull"), C.Hull, 0, 100)
        || !Number(J, TEXT("enemyShield"), C.EnemyShield, 0, 100) || !Number(J, TEXT("enemyHull"), C.EnemyHull, 0, 100)
        || !Number(J, TEXT("battleTime"), C.BattleTime, 0, 1e12)
        || !Integer(J, TEXT("order"), Order, 0, Schema >= 3 ? 5 : 3)
        || !J->TryGetBoolField(TEXT("battle"), C.bBattle) || !J->TryGetBoolField(TEXT("holdFire"), C.bHoldFire)
        || !J->TryGetBoolField(TEXT("piercing"), C.bPiercing) || !J->TryGetBoolField(TEXT("armorBlocked"), C.bArmorBlocked)) return false;
    C.Order = static_cast<EAquilaOrder>(Order);
    // Versions 1/2 retain the voyage and combat state, with an unstarted expedition and no invented rewards.
    if (Schema >= 3)
    {
        int32 Stage;
        if (!Integer(J,TEXT("expedition"),Stage,0,6)
            || !Integer(J,TEXT("expeditionNumber"),C.ExpeditionNumber,0,MAX_int32-1)
            || !Integer(J,TEXT("completedExpeditions"),C.CompletedExpeditions,0,C.ExpeditionNumber)
            || !Integer(J,TEXT("salvage"),C.Salvage,0,MAX_int32-1)
            || !Integer(J,TEXT("portTurretLevel"),C.PortTurretLevel,0,1)
            || !Vector(J,TEXT("signalPosition"),C.SignalPosition)
            || !Number(J,TEXT("expeditionSeconds"),C.ExpeditionSeconds,0,1e12)
            || !Number(J,TEXT("surveySeconds"),C.SurveySeconds,0,FAquilaCampaign::SurveyDuration)
            || !Number(J,TEXT("recoverySeconds"),C.RecoverySeconds,0,FAquilaCampaign::RecoveryDuration)) return false;
        C.Expedition = static_cast<EAquilaExpedition>(Stage);
        if ((C.Expedition == EAquilaExpedition::Available && C.ExpeditionNumber != 0)
            || (C.Expedition != EAquilaExpedition::Available && C.ExpeditionNumber == 0)
            || (C.ExpeditionActive() && C.CompletedExpeditions >= C.ExpeditionNumber)
            || (C.Expedition == EAquilaExpedition::Complete && C.CompletedExpeditions != C.ExpeditionNumber)) return false;
    }
    // Version 1 had no firing clocks or in-flight rounds. Migrate those to safe, delayed initial values.
    if (Schema >= 2)
    {
        if (!Number(J, TEXT("stepRemainder"), C.StepRemainder, 0, 1.0 / 120.0)
            || !Number(J, TEXT("enemyClock"), C.EnemyClock, -1e12, 10)) return false;
        for (int32 I = 0; I < 2; ++I)
            if (!Number(J, *FString::Printf(TEXT("turretClock%d"), I), C.TurretClock[I], -1e12, 10)
                || !Integer(J, *FString::Printf(TEXT("burstCount%d"), I), C.BurstCount[I], 0, 3)) return false;
        const TArray<TSharedPtr<FJsonValue>>* Shots;
        if (!J->TryGetArrayField(TEXT("rounds"), Shots) || Shots->Num() > FAquilaCampaign::MaxRounds) return false;
        for (const auto& Value : *Shots)
        {
            const TSharedPtr<FJsonObject>* Shot;
            FAquilaRound R;
            if (!Value->TryGetObject(Shot) || !Vector(*Shot, TEXT("position"), R.Position)
                || !Vector(*Shot, TEXT("velocity"), R.Velocity, 100000)
                || !Number(*Shot, TEXT("age"), R.Age, 0, 20)
                || !(*Shot)->TryGetBoolField(TEXT("enemy"), R.bEnemy)
                || !(*Shot)->TryGetBoolField(TEXT("piercing"), R.bPiercing)) return false;
            if (Schema >= 3 && !Number(*Shot,TEXT("damageScale"),R.DamageScale,0.001,2)) return false;
            C.Rounds.Add(R);
        }
    }
    Out = MoveTemp(C);
    Error.Empty();
    return true;
}

bool FAquilaSave::LoadJournal(const FString& Directory, FAquilaCampaign& Out, FString& Status, bool& bWritable)
{
    TArray<FString> Files;
    IFileManager::Get().FindFiles(Files, *(Directory / TEXT("*.json")), true, false);
    bWritable = true;
    bool bLoaded = false;
    TSet<FString> Revisions, Parents, Campaigns;
    int32 BestSequence = -1;
    for (const FString& File : Files)
    {
        FString Text, Error;
        FAquilaCampaign C;
        if (IFileManager::Get().FileSize(*(Directory / File)) > 1024 * 1024
            || !FFileHelper::LoadFileToString(Text, *(Directory / File)) || !Decode(Text, C, Error))
        {
            bWritable = false;
            continue;
        }
        Campaigns.Add(C.CampaignId);
        Revisions.Add(C.RevisionId);
        if (!C.ParentRevisionId.IsEmpty()) Parents.Add(C.ParentRevisionId);
        if (C.Sequence > BestSequence)
        {
            Out = C;
            BestSequence = C.Sequence;
            bLoaded = true;
        }
    }
    int32 Tips = 0;
    for (const FString& Revision : Revisions) if (!Parents.Contains(Revision)) ++Tips;
    if (Tips > 1 || Campaigns.Num() > 1) bWritable = false;
    Status = !bWritable ? TEXT("SAVE NEEDS REVIEW / ALL REVISIONS PRESERVED / WRITES PAUSED")
        : bLoaded ? TEXT("RESTORED LOCAL SAVE / CLOUD NOT CONNECTED") : TEXT("LOCAL CAMPAIGN / CLOUD NOT CONNECTED");
    return bLoaded;
}

bool FAquilaSave::Commit(const FString& Directory, FAquilaCampaign& Campaign, FString& Status)
{
    if (Campaign.Sequence >= MAX_int32 - 1) { Status = TEXT("SAVE LIMIT REACHED / PREVIOUS REVISIONS PRESERVED"); return false; }
    FAquilaCampaign Next = Campaign;
    Next.ParentRevisionId = Campaign.RevisionId;
    Next.RevisionId = FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens);
    ++Next.Sequence;
    const FString Text = Encode(Next);
    FAquilaCampaign Checked;
    FString Error;
    if (!Decode(Text, Checked, Error)) { Status = Error; return false; }
    IFileManager::Get().MakeDirectory(*Directory, true);
    const FString Final = Directory / (Next.RevisionId + TEXT(".json"));
    const FString Temporary = Final + TEXT(".pending");
    if (!FFileHelper::SaveStringToFile(Text, *Temporary, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM,
            &IFileManager::Get(), FILEWRITE_NoReplaceExisting)
        || !IFileManager::Get().Move(*Final, *Temporary, false, false, false, true))
    {
        Status = TEXT("SAVE FAILED / PREVIOUS REVISIONS PRESERVED");
        return false;
    }
    // Publish the in-memory revision only after the immutable file is fully written and renamed.
    Campaign = MoveTemp(Next);
    Status = TEXT("SAVED LOCALLY / CLOUD NOT CONNECTED");
    return true;
}
