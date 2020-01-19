/* -*- C++ -*-  */
#ifndef DIRECTORY_H__
#define DIRECTORY_H__

#include <vector>
#include <string>

class directory : public std::vector<std::string> {
public:
    directory (const std::string &path = "") {
	if (path != "") scan (path);
    };
    virtual ~directory (void);
    void scan (const std::string &path = ".");
    virtual bool select (const char *name) const;
    const std::string &path (void) const {
	return dirpath;
    }
protected:
    std::string dirpath;
};

class ttfdirectory : public directory {
public:
    virtual bool select (const char *name) const;
};

#endif // DIRECTORY_H__
