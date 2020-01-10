#include <iostream>
#include <algorithm>
#include <cctype>
#include <cstring>

#include "freetype/tttables.h"
#include "ttmkfdir.h"
#include "ttf.h"
#include "ttos2val.h"
#include "panose.h"
#include "encoding.h"

using namespace ttf;

namespace {
    inline bool
    isMacEnglish (FT_UShort  platform, FT_UShort  encoding, FT_UShort  language)
    {
	return ((platform == TT_PLATFORM_MACINTOSH) &&
		(encoding == TT_MAC_ID_ROMAN) &&
		(language == TT_MAC_LANGID_ENGLISH));
    }

    bool
    isUnicodeEnglish (FT_UShort  platform, FT_UShort  encoding, FT_UShort  language)
    {
	if ((platform == TT_PLATFORM_MICROSOFT) &&
	    ((encoding == TT_MS_ID_UNICODE_CS) || (encoding == TT_MS_ID_SYMBOL_CS))) {
	    return  ((language == TT_MS_LANGID_ENGLISH_UNITED_STATES) ||
		     (language == TT_MS_LANGID_ENGLISH_UNITED_KINGDOM) ||
		     (language == TT_MS_LANGID_ENGLISH_AUSTRALIA) ||
		     (language == TT_MS_LANGID_ENGLISH_CANADA) ||
		     (language == TT_MS_LANGID_ENGLISH_NEW_ZEALAND) ||
		     (language == TT_MS_LANGID_ENGLISH_IRELAND) ||
		     (language == TT_MS_LANGID_ENGLISH_SOUTH_AFRICA) ||
		     (language == TT_MS_LANGID_ENGLISH_JAMAICA) ||
		     (language == TT_MS_LANGID_ENGLISH_CARIBBEAN) ||
		     (language == TT_MS_LANGID_ENGLISH_BELIZE) ||
		     (language == TT_MS_LANGID_ENGLISH_TRINIDAD) ||
		     (language == TT_MS_LANGID_ENGLISH_ZIMBABWE) ||
		     (language == TT_MS_LANGID_ENGLISH_PHILIPPINES));
	} else if (platform == TT_PLATFORM_APPLE_UNICODE) {
	    return (language == TT_MAC_LANGID_ENGLISH); /* FIXME: is this correct ? */
	}

	return 0;
    }

}

bool
Face::MappingPresent (int cmapidx, NumericMapping *m, int enc_size, int start_code, bool enc_comp)
{
    int idx, missing = 0;
    FT_Set_Charmap (face, face->charmaps[cmapidx]);

    for (unsigned int i = start_code; i < m->size (); i++) {
	if ((*m)[i] < 0) 
	    continue;
	if ((idx = FT_Get_Char_Index (face, (*m)[i])) == 0)
	    missing++;
    }
    if (enc_size <= 256) {
        return (missing <= int (cmdline::instance()->option ("max-missing")));
    } else {
        return ((100 * missing/enc_size) <= int (cmdline::instance()->option ("max-missing-percentage")) & enc_comp);
    }
}

