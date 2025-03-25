#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int16_t BadChar_T[256] = {-1};

static void BM_Build_BadCharTable(uint8_t *pattern, uint16_t pattern_size)
{
    memset(BadChar_T, -1, sizeof(BadChar_T));

    /* build bad char table */
    for (uint16_t i = 0; i < pattern_size; i++)
        BadChar_T[pattern[i]] = i;
}

static void BM_Builf_Suffix_Prefix(uint8_t *pattern, uint16_t pattern_size, int16_t *p_Suffix, bool *p_Prefix)
{
    int16_t i = 0;
    int16_t j = 0;
    int16_t k = 0;

    for (i = 0; i < pattern_size; i++)
    {
        p_Suffix[i] = -1;
        p_Prefix[i] = false;
    }

    /* build suffix and prefix table (preprocess) */
    for (i = 0; i < pattern_size - 1; i++)
    {
        j = i;
        k = 0;

        while ((j >= 0) && (pattern[j] == pattern[pattern_size - k - 1]))
        {
            p_Suffix[k] = j;
            j--;
            k++;
        }

        if (j == -1)
            p_Prefix[k] = true;
    }
}

void BM_Build_GoodSuffix(uint8_t *pattern, uint16_t pattern_len, int16_t *suffix, bool *prefix, int16_t *good_suffix)
{
    for (uint16_t i = 0; i < pattern_len; i++)
        good_suffix[i] = pattern_len;

    for (int i = pattern_len - 1; i >= 0; i--)
    {
        if (suffix[i] != -1)
            good_suffix[pattern_len - 1 - suffix[i]] = pattern_len - 1 - i;
    }

    for (int i = 0; i < pattern_len - 1; i++)
    {
        if (prefix[i])
        {
            for (int16_t j = 0; j < pattern_len - 1 - i; j++)
            {
                if (good_suffix[j] == pattern_len)
                    good_suffix[j] = pattern_len - 1 - i;
            }
        }
    }
}

static int16_t BM_MoveByGS(uint16_t pattern_size, int16_t j, int16_t *p_Suffix, bool *p_Prefix)
{
    int k = pattern_size - j - 1;
    if (p_Suffix[k] != -1)
        return j - p_Suffix[k] + 1;

    for (int i = (k - 1); i >= 0; i--)
    {
        if (p_Prefix[i])
            return (pattern_size - i);
    }

    return pattern_size;
}

/* return match index */
int16_t BM_Search(const uint8_t *main_str, uint16_t str_size, char *pattern, uint16_t pattern_size)
{
    int16_t Suffix_T[pattern_size];
    bool Prefix_T[pattern_size];
    int16_t GoodSuffix_T[pattern_size];
    int16_t s = 0;
    int16_t j = 0;
    int16_t bad_char_shift = 0;
    int16_t good_suffix_shift = 0;

    if ((main_str == NULL) || (pattern == NULL) || (str_size < pattern_size))
        return -1;
    
    if (str_size == pattern_size)
    {
        if (memcmp(main_str, pattern, str_size) == 0)
            return 0;
        
        return -1;
    }
    else
    {
        BM_Build_BadCharTable((uint8_t *)pattern, pattern_size);
        BM_Builf_Suffix_Prefix((uint8_t *)pattern, pattern_size, Suffix_T, Prefix_T);
        BM_Build_GoodSuffix((uint8_t *)pattern, pattern_size, Suffix_T, Prefix_T, GoodSuffix_T);

        while (s <= str_size - pattern_size)
        {
            j = pattern_size - 1;
            while (j >= 0 && pattern[j] == main_str[s + j])
            {
                j--;
            }
            
            if (j >= 0)
            {
                bad_char_shift = j - BadChar_T[(unsigned char)main_str[s + j]];
                good_suffix_shift = GoodSuffix_T[j + 1];
                s += (bad_char_shift > good_suffix_shift) ? bad_char_shift : good_suffix_shift;
            }
            else
                return s;
        }
    }

    return -1;
}

int main()
{
    int16_t ret = -1;
    char *tmp = NULL;

    char *main_str = "abcdefghijklabacabaaababcaabbababcabacababcababmnopqrstuvwxyz";
    char *pattern = "ababcabab";

    ret = BM_Search(main_str, strlen(main_str), pattern, strlen(pattern));
    printf("ret = %d pattern size %d\n", ret, strlen(pattern));

    if (ret != -1)
    {
        tmp = main_str + ret;
        printf("main_str = %s\n", tmp);
    }

    return 0;
}