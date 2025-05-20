#pragma once

#include "CoreMinimal.h"

#define LOG_CALL_INFO ANSI_TO_TCHAR(__FUNCTION__)
#define EMBER_LOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("%s %s"), LOG_CALL_INFO, *FString::Printf(Format, ##__VA_ARGS__))

DECLARE_LOG_CATEGORY_EXTERN(LogEmber, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogEmberItem, Log, All);

inline DEFINE_LOG_CATEGORY(LogEmber);
inline DEFINE_LOG_CATEGORY(LogEmberItem);