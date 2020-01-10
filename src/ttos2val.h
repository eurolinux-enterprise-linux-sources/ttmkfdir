#ifndef TTOS2VALS_H__
#define TTOS2VALS_H__

/* values defined for the usWeightClass field of the OS/2 table */
#define TT_FW_THIN        100 /* Thin                      */
#define TT_FW_EXTRALIGHT  200 /* Extra-light (Ultra-light) */
#define TT_FW_LIGHT       300 /* Light                     */
#define TT_FW_NORMAL      400 /* Normal (Regular)          */
#define TT_FW_MEDIUM      500 /* Medium                    */
#define TT_FW_SEMIBOLD    600 /* Semi-bold (Demi-bold)     */
#define TT_FW_BOLD        700 /* Bold                      */
#define TT_FW_EXTRABOLD   800 /* Extra-Bold (Ultra-bold)   */
#define TT_FW_BLACK       900 /* Black (Heavy)             */

/* values defined for the usWidthClass field of the OS/2 table */
#define TT_FWIDTH_ULTRA_CONDENSED 1 /* Ultra-condensed */
#define TT_FWIDTH_EXTRA_CONDENSED 2 /* Extra-condensed */
#define TT_FWIDTH_CONDENSED       3 /* Condensed       */
#define TT_FWIDTH_SEMI_CONDENSED  4 /* Semi-condensed  */
#define TT_FWIDTH_NORMAL          5 /* Medium (normal) */
#define TT_FWIDTH_SEMI_EXPANDED   6 /* Semi-expanded   */
#define TT_FWIDTH_EXPANDED        7 /* Expanded        */
#define TT_FWIDTH_EXTRA_EXPANDED  8 /* Extra-expanded  */
#define TT_FWIDTH_ULTRA_EXPANDED  9 /* Ultra-expanded  */

/* bit mask for fsSelection */
#define TT_SELECTION_ITALIC      (1 << 0) /* Font contains Italic characters,
					     otherwise they are upright. */
#define TT_SELECTION_UNDERSCORE  (1 << 1) /* Characters are underscored. */
#define TT_SELECTION_NEGATIVE    (1 << 2) /* Characters have their foreground
					     and background reversed. */
#define TT_SELECTION_OUTLINED    (1 << 3) /* Outline (hollow) characters,
					     otherwise they are solid. */
#define TT_SELECTION_STRIKEOUT   (1 << 4) /* Characters are overstruck. */
#define TT_SELECTION_BOLD        (1 << 5) /* Characters are emboldened. */
#define TT_SELECTION_REGULAR     (1 << 6) /* Characters are in the standard
					     weight/style for the font. */

/* values defined for the ulCodePageRange1 field of the OS/2 table */
#define TT_CODEPAGE_RANGE_1252   (1L << 0) /* Latin 1 */
#define TT_CODEPAGE_RANGE_1250   (1L << 1) /* Latin 2: Eastern Europe */
#define TT_CODEPAGE_RANGE_1251   (1L << 2) /* Cyrillic */
#define TT_CODEPAGE_RANGE_1253   (1L << 3) /* Greek */
#define TT_CODEPAGE_RANGE_1254   (1L << 4) /* Turkish */
#define TT_CODEPAGE_RANGE_1255   (1L << 5) /* Hebrew */
#define TT_CODEPAGE_RANGE_1256   (1L << 6) /* Arabic */
#define TT_CODEPAGE_RANGE_1257   (1L << 7) /* Windows Baltic */
#define TT_CODEPAGE_RANGE_1258   (1L << 8) /* Vietnamese */
/* 9-15   Reserved for Alternate ANSI */
#define TT_CODEPAGE_RANGE_874    (1L << 16) /* Thai */
#define TT_CODEPAGE_RANGE_932    (1L << 17) /* JIS/Japan */
#define TT_CODEPAGE_RANGE_936    (1L << 18) /* Chinese: Simplified */
#define TT_CODEPAGE_RANGE_949    (1L << 19) /* Korean Wansung */
#define TT_CODEPAGE_RANGE_950    (1L << 20) /* Chinese: Traditional */
#define TT_CODEPAGE_RANGE_1361   (1L << 21) /* Korean Johab */
/* 22-28   Reserved for Alternate ANSI & OEM */
#define TT_CODEPAGE_RANGE_MAC    (1L << 29) /* Mac (US Roman) */
#define TT_CODEPAGE_RANGE_OEM    (1L << 30) /* OEM Character Set */
#define TT_CODEPAGE_RANGE_SYMBOL (1L << 31) /* Symbol Character Set */
/* 32-47   Reserved for OEM */
#define TT_CODEPAGE_RANGE_869    (1L << 48) /* IBM Greek */
#define TT_CODEPAGE_RANGE_866    (1L << 49) /* MS-DOS Russian */
#define TT_CODEPAGE_RANGE_865    (1L << 50) /* MS-DOS Nordic */
#define TT_CODEPAGE_RANGE_864    (1L << 51) /* Arabic */
#define TT_CODEPAGE_RANGE_863    (1L << 52) /* MS-DOS Canadian French */
#define TT_CODEPAGE_RANGE_862    (1L << 53) /* Hebrew */
#define TT_CODEPAGE_RANGE_861    (1L << 54) /* MS-DOS Icelandic */
#define TT_CODEPAGE_RANGE_860    (1L << 55) /* MS-DOS Portuguese */
#define TT_CODEPAGE_RANGE_857    (1L << 56) /* IBM Turkish */
#define TT_CODEPAGE_RANGE_855    (1L << 57) /* IBM Cyrillic; primarily Russian */
#define TT_CODEPAGE_RANGE_852    (1L << 58) /* Latin 2 */
#define TT_CODEPAGE_RANGE_775    (1L << 59) /* MS-DOS Baltic */
#define TT_CODEPAGE_RANGE_737    (1L << 60) /* Greek; former 437 G */
#define TT_CODEPAGE_RANGE_708    (1L << 61) /* Arabic; ASMO 708 */
#define TT_CODEPAGE_RANGE_850    (1L << 62) /* WE/Latin 1 */
#define TT_CODEPAGE_RANGE_437    (1L << 63) /* US */

#endif /* TTOS2VALS_H__ */
