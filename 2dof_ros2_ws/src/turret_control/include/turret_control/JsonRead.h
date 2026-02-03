
#ifndef jsonReadCLASS_H
#define jsonReadCLASS_H
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <eigen3/Eigen/Dense>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace Eigen;
class jsonRead
{
private:
	rapidjson::Document document; // rapidjson document

public:
	/*
	 * Creates a rapidjson document variable and copies all the jsonRead from the file in the members of the jsonRead class.
	 * @param docName The name of the .json file.
	 */
	jsonRead(const char *docName);

	/*
	 * Default Deconstructor
	 */
	~jsonRead();

    	/*
	 * Reads a int value from a json file.
	 * @param key The key associated with the int value to retrieve.
	 * @return The int value.
	 */
	int readInt(const std::vector<std::string> &path);

	/*
	 * Reads a double value from a json file.
	 * @param key The key associated with the double value to retrieve.
	 * @return The double value.
	 */
	double readDouble(const std::vector<std::string> &path);


    float readFloat(const std::vector<std::string> &path);
	/*
	 * Reads a string from a json file.
	 * @param key The key associated with the string to retrieve.
	 * @return The string.
	 */
	const char *readString(const std::vector<std::string> &path);

	/*
	 * Reads an array of arrays from a json file and transforms it into an Eigen matrix.
	 * @param key The key associated with the array to retrieve.
	 * @return The Eigen matrix.
	 */
	Matrix<double, Dynamic, Dynamic> readMatrix(const std::vector<std::string> &path);

	/*
	 * Reads an array from a json file and transforms it into an Eigen vector.
	 * @param key The key associated with the array to retrieve.
	 * @return The Eigen vector.
	 */
	VectorXd readVector(const std::vector<std::string> &path);
};
#endif