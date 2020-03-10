#define NX_SERVICE_ASSUME_NON_DOMAIN
#include "service_guard.h"
#include "runtime/hosversion.h"
#include "services/applet.h"
#include "services/capsu.h"

static Service g_capsuSrv;
static Service g_capsuAccessor;

static Result _capsuSetShimLibraryVersion(u64 version);

NX_GENERATE_SERVICE_GUARD(capsu);

Result _capsuInitialize(void) {
    Result rc=0;

    if (hosversionBefore(5,0,0))
        rc = MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    if (R_SUCCEEDED(rc)) rc = smGetService(&g_capsuSrv, "caps:u");

    if (R_SUCCEEDED(rc) && hosversionAtLeast(7,0,0)) rc = _capsuSetShimLibraryVersion(capsGetShimLibraryVersion());

    return rc;
}

void _capsuCleanup(void) {
    serviceClose(&g_capsuAccessor);
    serviceClose(&g_capsuSrv);
}

Service* capsuGetServiceSession(void) {
    return &g_capsuSrv;
}

Service* capsuGetServiceSession_Accessor(void) {
    return &g_capsuAccessor;
}

static Result _capsuCmdInU64NoOut(Service* srv, u64 inval, u32 cmd_id) {
    return serviceDispatchIn(srv, cmd_id, inval);
}

