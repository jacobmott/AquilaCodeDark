#pragma once
#include "Commandlets/Commandlet.h"
#include "AquilaContentCommandlet.generated.h"

UCLASS()
class UAquilaContentCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    virtual int32 Main(const FString& Params) override;
};
