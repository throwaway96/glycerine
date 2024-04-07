/*
 * glycerine.c
 * by throwaway96
 *
 * Licensed under GNU AGPL version 3 or later.
 *
 * https://github.com/throwaway96/glycerine
 */

#include <stddef.h>
#include <stdio.h>

#include <dlfcn.h>

#ifdef DEBUG
# define LOG(...) printf(__VA_ARGS__)
#else
# define LOG(...)
#endif

void *(*real_g_hash_table_lookup)(void *hash_table, const void *key) = NULL;
void *(*real_g_hash_table_new)(const void *hash_func, const void *key_equal_func) = NULL;

void *real_g_str_hash = NULL;
void *real_g_str_equal = NULL;

void *tgt = NULL;

void *g_hash_table_new(const void *hash_func, const void *key_equal_func) {
    LOG("hi from %s(%p, %p)\n", __func__, hash_func, key_equal_func);

    void *ret = real_g_hash_table_new(hash_func, key_equal_func);

    if ((tgt == NULL) && (hash_func == real_g_str_hash) && (key_equal_func == real_g_str_equal)) {
        LOG("set tgt to %p\n", ret);
        tgt = ret;
    }

    return ret;
}

void *g_hash_table_lookup(void *hash_table, const void *key) {
    LOG("hi from %s(%p, %p)\n", __func__, hash_table, key);

    if (hash_table == tgt) {
        LOG("found tgt\n");
        return (void *) 1;
    }

    return real_g_hash_table_lookup(hash_table, key);
}

__attribute__((__constructor__)) static void init(void) {
    LOG("hi from %s()\n", __func__);

    real_g_hash_table_lookup = dlsym(RTLD_NEXT, "g_hash_table_lookup");
    real_g_hash_table_new = dlsym(RTLD_NEXT, "g_hash_table_new");
    real_g_str_hash = dlsym(RTLD_DEFAULT, "g_str_hash");
    real_g_str_equal = dlsym(RTLD_DEFAULT, "g_str_equal");

    LOG("hash, eq = %p, %p\n", real_g_str_hash, real_g_str_equal);
    return;
}
