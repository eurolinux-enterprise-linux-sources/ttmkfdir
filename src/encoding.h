/* -*- C++ -*-  */
#ifndef ENCODING_H__
#define ENCODING_H__

#include <vector>
#include <map>
#include <string>

#include "freetype/freetype.h"

#include "util.h"

class NumericMapping : public std::vector<int> {
public:
    typedef unsigned long int Key_t;
    NumericMapping (size_type n, FT_UShort p, FT_UShort enc):
	std::vector<int>(n),
	platform (p),
	encoding (enc)
    {
	for (size_type i =    0; i < 0x20; i++) (*this)[i] = -1;
	for (size_type i = 0x20; i < n;    i++) (*this)[i] = i;
    }

    static inline Key_t cmapkey (FT_UShort p, FT_UShort e) {
	return (p << 16) | e;
    }

    inline Key_t cmapkey (void) const {
	return cmapkey (platform, encoding);
    }
    
    ~NumericMapping (void) {};
    FT_UShort     platform;
    FT_UShort     encoding;
};

class Encoding;
class Encodings_t : public std::multimap<NumericMapping::Key_t, Encoding *> {
public:
    Encodings_t (void);
protected:
    void ReadEncodings (void);
};

typedef util::Singleton<Encodings_t> Encodings;

class Encoding {
public:
    Encoding (unsigned int s = 0x100):size(s) {};
    inline void AddMapping (NumericMapping *m) {
	mappings[m->cmapkey ()] = m;
    }

    typedef std::map<NumericMapping::Key_t, NumericMapping *> MappingMap_t;
    typedef struct {
	unsigned int size;
        unsigned int enc_size;
        unsigned int start_code;
	struct {
	    FT_UShort     platform;
	    FT_UShort     encoding;
	    int *mappingtable;
	} mapdata;
	const char *aliases[5];
    } BuiltinEncoding_t;
    Encoding (Encodings_t *, BuiltinEncoding_t *);
    std::vector<std::string> names;
    MappingMap_t             mappings;
    unsigned int             size;
    unsigned int             enc_size;
    unsigned int             start_code;
    static BuiltinEncoding_t builtin_encodings[];
};

#endif /* ENCODING_H__ */