Face::Face (const std::string &filename)
{
    FT_Error fterror;
    string header_enc("-");
    int face_id, face_count;

    string::size_type pos = filename.rfind("/");
    if ( pos == string::npos ) FileName = filename;
        else FileName = filename.substr( pos + 1);

    if ((fterror = FT_Init_FreeType(&library)) != FT_Err_Ok){
        std::cout << "Warning: Can't initialize Library : " << FileName << "(" << fterror << ")" << std::endl;
	return;
    }

    face_id = 0; 
    face_count = 1;

    while(face_id < face_count) {


        if ((fterror = FT_New_Face(library, filename.c_str(), face_id, &face)) != FT_Err_Ok) {
            std::cout << "Warning: Can't initialize Face : " << FileName << "(" << fterror << ")" << std::endl;
	    return;
        }

        face_count = face->num_faces;

        if ((post = (TT_Postscript *) FT_Get_Sfnt_Table(face, ft_sfnt_post)) == 0){
            std::cout << "Warning: Can't get POST table : " << FileName << "(" << FT_Err_Post_Table_Missing << ")" << std::endl;
	    return;
    }

        if ((os2 = (TT_OS2 *) FT_Get_Sfnt_Table(face, ft_sfnt_os2)) == 0) {
            std::cout << "Warning: Can't get OS2 table : " << FileName << "(" << FT_Err_Table_Missing << ")" << std::endl;
	    return;
        }

        for (int j = 0; j < 32; j++) {
	    switch (os2->ulCodePageRange1 & (1 << j)) {
	        case TT_CODEPAGE_RANGE_932: /* Japanese */
                    header_enc = "ji";
	            break;
	        case TT_CODEPAGE_RANGE_936: /* Simplified Chinese */
                    header_enc = "gb";
	            break;
	        case TT_CODEPAGE_RANGE_949: /* Korean Wansung */
                    header_enc = "ks";
	            break;
	        case TT_CODEPAGE_RANGE_950: /* Traditional Chinese */
                    header_enc = "big";
	            break;
            }
        }
        /*
         * Iterate over all cmap entries.
         */
        for (int cmapidx = 0; cmapidx < face->num_charmaps; cmapidx++) {
    	    FT_UShort platform;
	    FT_UShort encoding;

	    /*
	     * get the id of the cmap entry.
	     */
    	    platform = face->charmaps[cmapidx]->platform_id;
	    encoding = face->charmaps[cmapidx]->encoding_id;

	    if ((platform == TT_PLATFORM_MICROSOFT) && (encoding == TT_MS_ID_SYMBOL_CS)) {
	        add_entries (std::string ("microsoft-symbol"),face_id);
	        continue;
	    }

	    /*
	     * make a search key from the cmap id.
	     */
	    NumericMapping::Key_t key = NumericMapping::cmapkey (platform, encoding);
	    /*
	     * find all the encodings that are based on this cmap.
	     */
	    typedef Encodings_t::const_iterator MI;
	    std::pair<MI, MI> bounds = Encodings::instance()->equal_range (key);

	    /*
	     * then look whether each of these encodings is present in this cmap.
	     */
	    for (Encodings_t::const_iterator i = bounds.first; i != bounds.second; i++) {
	        if (MappingPresent (cmapidx, 
	    		        i->second->mappings[key], 
				i->second->enc_size,
			       	i->second->start_code, 
				(header_enc.compare("-")?((i->second->names[0].find(header_enc) != string::npos)?1:0):1)
		    	        )) {
		    /*
		     * if the mapping is present, add all xlfd names for this mapping to the
		     * list of available font names.
		     */
		    for (unsigned int k = 0; k < i->second->names.size (); k++) {
	                add_entries (i->second->names[k],face_id);
		    }
	        }
	    }
        }

        /* All truetype fonts are Unicode */
        add_entries (std::string("iso10646-1"),face_id);

        /*
         * remove duplicates.
         */
        std::sort (xlfdnames.begin (), xlfdnames.end ());
        xlfdnames.erase (std::unique (xlfdnames.begin (), xlfdnames.end ()), xlfdnames.end ());

        if (cmdline::instance()->option ("debug-entries"))
	    add_entries (std::string("truetype-raw"),face_id);

        face_id++ ;
    }

}


const char *
Face::ExtractName (FT_UInt name_len, FT_Byte* name, bool unicode) const
{
    int          name_pos = 0;
    FT_UInt      i;
    int          increment;
    
    if (unicode) {
	name_len =  name_len < 512 ? name_len : 512;
	increment = 2;
	i = 1;
    } else {
	name_len =  name_len < 256 ? name_len : 256;
	increment = 1;
	i = 0;
    }

    for (; i < name_len; i += increment) {
	if (!isascii (name[i])) {
	    name[i] = '$';
	}
	name_buffer[name_pos++] = (name[i] == '-') ? '_' : name[i];
    }

    name_buffer[name_pos] = 0;

    return name_buffer;
}

