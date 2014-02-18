/******************************************************************************
 * Copyright (c) 2013 - 2014, AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

#include <alljoyn/services_common/Services_Common.h>
#include <alljoyn/services_common/PropertyStore.h>

/**
 * Turn on per-module debug printing by setting this variable to non-zero value
 * (usually in debugger).
 */
#ifndef NDEBUG
#ifdef CONFIG_SERVICE
AJ_EXPORT uint8_t dbgAJCFG = 0;
#endif
#ifdef ONBOARDING_SERVICE
AJ_EXPORT uint8_t dbgAJOBS = 0;
#endif
#if defined(NOTIFICATION_SERVICE_CONSUMER) || defined(NOTIFICATION_SERVICE_PRODUCER)
AJ_EXPORT uint8_t dbgAJNS = 0;
#endif
#ifdef CONTROLPANEL_SERVICE
AJ_EXPORT uint8_t dbgAJCPS = 0;
#endif
AJ_EXPORT uint8_t dbgAJSVC = 0;
#endif

uint8_t AJSVC_IsLanguageSupported(AJ_Message* msg, AJ_Message* reply, const char* language, int8_t* langIndex)
{
    uint8_t supported = TRUE;
    int8_t foundLangIndex = AJSVC_PropertyStore_GetLanguageIndex(language);
    if (foundLangIndex == AJSVC_PROPERTY_STORE_ERROR_LANGUAGE_INDEX) {
        AJ_MarshalErrorMsg(msg, reply, AJSVC_ERROR_LANGUAGE_NOT_SUPPORTED);
        supported = FALSE;
    }
    if (langIndex != NULL) {
        *langIndex = foundLangIndex;
    }
    return supported;
}

AJ_Status AJSVC_MarshalAppId(AJ_Message* msg, const char* appId)
{
    AJ_Arg appIdArg;
    uint8_t binAppId[UUID_LENGTH];
    uint32_t sz = strlen(appId);
    if (sz > UUID_LENGTH * 2) { // Crop application id that is too long
        sz = UUID_LENGTH * 2;
    }
    AJ_HexToRaw(appId, sz, binAppId, UUID_LENGTH);

    AJ_InitArg(&appIdArg, AJ_ARG_BYTE, AJ_ARRAY_FLAG, binAppId, UUID_LENGTH);

    return AJ_MarshalArg(msg, &appIdArg);
}

AJ_Status AJSVC_UnmarshalAppId(AJ_Message* msg, char* buf, size_t bufLen)
{
    AJ_Arg appIdArray;
    AJ_Status status;
    size_t appIdLen;

    if (bufLen < (UUID_LENGTH * 2 + 1)) {
        AJ_ErrPrintf(("UnmarshalAppId: Insufficient buffer size! Should be at least %u but got %u\n", UUID_LENGTH * 2 + 1, (uint32_t)bufLen));
        return AJ_ERR_RESOURCES;
    }

    status = AJ_UnmarshalArg(msg, &appIdArray);
    if (status != AJ_OK) {
        return status;
    }

    appIdLen = ((appIdArray.len > UUID_LENGTH) ? UUID_LENGTH : appIdArray.len) * 2 + 1;

    return AJ_RawToHex(appIdArray.val.v_byte, appIdArray.len, buf, appIdLen, FALSE);
}
