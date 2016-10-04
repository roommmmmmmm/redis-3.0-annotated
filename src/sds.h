/* SDSLib, A C dynamic strings library
 *
 * Copyright (c) 2006-2010, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __SDS_H
#define __SDS_H

/*
 * 最大预分配长度
 */
#define SDS_MAX_PREALLOC (1024*1024)

#include <sys/types.h>
#include <stdarg.h>

/*
 * 类型别名，用于指向 sdshdr 的 buf 属性
 */
typedef char *sds;

/*
 * 保存字符串对象的结构
 */
struct sdshdr {

    // buf 中已占用空间的长度
    int len;

    // buf 中剩余可用空间的长度
    int free;

    // 数据空间
    char buf[];
};

/*
 * 返回 sds 实际保存的字符串的长度
 *
 * T = O(1)
 */
static inline size_t sdslen(const sds s) {
    struct sdshdr *sh = (void*)(s-(sizeof(struct sdshdr)));
    return sh->len;
}

/*
 * 返回 sds 可用空间的长度
 *
 * T = O(1)
 */
static inline size_t sdsavail(const sds s) {
    struct sdshdr *sh = (void*)(s-(sizeof(struct sdshdr)));
    return sh->free;
}

sds sdsnewlen(const void *init, size_t initlen); /* 根据给定的初始化字符串init和字符串长度 initlen,创建一个新的 sds */
sds sdsnew(const char *init); /* 根据给定字符串 init ，创建一个包含同样字符串的 sds */
sds sdsempty(void); /* 创建并返回一个只保存了空字符串 "" 的sds */
size_t sdslen(const sds s); /* 返回 sds 实际保存的字符串的长度 */
sds sdsdup(const sds s); /* 复制给定 sds 的副本 */
void sdsfree(sds s); /* 释放给定的sds */
size_t sdsavail(const sds s); /* 返回 sds 可用空间的长度 */
sds sdsgrowzero(sds s, size_t len); /* 将 sds 扩充至指定长度，未使用的空间以 0 字节填充 */
sds sdscatlen(sds s, const void *t, size_t len); /* 将长度为 len 的字符串 t 追加到 sds 的字符串末尾 */
sds sdscat(sds s, const char *t); /* 将给定字符串 t 追加到 sds 的末尾 */
sds sdscatsds(sds s, const sds t); /* 将另一个 sds 追加到一个 sds 的末尾 */
sds sdscpylen(sds s, const char *t, size_t len); /* 将字符串 t 的前 len 个字符复制到 sds s 当中，并在字符串的最后添加终结符 */
sds sdscpy(sds s, const char *t); /* 将字符串复制到 sds 当中，覆盖原有的字符 */

sds sdscatvprintf(sds s, const char *fmt, va_list ap); /* 打印函数，被 sdscatprintf 所调用 */
#ifdef __GNUC__
sds sdscatprintf(sds s, const char *fmt, ...) /* 打印任意数量个字符串，并将这些字符串追加到给定 sds 的末尾 */
    __attribute__((format(printf, 2, 3)));
#else
sds sdscatprintf(sds s, const char *fmt, ...); /* 打印任意数量个字符串，并将这些字符串追加到给定 sds 的末尾 */
#endif

sds sdscatfmt(sds s, char const *fmt, ...);
sds sdstrim(sds s, const char *cset); /* 对 sds 左右两端进行修剪，清除其中 cset 指定的所有字符 */
void sdsrange(sds s, int start, int end); /* 按索引对截取 sds 字符串的其中一段 */
void sdsupdatelen(sds s); /* 未使用此函数 */
void sdsclear(sds s); /* 在不释放 sds 的字符串空间的情况下，重置 sds 所保存的字符串为空字符串 */
int sdscmp(const sds s1, const sds s2); /* 比较两个sds */
sds *sdssplitlen(const char *s, int len, const char *sep, int seplen, int *count); /* 使用分隔符 sep 对 s 进行分割，返回一个 sds 字符串的数组(这是一个非常6的函数，写PHP的简直要震惊了) */
void sdsfreesplitres(sds *tokens, int count); /* 释放tokens数组中count个sds */
void sdstolower(sds s); /* 将sds字符串中的所有字符转换为小写 */
void sdstoupper(sds s); /* 将sds字符串中的所有字符转换为大写 */
sds sdsfromlonglong(long long value); /* 根据输入的long long值value,创建一个sds */
sds sdscatrepr(sds s, const char *p, size_t len); /* 这个函数类似过滤函数，可以看具体实现 */
sds *sdssplitargs(const char *line, int *argc); /* 非常强大的函数，具体可以看实现，实现中有详细的注释 */
sds sdsmapchars(sds s, const char *from, const char *to, size_t setlen); /* 字符串替换，将字符串 s 中，所有在 from 中出现的字符，替换成 to 中的字符 */
sds sdsjoin(char **argv, int argc, char *sep); /* 使用分割副sep对传入的数组进行拼接，返回拼接后的sds字符串 */

/* Low level functions exposed to the user API */
sds sdsMakeRoomFor(sds s, size_t addlen); /* 对sds中buf的长度进行扩展 */
void sdsIncrLen(sds s, int incr); /* 根据incr参数，增加sds的长度，缩减空余空间 */
sds sdsRemoveFreeSpace(sds s); /* 回收 sds 中的空闲空间，但不会对sds中已保存的内容进行修改 */
size_t sdsAllocSize(sds s); /* 返回给定 sds 分配的内存字节数 */

#endif