const char *
Face::FontFamilyName (void) const
{
    int i, n;
    const char *result = 0;

    FT_Error   fterror;
    FT_UShort  platform;
    FT_UShort  encoding;
    FT_UShort  language;
    FT_UShort  NameId;
    FT_UInt    NameLen;
    FT_SfntName NamePtr;


    /* get the number of name strings in this font */
    if ((n = FT_Get_Sfnt_Name_Count (face)) > 0) {

	/* now look at every entry */
	for (i = 0; i < n; i++) {
	    if ((fterror = FT_Get_Sfnt_Name (face, i, &NamePtr)) != FT_Err_Ok) {
                    std::cout << "Warning: Can't SFNT name : " << FileName << "(" << fterror << ")" << std::endl;
	            return;
            };
            platform = NamePtr.platform_id;
            encoding = NamePtr.encoding_id;
            language = NamePtr.language_id;
            NameId = NamePtr.name_id;
            NameLen = NamePtr.string_len;

	    if ((NameId == TT_NAME_ID_FONT_FAMILY) || (NameId == TT_NAME_ID_PS_NAME)) {
		if (isMacEnglish (platform, encoding, language)) {
		    result = ExtractName (NameLen, NamePtr.string, false);
		} else if (isUnicodeEnglish (platform, encoding, language)) {
		    result = ExtractName (NameLen, NamePtr.string, true);
		}

		if (result)
		    return result;
	    }
	}
    }
    return "unknown";
}

