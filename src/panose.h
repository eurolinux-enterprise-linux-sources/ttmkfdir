#ifndef TT_PANOSE_H__
#define TT_PANOSE_H__

/* index in the FreeType PANOSE array */
#define TT_PANOSE_FAMILY_TYPE     0 /* this is constant */
#define TT_PANOSE_SERIF_STYLE     1 /* for Latin Text */
#define TT_PANOSE_TOOL            1 /* for Latin Hand Writing */
#define TT_PANOSE_CLASS           1 /* for Latin Decorative */
#define TT_PANOSE_WEIGHT          2
#define TT_PANOSE_PROPORTION      3
#define TT_PANOSE_ASPECT          3
#define TT_PANOSE_CONTRAST        4
#define TT_PANOSE_STROKE_VARIANT  5
#define TT_PANOSE_ARM_STYLE       6
#define TT_PANOSE_LETTER_FORM     7
#define TT_PANOSE_MIDLINE         8
#define TT_PANOSE_XHEIGHT         9

/* values of the PANOSE Family kind */
#define TT_PANOSE_FAMILY_ANY                0
#define TT_PANOSE_FAMILY_NO_FIT             1
#define TT_PANOSE_FAMILY_LATIN_TEXT         2
#define TT_PANOSE_FAMILY_LATIN_HAND         3
#define TT_PANOSE_FAMILY_LATIN_DECORATIVE   4
#define TT_PANOSE_FAMILY_LATIN_SYMBOL       5

/* values of the PANOSE Weight filed for Latin Text fonts */
#define TT_PANOSE_LT_WEIGHT_ANY           0
#define TT_PANOSE_LT_WEIGHT_NO_FIT        1
#define TT_PANOSE_LT_WEIGHT_VERY_LIGHT    2
#define TT_PANOSE_LT_WEIGHT_LIGHT         3
#define TT_PANOSE_LT_WEIGHT_THIN          4
#define TT_PANOSE_LT_WEIGHT_BOOK          5
#define TT_PANOSE_LT_WEIGHT_MEDIUM        6
#define TT_PANOSE_LT_WEIGHT_DEMI          7
#define TT_PANOSE_LT_WEIGHT_BOLD          8
#define TT_PANOSE_LT_WEIGHT_HEAVY         9
#define TT_PANOSE_LT_WEIGHT_BLACK         10
#define TT_PANOSE_LT_WEIGHT_EXTRA_BLACK   11

/* values for the PANOSE PROPORTION field for Latin Text fonts */
#define TT_PANOSE_LT_PROPORTION_ANY            0
#define TT_PANOSE_LT_PROPORTION_NO_FIT         1
#define TT_PANOSE_LT_PROPORTION_OLD_STYLE      2
#define TT_PANOSE_LT_PROPORTION_MODERN         3
#define TT_PANOSE_LT_PROPORTION_EVEN_WIDTH     4
#define TT_PANOSE_LT_PROPORTION_EXTENDED       5
#define TT_PANOSE_LT_PROPORTION_CONDENSED      6
#define TT_PANOSE_LT_PROPORTION_VERY_EXTENDED  7
#define TT_PANOSE_LT_PROPORTION_VERY_CONDENSED 8
#define TT_PANOSE_LT_PROPORTION_MONOSPACED     9

/* values for the PANOSE ASPECT field for Latin Decorative fonts */
#define TT_PANOSE_LD_ANY                       0
#define TT_PANOSE_LD_NO_FIT                    1
#define TT_PANOSE_LD_SUPER_CONDENSED           2
#define TT_PANOSE_LD_VERY_CONDENSED            3
#define TT_PANOSE_LD_CONDENSED                 4
#define TT_PANOSE_LD_NORMAL                    5
#define TT_PANOSE_LD_EXTENDED                  6
#define TT_PANOSE_LD_VERY_EXTENDED             7
#define TT_PANOSE_LD_SUPER_EXTENDED            8
#define TT_PANOSE_LD_MONOSPACED                9

#endif /* TT_PANOSE_H__ */
