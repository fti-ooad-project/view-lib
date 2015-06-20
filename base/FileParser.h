#ifndef __RFileParser_H__
#define __RFileParser_H__
#include <stdio.h>
#include <string>
#include <base/Fileloader.h>
typedef std::string Word;
#define RWORDS_COUNT 1
const Word res_word[RWORDS_COUNT] = { "$include" };
class FileParser
{
public:
	static uint stringLength(const char *a);
	static std::string genParsed(std::string fullname);
	static uint toSpace(const char *r);
	static uint getPreproc(Word const c);
	static bool compare(Word const a, Word const b);
	static uint getFileNamePosition(std::string &fullname);
};
#endif