const char *
Face::FoundryName (void) const
{

    char code[5];
    
    static struct {
	const char  code[5];	/* TTF foundry code */
	const char *xlfd;	/* xlfd foundry name */
    } *p, known_foundries[] = {
	{ "2REB", "2Rebels"},
	{ "3ip" , "Three Islands Press"},
	{ "3ip ", "Three Islands Press"},
	{ "ABC" , "Altek Instruments"},
	{ "ABC ", "Altek Instruments"},
	{ "ACUT", "Acute Type"},
	{ "ADBE", "adobe"},
	{ "AGFA", "Agfa Typographic Systems"},
	{ "ALTS", "altsys"},
	{ "AOP" , "An Art of Penguin"},
	{ "AOP ", "An Art of Penguin"},
	{ "APPL", "Apple"},
	{ "ARPH", "Arphic Technology Co."},
	{ "ATEC", "alltype"},
	{ "AZLS", "Azalea Software, Inc."},
	{ "B&H",  "b&h"},
	{ "B&H ", "b&h"},
	{ "BARS", "CIA (BAR CODES) UK"},
	{ "BERT", "Berthold"},
	{ "BITM", "Bitmap Software"},
	{ "BITS", "Bitstream"},
	{ "BLAH", "Mister Bla's Fontworx"},
	{ "BOYB", "I. Frances"},
	{ "BRTC", "Bear Rock Technologies"},
	{ "BWFW", "B/W Fontworks"},
	{ "C&C",  "Carter & Cone"},
	{ "C&C ", "Carter & Cone"},
	{ "CAK" , "pluginfonts.com"},
	{ "CAK ", "pluginfonts.com"},
	{ "CANO", "cannon"},
	{ "CASL", "H.W. Caslon & Company Ltd."},
	{ "COOL", "Cool Fonts"},
	{ "CTDL", "China Type Design Ltd."},
	{ "DAMA", "Dalton Maag Limited"},
	{ "DS"  , "Dainippon Screen Mfg. Co., Inc."},
	{ "DS  ", "Dainippon Screen Mfg. Co., Inc."},
	{ "DSCI", "Design Science, Inc."},
	{ "DTC",  "Digital Typeface Corp."},
	{ "DTC ", "Digital Typeface Corp."},
	{ "DTPS", "DTP Software"},
	{ "DUXB", "Duxbury Systems, Inc."},
	{ "DYNA", "DynaLab"},
	{ "ECF" , "emerald city fontwerks"},
	{ "ECF ", "emerald city fontwerks"},
	{ "EDGE", "Rivers Edge Corp."},
	{ "EFF" , "Electronic Font Foundry"},
	{ "EFF ", "Electronic Font Foundry"},
	{ "EFNT", "E Fonts L.L.C."},
	{ "ELSE", "elseware"},
	{ "EPSN", "epson"},
	{ "ERAM", "eraman"},
	{ "ESIG", "E-Signature"},
	{ "FBI",  "The Font Bureau, Inc."},
	{ "FBI ", "The Font Bureau, Inc."},
	{ "FCAB", "The Font Cabinet"},
	{ "FJ",   "Fujitsu"},
	{ "FJ  ", "Fujitsu"},
	{ "FONT", "Font Source"},
	{ "FS"  , "Formula Solutions"},
	{ "FS  ", "Formula Solutions"},
	{ "FSE" , "Font Source Europe"},
	{ "FSE ", "Font Source Europe"},
	{ "FSI" , "FSI Fonts und Software GmbH"},
	{ "FSI ", "FSI Fonts und Software GmbH"},
	{ "FTFT", "FontFont"},
	{ "FWRE", "Fontware Limited"},
	{ "GALA", "Galapagos"},
	{ "GD"  , "GD Fonts"},
	{ "GD  ", "GD Fonts"},
	{ "GLYF", "Glyph Systems"},
	{ "GPI",  "Gamma Productions, Inc."},
	{ "GPI ", "Gamma Productions, Inc."},
	{ "HILL", "Hill Systems"},
	{ "HOUS", "House Industries"},
	{ "HP",   "hp"},
	{ "HP  ", "hp"},
	{ "HY",   "HanYang System"},
	{ "HY  ", "HanYang System"},
	{ "IBM",  "IBM"},
	{ "IBM ", "IBM"},
	{ "IDEE", "IDEE TYPOGRAFICA"},
	{ "IDF",  "International Digital Fonts"},
	{ "IDF ", "International Digital Fonts"},
	{ "ILP",  "Indigenous Languages Project"},
	{ "ILP ", "Indigenous Languages Project"},
	{ "ITC",  "itc"},
	{ "ITC ", "itc"},
	{ "IMPR", "impress"},
	{ "ITF" , "International Typefounders, Inc."},
	{ "ITF ", "International Typefounders, Inc."},
	{ "KATF", "Kingsley/ATF"},
	{ "LANS", "Lanston Type Co., Ltd."},
	{ "LARA", "Larabiefonts"},
	{ "LEAF", "Interleaf, Inc."},
	{ "LETR", "letraset"},
	{ "LGX" , "Logix Research Institute, Inc."},
	{ "LGX ", "Logix Research Institute, Inc."},
	{ "LING", "Linguist's Software"},
	{ "LINO", "linotype"},
	{ "LP",   "LetterPerfect Fonts"},
	{ "LP  ", "LetterPerfect Fonts"},
	{ "LTRX", "lighttracks"},
	{ "MACR", "macromedia"},
	{ "MC"  , "Cerajewski Computer Consulting"},
	{ "MC  ", "Cerajewski Computer Consulting"},
	{ "MILL", "Millan"},
	{ "MJ"  , "Majus Corporation"},
	{ "MJ  ", "Majus Corporation"},
	{ "MLGC", "Micrologic Software"},
	{ "MONO", "monotype"},
	{ "MS",   "microsoft"},
	{ "MS  ", "microsoft"},
	{ "MSCR", "Majus Corporation"},
	{ "MT",   "monotype"},
	{ "MT  ", "monotype"},
	{ "MTY" , "Motoya Co. ,LTD."},
	{ "MTY ", "Motoya Co. ,LTD."},
	{ "MUTF", "CACHE Enterprise Sdn. Bhd"},
	{ "NB"  , "No Bodoni Typography"},
	{ "NB  ", "No Bodoni Typography"},
	{ "NDTC", "Neufville Digital"},
	{ "NEC",  "nec"},
	{ "NEC ", "nec"},
	{ "NIS" , "NIS Corporation"},
	{ "NIS ", "NIS Corporation"},
	{ "ORBI", "Orbit Enterprises, Inc."},
	{ "P22" , "P22 Inc."},
	{ "P22 ", "P22 Inc."},
	{ "PARA", "ParaType"},
	{ "PDWX", "Parsons Design Workx"},
	{ "PF"  , "Phil's Fonts, Inc."},
	{ "PF  ", "Phil's Fonts, Inc."},
	{ "PRFS", "Production First Software"},
	{ "QMSI", "QMS/Imagen"},
	{ "RICO", "Ricoh"},
	{ "RKFN", "R K Fonts"},
	{ "robo", "Buro Petr van Blokland"},
	{ "RUDY", "RudynFluffy"},
	{ "SAX" , "s.a.x. Software gmbh"},
	{ "SAX ", "s.a.x. Software gmbh"},
	{ "Sean", "The FontSite"},
	{ "SFI" , "Software Friends"},
	{ "SFI ", "Software Friends"},
	{ "SFUN", "Soft Union"},
	{ "SG"  , "Scooter Graphics"},
	{ "SG  ", "Scooter Graphics"},
	{ "SIG" , "Signature Software, Inc."},
	{ "SIG ", "Signature Software, Inc."},
	{ "skz" , "Celtic Lady's Fonts"},
	{ "skz ", "Celtic Lady's Fonts"},
	{ "SN"  , "SourceNet"},
	{ "SN  ", "SourceNet"},
	{ "SOHO", "Soft Horizons"},
	{ "SOS" , "Standing Ovations Software"},
	{ "SOS ", "Standing Ovations Software"},
	{ "STF" , "Brian Sooy & Co + Sooy Type Foundry"},
	{ "STF ", "Brian Sooy & Co + Sooy Type Foundry"},
	{ "STON", "ZHUHAI Stone"},
	{ "SUNW", "sunwalk fontworks"},
	{ "SWFT", "Swfte International"},
	{ "SYN" , "SynFonts"},
	{ "SYN ", "SynFonts"},
	{ "TDR" , "Tansin A. Darcos & Co."},
	{ "TDR ", "Tansin A. Darcos & Co."},
	{ "TF"  , "Treacyfaces/Headliners"},
	{ "TF  ", "Treacyfaces/Headliners"},
	{ "TILD", "SIA Tilde"},
	{ "TPTC", "TEST PILOT"},
	{ "TR"  , "Type Revivals"},
	{ "TR  ", "Type Revivals"},
	{ "TS"  , "TamilSoft Corporation"},
	{ "TS  ", "TamilSoft Corporation"},
	{ "UA"  , "UnAuthorized Type"},
	{ "UA  ", "UnAuthorized Type"},
	{ "URW",  "urw"},
	{ "URW ", "urw"},
	{ "VKP" , "Vijay K. Patel"},
	{ "VKP ", "Vijay K. Patel"},
	{ "VLKF", "Visualogik Technology & Design"},
	{ "VOG" , "Martin Vogel"},
	{ "VOG ", "Martin Vogel"},
	{ "Y&Y" , "y&y"},
	{ "Y&Y ", "y&y"},
	{ "ZeGr", "Zebra Font Factory"},
	{ "zeta", "Tangram Studio"},
	{ "ZSFT", "ZSoft"},
	{ {0,},   0}
    };

    code[0] = toupper(os2->achVendID[0]);
    code[1] = toupper(os2->achVendID[1]);
    code[2] = toupper(os2->achVendID[2]);
    code[3] = toupper(os2->achVendID[3]);
    code[4] = 0;

    if (code[0] == 0) { 
	return std::string (cmdline::instance()->option ("default-foundry")).c_str ();
    }
    
    for (p = known_foundries; p->xlfd; p++) {
	if (memcmp (p->code, code, strlen (code)) == 0) {
	    return p->xlfd;
	}
    }

    return std::string (cmdline::instance()->option ("default-foundry")).c_str ();
}

