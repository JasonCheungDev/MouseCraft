#pragma once

#include <string>

// Simple utility function that helps evaluate a path string (eg. base/..base2/file.txt => base2/file.txt)
std::string EvaluatePath(const std::string path);


