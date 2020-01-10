/* -*- C++ -*-  */
#ifndef TTMKFDIRUTIL_H__
#define TTMKFDIRUTIL_H__

namespace util {
    template <class TYPE, class PRESENTED = TYPE> class Singleton {
    public:
	static PRESENTED *instance (void) {
	    static TYPE *val = 0;
	
	    if (!val)
		val = new TYPE ();

	    return val;
	}
    protected:
	Singleton (void) {};
	~Singleton (void) {};
    };
}; 

#endif // TTMKFDIRUTIL_H__
