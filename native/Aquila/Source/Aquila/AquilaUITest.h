#pragma once
#if WITH_DEV_AUTOMATION_TESTS && PLATFORM_WINDOWS
#include "CoreMinimal.h"

class FAutomationTestBase;
class SWindow;
namespace AquilaUITest
{
    bool ClickText(FAutomationTestBase& Test, const TSharedRef<SWindow>& Window, const TCHAR* Text, FName Scope=NAME_None);
}
#endif