const char *
Face::PanoseWeight (void) const
{
    static char *panose_weight_latin[] = {
	"any",
	"no fit",
	"very light",
	"light",
	"thin",
	"book",
	"medium",
	"demi",
	"bold",
	"heavy",
	"black",
	"extra black",
    };

    switch (os2->panose[TT_PANOSE_FAMILY_TYPE]) {
    case TT_PANOSE_FAMILY_LATIN_TEXT:
    case TT_PANOSE_FAMILY_LATIN_HAND:
    case TT_PANOSE_FAMILY_LATIN_DECORATIVE:
	if (!((os2->panose[TT_PANOSE_WEIGHT] < TT_PANOSE_LT_WEIGHT_VERY_LIGHT) ||
	      (os2->panose[TT_PANOSE_WEIGHT] > TT_PANOSE_LT_WEIGHT_EXTRA_BLACK))) {
	    return panose_weight_latin[os2->panose[TT_PANOSE_WEIGHT]];
	}
	break;
    }

    return 0;
}

const char *
Face::OS2Weight (void) const
{
    switch (os2->usWeightClass) {
    case TT_FW_THIN: return "thin";
    case TT_FW_EXTRALIGHT: return "extra light";
    case TT_FW_LIGHT: return "light";
    case TT_FW_NORMAL: return "medium";
    case TT_FW_MEDIUM: return "medium";
    case TT_FW_SEMIBOLD: return "semi bold";
    case TT_FW_BOLD: return "bold";
    case TT_FW_EXTRABOLD: return "extra bold";
    case TT_FW_BLACK:  return "black";
    }

    return 0;
}

