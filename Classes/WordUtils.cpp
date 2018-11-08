#include <algorithm>
#include <random>

namespace WordUtils
{
	int calculateMoney(int money, unsigned int wordLength)
	{
		if (wordLength == 3)
		    money += 1;
		else if (wordLength ==4)
		    money += 2;
		else if (wordLength ==5)
		    money += 3;
		else if (wordLength ==6)
		    money += 5;
		else if (wordLength ==7)
		    money += 7;
		else if (wordLength ==8)
		    money += 10;
		else 
		    money += 20;
		return money;
	}

	std::string generateRandomLetters(int numLetters)
	{
	    std::random_device random_device;
	    std::mt19937 engine{random_device()};
	    std::uniform_int_distribution<int> dist(3, 5);
	    int numVowels = dist(engine);
	    int numConsonants = numLetters - numVowels;

	    std::string consonant = "BBBBBCCCCCCCCCDDDDDDDDDFFFGGGGGGGHHHHHJJLLLLLLLLLLLMMMMMMNNNNNNNNNNNNNNPPPPPPQRRRRRRRRRRRRRRSSSSSSSSSSSSSSSSSTTTTTTTTTTTTTTTTVVVXXZZ";
	    std::string vowels = "AAAAAEEEEEEIIIIIOOOOUU";
	    std::string ret;

	    for (int i = 0; i < numConsonants; i++)
	    {
	        std::mt19937 engine2{random_device()};
	        std::uniform_int_distribution<int> dist2(0, consonant.length() - 1);
	        ret += consonant.at(dist2(engine));    
	    }
	    for (int i = 0; i < numVowels; i++)
	    {
	        std::mt19937 engine2{random_device()};
	        std::uniform_int_distribution<int> dist2(0, vowels.length() - 1);
	        ret += vowels.at(dist2(engine));    
	    }
	    std::random_shuffle(ret.begin(), ret.end());
	    return ret;    
	}

}