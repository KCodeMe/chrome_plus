#include "appid.h"

#include <windows.h>

#include <propkey.h>
#include <shobjidl.h>

#include "detours.h"

#include "utils.h"

namespace {

static auto RawPSStringFromPropertyKey = PSStringFromPropertyKey;

HRESULT WINAPI MyPSStringFromPropertyKey(REFPROPERTYKEY pkey,
                                         LPWSTR psz,
                                         UINT cch) {
  HRESULT result = RawPSStringFromPropertyKey(pkey, psz, cch);
  if (SUCCEEDED(result)) {
    if (pkey == PKEY_AppUserModel_ID) {
      // DebugLog(L"MyPSStringFromPropertyKey {}", psz);
      return -1;
    }
  }
  return result;
}

}  // namespace

void SetAppId() {
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  DetourAttach(reinterpret_cast<LPVOID*>(&RawPSStringFromPropertyKey),
               reinterpret_cast<void*>(MyPSStringFromPropertyKey));
  auto status = DetourTransactionCommit();
  if (status != NO_ERROR) {
    DebugLog(L"SetAppId failed {}", status);
  }
}
