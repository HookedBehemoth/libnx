/**
 * @file capsu.h
 * @brief Application Album (caps:u) service IPC wrapper.
 * This is only usable with AlbumFiles associated with the current host Application.
 * @author yellows8
 * @copyright libnx Authors
 */
#pragma once
#include "../types.h"
#include "../sf/service.h"
#include "../services/caps.h"
#include "../services/acc.h"

/// Initialize caps:u. Only available on [5.0.0+].
Result capsuInitialize(void);

/// Exit caps:u.
void capsuExit(void);

/// Gets the Service for caps:u.
Service* capsuGetServiceSession(void);

/// Gets the Service for IAlbumAccessorApplicationSession, only initialized after \ref capsuOpenAlbumMovieStream was used (unaffected by using \ref capsuCloseAlbumMovieStream).
Service* capsuGetServiceSession_Accessor(void);

Result capsuGetAlbumFileList0AafeAruid(CapsApplicationAlbumFileEntry* entries, s32 count, CapsContentType type, u64 start_timestamp, u64 end_timestamp, s32 *total_entries);

/**
 * @brief Deletes the specified AlbumFile.
 * @param[in] type \ref CapsContentType, must match ::CapsContentType_ExtraMovie.
 * @param[in] entry \ref CapsApplicationAlbumFileEntry
 */
Result capsuDeleteAlbumFileByAruid(CapsContentType type, const CapsApplicationAlbumFileEntry *entry);

/**
 * @brief Gets the filesize for the entire specified AlbumFile.
 * @param[in] entry \ref CapsApplicationAlbumFileEntry
 * @param[out] size Output filesize.
 */
Result capsuGetAlbumFileSizeByAruid(const CapsApplicationAlbumFileEntry *entry, u64 *size);

/**
 * @brief Deletes the specified AlbumFile.
 * @param[in] entry \ref CapsApplicationAlbumFileEntry
 */
Result capsuDeleteAlbumFileByAruidForDebug(const CapsApplicationAlbumFileEntry *entry);

Result capsuLoadAlbumScreenShotImage(CapsLoadAlbumScreenShotImageOutputForApplication *out, void* image, size_t image_size, void* workbuf, size_t workbuf_size, const CapsApplicationAlbumFileEntry *entry, const CapsScreenShotDecodeOption *option);
Result capsuLoadAlbumScreenShotThumbnailImage(CapsLoadAlbumScreenShotImageOutputForApplication *out, void* image, size_t image_size, void* workbuf, size_t workbuf_size, const CapsApplicationAlbumFileEntry *entry, const CapsScreenShotDecodeOption *option);

/**
 * @brief PrecheckToCreateContents. Official sw only uses this with ::CapsContentType_ExtraMovie.
 * @param[in] type \ref CapsContentType
 * @param[in] unk Unknown.
 */
Result capsuPrecheckToCreateContentsByAruid(CapsContentType type, u64 unk);

Result capsuGetAlbumFileList1AafeAruid(CapsApplicationAlbumFileEntry *entries, s32 count, CapsContentType type, CapsAlbumFileDateTime start_datetime, CapsAlbumFileDateTime end_datetime, s32 *total_entries);
Result capsuGetAlbumFileList2AafeUidAruid(CapsApplicationAlbumFileEntry *entries, s32 count, CapsContentType type, CapsAlbumFileDateTime start_datetime, CapsAlbumFileDateTime end_datetime, AccountUid uid, s32 *total_entries);
Result capsuGetAlbumFileList3AaeAruid(CapsApplicationAlbumEntry *entries, s32 count, CapsContentType type, CapsAlbumFileDateTime start_datetime, CapsAlbumFileDateTime end_datetime, s32 *total_entries);
Result capsuGetAlbumFileList4AaeUidAruid(CapsApplicationAlbumEntry *entries, s32 count, CapsContentType type, CapsAlbumFileDateTime start_datetime, CapsAlbumFileDateTime end_datetime, AccountUid uid, s32 *total_entries);

/**
 * @brief Opens an AlbumMovieStream.
 * @note This opens IAlbumAccessorApplicationSession if not previously opened, it's closed during \ref capsuExit.
 * @note Up to 4 streams can be open at the same time. Multiple streams can be open at the same time for the same \ref CapsApplicationAlbumFileEntry.
 * @param[out] stream Stream handle.
 * @param[in] entry \ref CapsApplicationAlbumFileEntry
 */
Result capsuOpenAlbumMovieReadStream(u64 *stream, const CapsApplicationAlbumFileEntry *entry);

/**
 * @brief Closes an AlbumMovieStream.
 * @param[in] stream Stream handle.
 */
Result capsuCloseAlbumMovieReadStream(u64 stream);

/**
 * @brief Gets the data size of an AlbumMovieStream.
 * @param[in] stream Stream handle.
 * @param[out] size Size of the actual MP4, without the JPEG at the end.
 */
Result capsuGetAlbumMovieReadStreamMovieDataSize(u64 stream, u64 *size);

/**
 * @brief Reads data from an AlbumMovieStream.
 * @note offset(+size) must not be negative. offset and size must be aligned to 0x40000-bytes.
 * @note When offset(+size) goes beyond the size from \ref capsuGetAlbumMovieStreamSize, the regions of the buffer which goes beyond that are cleared to 0, and actual_size is still set to the input size.
 * @param[in] stream Stream handle.
 * @param[in] offset Offset.
 * @param[out] Output data buffer.
 * @param[in] size Data buffer size.
 * @param[out] actual_size Actual read size.
 */
Result capsuReadMovieDataFromAlbumMovieReadStream(u64 stream, s64 offset, void* buffer, size_t size, u64 *actual_size);

/**
 * @brief Gets the BrokenReason for an AlbumMovieStream.
 * @note Official sw doesn't use this.
 * @param[in] stream Stream handle.
 */
Result capsuGetAlbumMovieReadStreamBrokenReason(u64 stream);