static Result _capsuSetShimLibraryVersion(u64 version) {
    if (hosversionBefore(7,0,0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        u64 version;
        u64 AppletResourceUserId;
    } in = { version, appletGetAppletResourceUserId() };

    return serviceDispatchIn(&g_capsuSrv, 32, in,
        .in_send_pid = true,
    );
}

Result capsuGetAlbumFileList0AafeAruid(CapsApplicationAlbumFileEntry* entries, s32 count, CapsContentType type, u64 start_timestamp, u64 end_timestamp, s32 *total_entries) {
    const struct {
        u8 type;
        u8 pad[7];
        u64 start_timestamp;
        u64 end_timestamp;
        u64 AppletResourceUserId;
    } in = { type, {0}, start_timestamp, end_timestamp, appletGetAppletResourceUserId() };

    u64 total_out=0;
    Result rc = serviceDispatchInOut(&g_capsuSrv, 102, in, total_out,
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
        .buffers = { { entries, count*sizeof(CapsApplicationAlbumFileEntry) } },
        .in_send_pid = true,
    );
    if (R_SUCCEEDED(rc) && total_entries) *total_entries = total_out;
    return rc;
}

Result capsuDeleteAlbumFileByAruid(CapsContentType type, const CapsApplicationAlbumFileEntry *entry) {
    const struct {
        u8 type;
        u8 pad[7];
        CapsApplicationAlbumFileEntry entry;
        u64 AppletResourceUserId;
    } in = { type, {0}, *entry, appletGetAppletResourceUserId() };

    return serviceDispatchIn(&g_capsuSrv, 103, in,
        .in_send_pid = true,
    );
}

Result capsuGetAlbumFileSizeByAruid(const CapsApplicationAlbumFileEntry *entry, u64 *size) {
    const struct {
        CapsApplicationAlbumFileEntry entry;
        u64 AppletResourceUserId;
    } in = { *entry, appletGetAppletResourceUserId() };

    return serviceDispatchInOut(&g_capsuSrv, 104, in, *size,
        .in_send_pid = true,
    );
}

Result capsuDeleteAlbumFileByAruidForDebug(const CapsApplicationAlbumFileEntry *entry) {
    const struct {
        CapsApplicationAlbumFileEntry entry;
        u64 AppletResourceUserId;
    } in = { *entry, appletGetAppletResourceUserId() };

    return serviceDispatchIn(&g_capsuSrv, 105, in,
        .in_send_pid = true,
    );
}

static Result _capsuLoadAlbumScreenShot(u32 cmd_id, CapsLoadAlbumScreenShotImageOutputForApplication *out, void* image, size_t image_size, void* workbuf, size_t workbuf_size, const CapsApplicationAlbumFileEntry *entry, const CapsScreenShotDecodeOption *option) {
    const struct {
        CapsApplicationAlbumFileEntry entry;
        CapsScreenShotDecodeOption option;
        u64 AppletResourceUserId;
    } in = { *entry, *option, appletGetAppletResourceUserId() };

    return serviceDispatchIn(&g_capsuSrv, cmd_id, in,
        .buffer_attrs = {
            SfBufferAttr_HipcMapAlias | SfBufferAttr_Out,
            SfBufferAttr_HipcMapTransferAllowsNonSecure | SfBufferAttr_HipcMapAlias | SfBufferAttr_Out,
            SfBufferAttr_HipcMapAlias | SfBufferAttr_Out,
        },
        .buffers = {
            { out, sizeof(*out) },
            { image, image_size },
            { workbuf, workbuf_size },
        },
        .in_send_pid = true,
    );
}

Result capsuLoadAlbumScreenShotImage(CapsLoadAlbumScreenShotImageOutputForApplication *out, void* image, size_t image_size, void* workbuf, size_t workbuf_size, const CapsApplicationAlbumFileEntry *entry, const CapsScreenShotDecodeOption *option) {
    return _capsuLoadAlbumScreenShot(110, out, image, image_size, workbuf, workbuf_size, entry, option);
}

Result capsuLoadAlbumScreenShotThumbnailImage(CapsLoadAlbumScreenShotImageOutputForApplication *out, void* image, size_t image_size, void* workbuf, size_t workbuf_size, const CapsApplicationAlbumFileEntry *entry, const CapsScreenShotDecodeOption *option) {
    return _capsuLoadAlbumScreenShot(120, out, image, image_size, workbuf, workbuf_size, entry, option);
}

Result capsuPrecheckToCreateContentsByAruid(CapsContentType type, u64 unk) {
    const struct {
        u8 type;
        u8 pad[7];
        u64 unk;
        u64 AppletResourceUserId;
    } in = { type, {0}, unk, appletGetAppletResourceUserId() };

    return serviceDispatchIn(&g_capsuSrv, 130, in,
        .in_send_pid = true,
    );
}

static Result _capsuGetAlbumFileList(u32 cmd_id, void* entries, size_t entrysize, s32 count, CapsContentType type, CapsAlbumFileDateTime start_datetime, CapsAlbumFileDateTime end_datetime, s32 *total_entries) {
    const struct {
        u8 type;
        u8 pad;
        CapsAlbumFileDateTime start_datetime;
        CapsAlbumFileDateTime end_datetime;
        u8 pad2[6];
        u64 AppletResourceUserId;
    } in = { type, 0, start_datetime, end_datetime, {0}, appletGetAppletResourceUserId() };

    u64 total_out=0;
    Result rc = serviceDispatchInOut(&g_capsuSrv, cmd_id, in, total_out,
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
        .buffers = { { entries, count*entrysize } },
        .in_send_pid = true,
    );
    if (R_SUCCEEDED(rc) && total_entries) *total_entries = total_out;
    return rc;
}

static Result _capsuGetAlbumFileListUid(u32 cmd_id, void* entries, size_t entrysize, s32 count, CapsContentType type, CapsAlbumFileDateTime start_datetime, CapsAlbumFileDateTime end_datetime, AccountUid uid, s32 *total_entries) {
    const struct {
        u8 type;
        u8 pad;
        CapsAlbumFileDateTime start_datetime;
        CapsAlbumFileDateTime end_datetime;
        u8 pad2[6];
        AccountUid uid;
        u64 AppletResourceUserId;
    } in = { type, 0, start_datetime, end_datetime, {0}, uid, appletGetAppletResourceUserId() };

    u64 total_out=0;
    Result rc = serviceDispatchInOut(&g_capsuSrv, cmd_id, in, total_out,
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
        .buffers = { { entries, count*entrysize } },
        .in_send_pid = true,
    );
    if (R_SUCCEEDED(rc) && total_entries) *total_entries = total_out;
    return rc;
}

Result capsuGetAlbumFileList1AafeAruid(CapsApplicationAlbumFileEntry *entries, s32 count, CapsContentType type, CapsAlbumFileDateTime start_datetime, CapsAlbumFileDateTime end_datetime, s32 *total_entries) {
    return _capsuGetAlbumFileList(140, entries, sizeof(CapsApplicationAlbumFileEntry), count, type, start_datetime, end_datetime, total_entries);
}

Result capsuGetAlbumFileList2AafeUidAruid(CapsApplicationAlbumFileEntry *entries, s32 count, CapsContentType type, CapsAlbumFileDateTime start_datetime, CapsAlbumFileDateTime end_datetime, AccountUid uid, s32 *total_entries) {
    return _capsuGetAlbumFileListUid(141, entries, sizeof(CapsApplicationAlbumFileEntry), count, type, start_datetime, end_datetime, uid, total_entries);
}

Result capsuGetAlbumFileList3AaeAruid(CapsApplicationAlbumEntry *entries, s32 count, CapsContentType type, CapsAlbumFileDateTime start_datetime, CapsAlbumFileDateTime end_datetime, s32 *total_entries) {
    return _capsuGetAlbumFileList(142, entries, sizeof(CapsApplicationAlbumEntry), count, type, start_datetime, end_datetime, total_entries);
}

Result capsuGetAlbumFileList4AaeUidAruid(CapsApplicationAlbumEntry *entries, s32 count, CapsContentType type, CapsAlbumFileDateTime start_datetime, CapsAlbumFileDateTime end_datetime, AccountUid uid, s32 *total_entries) {
    return _capsuGetAlbumFileListUid(143, entries, sizeof(CapsApplicationAlbumEntry), count, type, start_datetime, end_datetime, uid, total_entries);
}

static Result _capsuOpenAccessorSessionForApplication(Service* srv_out, const CapsApplicationAlbumFileEntry *entry) {
    const struct {
        CapsApplicationAlbumFileEntry entry;
        u64 AppletResourceUserId;
    } in = { *entry, appletGetAppletResourceUserId() };

    return serviceDispatchIn(&g_capsuSrv, 60002, in,
        .in_send_pid = true,
        .out_num_objects = 1,
        .out_objects = srv_out,
    );
}

static Result _capsuOpenAlbumMovieReadStream(u64 *stream, const CapsApplicationAlbumFileEntry *entry) {
    const struct {
        CapsApplicationAlbumFileEntry entry;
        u64 AppletResourceUserId;
    } in = { *entry, appletGetAppletResourceUserId() };

    return serviceDispatchInOut(&g_capsuAccessor, 2001, in, *stream,
        .in_send_pid = true,
    );
}

static Result _capsuGetAlbumMovieReadStreamMovieDataSize(u64 stream, u64 *size) {
    return serviceDispatchInOut(&g_capsuAccessor, 2003, stream, *size);
}

static Result _capsuReadMovieDataFromAlbumMovieReadStream(u64 stream, s64 offset, void* buffer, size_t size, u64 *actual_size) {
    const struct {
        u64 stream;
        s64 offset;
    } in = { stream, offset };

    return serviceDispatchInOut(&g_capsuAccessor, 2004, in, *actual_size,
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
        .buffers = { { buffer, size } },
    );
}

Result capsuOpenAlbumMovieReadStream(u64 *stream, const CapsApplicationAlbumFileEntry *entry) {
    Result rc=0;

    if (!serviceIsActive(&g_capsuAccessor)) rc = _capsuOpenAccessorSessionForApplication(&g_capsuAccessor, entry);

    if (R_SUCCEEDED(rc)) rc = _capsuOpenAlbumMovieReadStream(stream, entry);

    return rc;
}

Result capsuCloseAlbumMovieReadStream(u64 stream) {
    if (!serviceIsActive(&g_capsuAccessor))
        return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);

    return _capsuCmdInU64NoOut(&g_capsuAccessor, stream, 2002);
}

Result capsuGetAlbumMovieReadStreamMovieDataSize(u64 stream, u64 *size) {
    if (!serviceIsActive(&g_capsuAccessor))
        return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);

    return _capsuGetAlbumMovieReadStreamMovieDataSize(stream, size);
}

Result capsuReadMovieDataFromAlbumMovieReadStream(u64 stream, s64 offset, void* buffer, size_t size, u64 *actual_size) {
    if (!serviceIsActive(&g_capsuAccessor))
        return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);

    return _capsuReadMovieDataFromAlbumMovieReadStream(stream, offset, buffer, size, actual_size);
}

Result capsuGetAlbumMovieReadStreamBrokenReason(u64 stream) {
    if (!serviceIsActive(&g_capsuAccessor))
        return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);

    return _capsuCmdInU64NoOut(&g_capsuAccessor, stream, 2005);
}

