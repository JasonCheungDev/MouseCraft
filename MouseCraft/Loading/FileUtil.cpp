#include "FileUtil.h"

std::string EvaluatePath(const std::string path)
{
	std::string validPath = path;

	auto end = validPath.find("..");
	while (end != std::string::npos)
	{
		auto begin = validPath.find_last_of("/\\", end - 2);
		validPath = validPath.erase(begin + 1, end - begin + 2);
		end = validPath.find("..");
	}

	/* performance
	char array[] 
	char lastChar = ''
	char curChar = ''
	from i = back
		lastChar = curChar
		curChar = i
		if (lastChar && curChar == '.')
			
			// mark characters for removal until next directory 
			int slashCount = 3
			for j = i + 1 
				if char[j] == '/'
					slashCount-- 
					if slashCount == 0
						i = j
						break 
				aray[j] = MARKED

	// delete all marked characters 
	// can use double pointer (next_empty_index, current_index) 
	*/

	return validPath;
}