const char *
Face::Weight (void) const
{
    const char *result;
    
    if (cmdline::instance()->option ("panose") && ((result = PanoseWeight ()) != 0)) {
	return result;
    } else if ((result = OS2Weight ()) != 0) {
	return result;
    }

    /* last fall back */
    return os2->fsSelection & (1 << 5) ? "bold" : "medium";
}

const char *
Face::PanoseWidth (void) const
{
    static char *panose_spacing_latin_text [] = {
	"any",
	"no fit",
	"old style",
	"modern",
	"even width",
	"extended",
	"condensed",
	"very extended",
	"very condensed",
	"monospaced",
    };
    
    static char *panose_spacing_latin_decorative [] = {
	"any",
	"no fit",
	"super condensed",
	"very condensed",
	"condensed",
	"normal",
	"extended",
	"very extended",
	"super extended",
	"monospaced",
    };

    switch (os2->panose[TT_PANOSE_FAMILY_TYPE]) {
    case TT_PANOSE_FAMILY_LATIN_TEXT:
	if (!((os2->panose[TT_PANOSE_PROPORTION] < TT_PANOSE_LT_PROPORTION_OLD_STYLE) ||
	      (os2->panose[TT_PANOSE_PROPORTION] > TT_PANOSE_LT_PROPORTION_MONOSPACED))) {
	    return panose_spacing_latin_text[os2->panose[TT_PANOSE_PROPORTION]];
	}
	break;
	
    case TT_PANOSE_FAMILY_LATIN_DECORATIVE:
	if (!((os2->panose[TT_PANOSE_ASPECT] < TT_PANOSE_LD_SUPER_CONDENSED) ||
	      (os2->panose[TT_PANOSE_ASPECT] > TT_PANOSE_LD_MONOSPACED))) {
	    return panose_spacing_latin_decorative[os2->panose[TT_PANOSE_ASPECT]];
	}
	break;
    }

    return 0;
}

