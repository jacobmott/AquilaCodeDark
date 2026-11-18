#pragma once

#include "AquilaSimulation.h"

class FAquilaSave
{
public:
    static FString Encode(const FAquilaCampaign& Campaign);
    // Decode into a temporary: invalid/newer saves never partially mutate a live campaign.
    static bool Decode(const FString& Json, FAquilaCampaign& Out, FString& Error);
    static bool LoadJournal(const FString& Directory, FAquilaCampaign& Out, FString& Status, bool& bWritable);
    static bool Commit(const FString& Directory, FAquilaCampaign& Campaign, FString& Status);
};
