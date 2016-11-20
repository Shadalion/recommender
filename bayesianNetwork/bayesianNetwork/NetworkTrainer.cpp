#include "stdafx.h"
#include "NetworkTrainer.h"


NetworkTrainer::NetworkTrainer(void)
{
}


NetworkTrainer::~NetworkTrainer(void)
{
}

//Determines what the user likes genre
void NetworkTrainer::fillUserGenresRelations(User * user, vector<string> * genres, map<int, Movie*> * movieMap, int * genresUsersCount) {
	//Можно genreModel убрать genreId и сделать по аналогии с genres
	for(int i = 0; i < genres->size(); i++) {
		GenreModel * model = new GenreModel();
		model -> genreId = i;
		model -> isFavorite = 0;
		user -> genresRelations.push_back(model);
	}

	int * genresRatings = new int[genres->size()];
	int totalUserRating = 0;
	
	for (int i = 0; i < genres->size(); i++) {
		genresRatings[i] = 0;
	}

	for(int i = 0; i < user -> moviesRatings.size(); i++) {
		int * genreIds = movieMap -> find(user->moviesRatings[i]->movieId)->second->genreIds;
		int movieGenresCount = movieMap -> find(user->moviesRatings[i]->movieId)->second->genreCount;
		for(int j = 0; j < movieGenresCount; j++) {
			int genreId = genreIds[j];
			genresRatings[genreId] += (user -> moviesRatings[i]->rating);
		}
		totalUserRating += (user -> moviesRatings[i]->rating);
	}

	//TODO Добавить проверку булевую на проценты
	for (int i = 0; i < genres->size(); i++) {
		float likeProbability = (float)genresRatings[i] / totalUserRating;
		if(likeProbability >= 0.2f) {
			user -> genresRelations[i] -> isFavorite = 1;
			genresUsersCount[i] ++;
		}
	}
}


void NetworkTrainer::fillGenderTables(map<int, User*> * userMap, float **genderTable, vector<string> * genres, int * genresUsersCount) {
	//TODO В будущем можно объединить 2 цикла в один, пока что отдельно для понимания
	for(map<int, User*>::iterator MapItor = userMap->begin(); MapItor != userMap->end(); ++MapItor)
	{
		User* user = (*MapItor).second;
		char gender = user->gender;
		int age = user->age;
		int occupation = user->occupation;
		//cout << gender << endl;
		for(int i = 0; i < user->genresRelations.size(); i++) {
			GenreModel * model = user->genresRelations[i];
			if(model -> isFavorite) {
				if(gender == 'M')
					genderTable[i][0]++;
				else
					genderTable[i][1]++;
			}
		}
	}

	for (int i = 0; i < genres->size(); i++) {
		for(int j = 0; j < 2; j++) {
			genderTable[i][j] /= (float)genresUsersCount[i];
			//cout << "genre i = " << i << "  j = " << j << " value " << genderTable[i][j] << endl; 
		}	 
	}

}