const char *
Face::OS2Width (void) const
{
    switch (os2->usWidthClass) {
    case TT_FWIDTH_ULTRA_CONDENSED: return "ultra condensed";
    case TT_FWIDTH_EXTRA_CONDENSED: return "extra condensed";
    case TT_FWIDTH_CONDENSED: return "condensed";
    case TT_FWIDTH_SEMI_CONDENSED: return "semi condensed";
    case TT_FWIDTH_NORMAL: return "normal";
    case TT_FWIDTH_SEMI_EXPANDED: return "semi expanded";
    case TT_FWIDTH_EXPANDED: return "expanded";
    case TT_FWIDTH_EXTRA_EXPANDED: return "extra expanded";
    case TT_FWIDTH_ULTRA_EXPANDED: return "ultra expanded";
    }

    return 0;
}

const char *
Face::Width (void) const
{
    const char *result;
    
    if (cmdline::instance()->option ("panose") && ((result = PanoseWidth ()) != 0)) {
	return result;
    } else if ((result = OS2Width ()) != 0) {
	return result;
    }
	
    /* last fall back */
    return "normal";
}

Face::~Face (void)
{
    FT_Done_Face (face);
    FT_Done_Library (library);
}

void Face::add_entries (const string &encodingname, int idx)
{
    xlfdnames.push_back (xlfd(idx,encodingname));

    if ((int (cmdline::instance()->option ("additional-entries"))) == 1) {
        if ((int (cmdline::instance()->option ("font-backend"))) == 2) {
            xlfdnames.push_back (xlfd_cjk (idx,encodingname,"ai=0.3:","medium","i"));
            xlfdnames.push_back (xlfd_cjk (idx,encodingname,"ds=y:","bold","r"));
            xlfdnames.push_back (xlfd_cjk (idx,encodingname,"ds=y:ai=0.3:","bold","i"));
        } else if ((int (cmdline::instance()->option ("font-backend"))) == 1) {
            xlfdnames.push_back (xlfd_cjk (idx,encodingname,"","bold","r"));
        } 
    }
}

const std::string
Face::xlfd (int idx,const string &encodingname) const
{
    std::string xlfdname;

    xlfdname = Collection (idx);
    xlfdname += FileName;
    xlfdname += ' ';
    xlfdname += '-';
    xlfdname += FoundryName ();
    xlfdname += '-';
    xlfdname += FontFamilyName ();
    xlfdname += '-';
    xlfdname += Weight ();
    xlfdname += '-';
    xlfdname += (os2->fsSelection & TT_SELECTION_ITALIC) ? 'i' : 'r';
    xlfdname += '-';
    xlfdname += Width ();
    xlfdname += "--0-0-0-0-";
    xlfdname += ((post->isFixedPitch) ? 'm' : ((face->num_glyphs > 2000) ? 'c' : 'p'));
    xlfdname += "-0-";
    xlfdname += encodingname;
    
    return xlfdname;
}

const std::string
Face::xlfd_cjk (int idx,
	        const string &encodingname,
		const string &p1,
		const string &p2,
		const string &p3) const
{
    std::string xlfdname;
    
    xlfdname = Collection (idx);
    xlfdname += p1;
    xlfdname += FileName;
    xlfdname += ' ';
    xlfdname += '-';
    xlfdname += FoundryName ();
    xlfdname += '-';
    xlfdname += FontFamilyName ();
    xlfdname += '-';
    xlfdname += p2;
    xlfdname += '-';
    xlfdname += p3;
    xlfdname += '-';
    xlfdname += Width ();
    xlfdname += "--0-0-0-0-";
    xlfdname += ((post->isFixedPitch) ? 'm' : ((face->num_glyphs > 2000) ? 'c' : 'p'));
    xlfdname += "-0-";
    xlfdname += encodingname;
    
    return xlfdname;
}

const std::string
Face::Collection (int idx) const
{
    std::string collection_str;
    
    char idx_char = idx + 48 ; 

    if (idx == 0) collection_str ="";
    else {
            if ((int (cmdline::instance()->option ("font-backend"))) == 1) {
                collection_str = ':';
                collection_str += idx_char;
                collection_str += ':'; 
	    } else {
                collection_str = ":fn=";
                collection_str += idx_char;
                collection_str += ':';
	    }
    }

    return collection_str;

}
