#pragma once
#include <string>

class Movie
{
public:
	int movieId;
	std::string title;
	int genreCount;
	int * genreIds;
	Movie(void);
	~Movie(void);
};

