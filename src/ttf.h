/* -*- C++ -*-  */
#ifndef TTF_H__
#define TTF_H__

#include <string>
#include "freetype/freetype.h"
#include "freetype/tttables.h"
#include "freetype/ftsnames.h"
#include "freetype/ttnameid.h"
#include "freetype/fterrors.h"
#include "freetype/ftmodule.h"

#include "util.h"
#include "encoding.h"

namespace ttf {

    class Face {
    public:
	typedef std::vector<std::string> xlfdcontainer;
	Face (const std::string &filename);
	~Face (void);
	const char         *FontFamilyName (void) const;
	const char         *FoundryName (void) const;
	const char         *Weight (void) const;
	const char         *Width (void) const;
	xlfdcontainer       xlfdnames;
    protected:
	void                add_entries (const std::string &encodingname,
			                 int idx) ;
	const std::string   xlfd (int idx,
			          const std::string &encodingname) const;
	const std::string   xlfd_cjk (int idx,
			              const std::string &encodingname,
			              const std::string &p1,
			              const std::string &p2,
			              const std::string &p3) const;
	bool                MappingPresent (int cmapidx, NumericMapping *m, int enc_size, int start_code, bool enc_comp);
	const char         *OS2Weight (void) const;
	const char         *PanoseWeight (void) const;
	const char         *ExtractName (FT_UInt name_len, FT_Byte* name, bool unicode) const;
	const char         *OS2Width (void) const;
	const char         *PanoseWidth (void) const;
	const std::string   Collection (int idx) const;
	mutable char        name_buffer[257];
	FT_Library	    library;
	FT_Face             face;
	TT_OS2 	            *os2;
	TT_Postscript	    *post;
	std::string	    FileName;
    };
};

#endif /* TTF_H__ */


