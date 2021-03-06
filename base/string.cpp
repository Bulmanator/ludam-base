// @Note: This is intentionally left out of the header and should not be used, just wrap null-terminated strings
// into a str8 using WrapZ
//
function uptr StringLength(u8 *data) {
    uptr result = 0;
    while (data[result] != 0) {
        result += 1;
    }

    return result;
}

function str8 WrapCount(u8 *data, uptr count) {
    str8 result;
    result.count = count;
    result.data  = data;

    return result;
}

function str8 WrapRange(u8 *start, u8 *end) {
    str8 result;
    result.count = cast(uptr) (end - start);
    result.data  = start;

    return result;
}

function str8 WrapZ(u8 *z) {
    str8 result;
    result.count = StringLength(z);
    result.data  = z;

    return result;
}

function str8 WrapZ(const char *z) {
    str8 result = WrapZ(cast(u8 *) z);
    return result;
}

function b32 IsValid(str8 str) {
    b32 result = (str.count != 0) && (str.data != 0);
    return result;
}

function str8 Advance(str8 base, uptr count) {
    str8 result;

    uptr min = Min(base.count, count);

    result.count = base.count - min;
    result.data  = base.data  + min;

    return result;
}

function str8 Remove(str8 base, uptr count) {
    str8 result;

    uptr min = Min(base.count, count);

    result.count = base.count - min;
    result.data  = base.data;

    return result;
}

function str8 Suffix(str8 base, uptr count) {
    str8 result;
    result.count = Min(base.count, count);
    result.data  = base.data + (base.count - result.count);

    return result;
}

function str8 Prefix(str8 base, uptr count) {
    str8 result;
    result.count = Min(base.count, count);
    result.data  = base.data;

    return result;
}

function str8 Substring(str8 base, uptr start, uptr end) {
    str8 result;

    uptr min_start = Min(base.count, start);
    uptr min_end   = Min(base.count, end);

    if (min_start > min_end) { Swap(min_start, min_end); }

    result.count = min_end - min_start;
    result.data  = base.data + min_start;

    return result;
}

function uptr ProcessFormat(str8 output, const char *format, va_list args) {
    uptr result = vsnprintf(cast(char *) output.data, output.count, format, args);
    return result;
}

function str8 FormatStrArgs(Memory_Arena *arena, const char *format, va_list args) {
    str8 result;

    va_list copy;
    va_copy(copy, args); // Needed in case the initial attempt buffer is too short

    str8 buffer;
    buffer.count = 1024;
    buffer.data  = AllocArray(arena, u8, buffer.count);

    result.count = ProcessFormat(buffer, format, args);
    if (result.count > buffer.count) {
        RemoveSize(arena, buffer.count);

        result.data = AllocArray(arena, u8, result.count);
        ProcessFormat(result, format, copy);
    }
    else {
        uptr to_remove = buffer.count - result.count;
        RemoveSize(arena, to_remove);

        result.data = buffer.data;
    }

    return result;
}

function str8 FormatStrArgs(str8 buffer, const char *format, va_list args) {
    str8 result;
    uptr count = ProcessFormat(buffer, format, args);
    result.count = Min(count, buffer.count);
    result.data  = buffer.data;

    return result;
}

function str8 FormatStr(Memory_Arena *arena, const char *format, ...) {
    str8 result;

    va_list args;
    va_start(args, format);
    result = FormatStrArgs(arena, format, args);
    va_end(args);

    return result;
}

function str8 FormatStr(str8 buffer, const char *format, ...) {
    str8 result;

    va_list args;
    va_start(args, format);
    result = FormatStrArgs(buffer, format, args);
    va_end(args);

    return result;
}

function const char *CopyZ(Memory_Arena *arena, str8 str) {
    const char *result = 0;

    u8 *buffer = AllocArray(arena, u8, str.count + 1, Allocation_NoClear);

    CopySize(buffer, str.data, str.count);
    buffer[str.count] = 0;

    result = cast(const char *) buffer;
    return result;
}

function str8 CopyStr(Memory_Arena *arena, str8 str) {
    str8 result;
    result.count = str.count;
    result.data  = AllocArray(arena, u8, result.count);

    CopySize(result.data, str.data, result.count);

    return result;
}

function b32 StringsEqual(str8 a, str8 b, u32 flags) {
    b32 result = (a.count == b.count);
    if (result) {
        b32 no_case = (flags & StringCompare_NoCase);

        for (uptr it = 0; it < a.count; ++it) {
            u8 chr_a = a.data[it];
            u8 chr_b = b.data[it];

            if (no_case) {
                chr_a = ToUppercase(chr_a);
                chr_b = ToUppercase(chr_b);
            }

            if (chr_a != chr_b) {
                result = false;
                break;
            }
        }
    }

    return result;
}

function u8 ToUppercase(u8 c) {
    u8 result = (c >= 'a' && c <= 'z') ? (c - ' ') : c;
    return result;
}

function u8 ToLowercase(u8 c) {
    u8 result = (c >= 'A' && c <= 'Z') ? (c + ' ') : c;
    return result;
}
