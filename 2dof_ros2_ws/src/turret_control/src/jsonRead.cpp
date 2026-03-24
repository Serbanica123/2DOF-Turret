#include "turret_control/JsonRead.h"

jsonRead::jsonRead(const char *docName)
{
	std::ifstream file(docName); // json file initialization

	if (!file.is_open())
	{
		std::cerr << "Failed to open the JSON file." << std::endl;
		// Handle the error, return, or throw an exception as needed.
	}

	std::string json((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()); // std::string conversion of the json file

	file.close(); // close json file

	if (document.Parse(json.c_str()).HasParseError())
	{
		std::cerr << "Failed to parse the JSON content." << std::endl;
		std::cerr << "Parse error code: " << document.GetParseError() << std::endl;
		std::cerr << "Parse error offset: " << document.GetErrorOffset() << std::endl;
		// Handle the error, return, or throw an exception as needed.
	}

	document.Parse(json.c_str()); // parse the json string
}

jsonRead::~jsonRead()
{
	std::cout<< "Config Destroyed\n";
}

double jsonRead::readDouble(const std::vector<std::string> &path)
{
	const rapidjson::Value *val = &document;
	for (const auto &key : path)
	{
		if (!val->HasMember(key.c_str()))
			return 0.0;
		val = &(*val)[key.c_str()];
	}
	return (val->IsDouble()) ? val->GetDouble() : 0.0; // return the value if the variable parsed is a double
}

float jsonRead::readFloat(const std::vector<std::string> &path)
{
	const rapidjson::Value *val = &document;
	for (const auto &key : path)
	{
		if (!val->HasMember(key.c_str()))
			return 0.0;
		val = &(*val)[key.c_str()];
	}

	return (val->IsFloat()) ? val->GetFloat() : 0.0; // return the value if the variable parsed is a float
}

int jsonRead::readInt(const std::vector<std::string> &path)
{
	const rapidjson::Value *val = &document;
	for (const auto &key : path)
	{
		if (!val->HasMember(key.c_str()))
			return 0;
		val = &(*val)[key.c_str()];
	}
	return (val->IsInt()) ? val->GetInt() : 0; // return the value if the variable parsed is an int
}

const char *jsonRead::readString(const std::vector<std::string> &path)
{
	const rapidjson::Value *val = &document;
	for (const auto &key : path)
	{
		if (!val->HasMember(key.c_str()))
			return "";
		val = &(*val)[key.c_str()];
	}
	return (val->IsString()) ? val->GetString() : ""; // return the value if the variable parsed is a string
}

VectorXd jsonRead::readVector(const std::vector<std::string> &path)
{
	const rapidjson::Value *val = &document;
	for (const auto &key : path)
	{
		if (!val->HasMember(key.c_str()))
			return Eigen::VectorXd();
		val = &(*val)[key.c_str()];
	}
	if (val->IsArray()) // check if the parsed value is an array
	{
		unsigned int numElem = val->Size();
		Eigen::VectorXd eigenArray(numElem);
		for (rapidjson::SizeType i = 0; i < numElem; i++)
		{
			eigenArray(i) = (*val)[i].GetDouble(); // populate an Eigen array with the elements of the parsed array
		}
		return eigenArray;
	}
	return Eigen::VectorXd();
}

Matrix<double, Eigen::Dynamic, Eigen::Dynamic> jsonRead::readMatrix(const std::vector<std::string> &path)
{
	const rapidjson::Value *val = &document;
	for (const auto &key : path)
	{
		if (!val->HasMember(key.c_str()))
			return Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>();
		val = &(*val)[key.c_str()];
	}
	if (val->IsArray()) // check if the parsed value is an array
	{
		unsigned int numRows = val->Size();
		unsigned int numCols = (numRows > 0) ? (*val)[0].Size() : 0;
		Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> eigenMatrix(numRows, numCols);
		for (rapidjson::SizeType i = 0; i < numRows; i++)
		{
			for (rapidjson::SizeType j = 0; j < numCols; j++)
			{
				eigenMatrix(i, j) = (*val)[i][j].GetDouble(); // populate an Eigen matrix with the elements of the parsed array of arrays
			}
		}
		return eigenMatrix;
	}

	return Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>();
}