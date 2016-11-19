#include "stdafx.h"
#include "User.h"
#include "MovieRating.h"
#include <iostream>
#include <vector>

User::User(void)
{

}

void User::print() {
	std::cout << "Gender is " << gender << std::endl;
}
User::~User(void)
{
	for(int i = 0; i < moviesRatings.size(); i++) {
		delete moviesRatings[i];
	}

	for(int i = 0; i < genresRelations.size(); i++) {
		delete genresRelations[i];
	}
}
