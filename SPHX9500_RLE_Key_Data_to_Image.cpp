#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <bitset>
#include <algorithm>
#include <iomanip>
#include <vector>

using namespace std;

std::string string_to_hex(const std::string& in) {
	std::stringstream ss;

	ss << std::hex << std::setfill('0');
	for (size_t i = 0; in.length() > i; ++i) {
		ss << std::setw(2) << static_cast<unsigned int>(static_cast<unsigned char>(in[i])) << " ";
	}

	return ss.str();
}


std::string hex_to_string(const std::string& in) {
	std::string output;

	if ((in.length() % 2) != 0) {
		throw std::runtime_error("String is not valid length ...");
	}

	size_t cnt = in.length() / 2;

	for (size_t i = 0; cnt > i; ++i) {
		uint32_t s = 0;
		std::stringstream ss;
		ss << std::hex << in.substr(i * 2, 2);
		ss >> s;

		output.push_back(static_cast<unsigned char>(s));
	}

	return output;
}


//================================================================
// Command: GetFileName
// Type: void
// Description: Prints the filename of the program.
// Uses: char* arg0 (argv[0]), string, auto, printf
//================================================================

void GetFileName(const char* arg0)
{
	//Source of the following function: https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c answer #2.
	std::string s = std::string(arg0);

	std::vector<std::string> out;
	std::string delim = "\\";

	auto start = 0U;
	auto end = s.find(delim);
	while (end != std::string::npos)
	{
		start = end + delim.length();
		end = s.find(delim, start);
	}

	printf("%s", s.substr(start, end).c_str()); //Please be wary that ".c_str() MUST be added according to https://stackoverflow.com/questions/10865957/printf-with-stdstring answer #4.
}

void PrintUsage(char* arg0) {
	cout << "Usage: ";
	GetFileName(arg0);
	cout << " (key file separated from image) (data file separated from image) (output) ((horizontal) (vertical), optional)\n";
}
int main(int argc, char* argv[]) {
	if (argc == 1) {
		cout << "The following is not provided:\nKey file (split from image)\nData file (split from image)\nOutput file\n";
		PrintUsage(argv[0]);
	}
	if (argc == 2) {
		cout << "The following is not provided:\nData file (split from image)\nOutput file\n";
		PrintUsage(argv[0]);
	}
	if (argc == 3) {
		cout << "The following is not provided:\nOutput file\n";
		PrintUsage(argv[0]);
	}
	if (argc == 4) {
		std::ifstream inFile;
		inFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			inFile.open(argv[1], std::ios::binary); //open the input file
		} catch (std::system_error& e) {
			std::cerr << "FAILURE TO OPEN FILE" << std::endl;
		}

		std::stringstream strStream;
		strStream << inFile.rdbuf(); //read the file'

			//Data
		std::ifstream DataStream;
		DataStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			DataStream.open(argv[2], std::ios::binary); //open the input file
		}
		catch (std::system_error& e) {
			std::cerr << "FAILURE TO OPEN FILE" << std::endl;
		}
		//DataStreamOut
		ostringstream DataStreamOut;
		DataStreamOut << DataStream.rdbuf(); //read the file
		std::string DataStreamRaw(DataStreamOut.str()); //str holds the content of the file
		std::string Data = string_to_hex(DataStreamRaw);

		std::ofstream WriteTo;
		WriteTo.open(argv[3], std::ios::binary);

		std::string str = strStream.str(); //str holds the content of the file
		//string str = "ABCDEFGHIJKL";
		string final;
		for (int f = 0; f < str.length(); f = f + 4) {
			string strpart = str.substr(f, 4);
			reverse(strpart.begin(), strpart.end());
			final = final + strpart;
		}
		//cout << final << endl;
		string bits;
		string Key;

		for (std::size_t i = 0; i < final.size(); ++i)
		{
			bits = bitset<8>(final.c_str()[i]).to_string();
			//cout << bits << "\n";
			for (std::size_t i = 0; i < bits.size(); i = i + 1)
			{
				if (bits.substr(i, 1) == "1") {
					Key = Key + "ff ff ";
				}
				else if (bits.substr(i, 1) == "0") {
					Key = Key + "00 00 ";
				}
				//cout << bits.substr(i, 1) << "\n";
			}
		}
		//WriteTo << hex_to_string(final2);
		std::string Data_Partial;
		std::string Data_Final;
		int address = 0;
		for (int i = 0; i < Key.length(); i = i + 6) {
			std::string Key_Partial = Key.substr(i, 6);
			if (Key_Partial == "00 00 ") {
				//std::cout << "Data get mark detected.\n";
				Data_Partial = Data.substr(address, 6);
				address = address + 6;
				Data_Final = Data_Final + Data_Partial;
			}
			else if (Key_Partial == "ff ff ") {
				//std::cout << "Data copy mark detected.\n";
				Data_Final = Data_Final + Data_Partial;
			}
		}
		Data_Final.erase(remove(Data_Final.begin(), Data_Final.end(), ' '), Data_Final.end());
		WriteTo << hex_to_string(Data_Final);
		WriteTo.close();
	}
	if (argc == 6) {
		std::ifstream inFile;
		inFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			inFile.open(argv[1], std::ios::binary); //open the input file
		}
		catch (std::system_error& e) {
			std::cerr << "FAILURE TO OPEN FILE" << std::endl;
		}

		std::stringstream strStream;
		strStream << inFile.rdbuf(); //read the file'

			//Data
		std::ifstream DataStream;
		DataStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			DataStream.open(argv[2], std::ios::binary); //open the input file
		}
		catch (std::system_error& e) {
			std::cerr << "FAILURE TO OPEN FILE" << std::endl;
		}
		//DataStreamOut
		ostringstream DataStreamOut;
		DataStreamOut << DataStream.rdbuf(); //read the file
		std::string DataStreamRaw(DataStreamOut.str()); //str holds the content of the file
		std::string Data = string_to_hex(DataStreamRaw);

		std::ofstream WriteTo;
		WriteTo.open(argv[3], std::ios::binary);

		std::string str = strStream.str(); //str holds the content of the file
		//string str = "ABCDEFGHIJKL";
		string final;
		for (int f = 0; f < str.length(); f = f + 4) {
			string strpart = str.substr(f, 4);
			reverse(strpart.begin(), strpart.end());
			final = final + strpart;
		}
		//cout << final << endl;
		string bits;
		string Key;

		for (std::size_t i = 0; i < final.size(); ++i)
		{
			bits = bitset<8>(final.c_str()[i]).to_string();
			//cout << bits << "\n";
			for (std::size_t i = 0; i < bits.size(); i = i + 1)
			{
				if (bits.substr(i, 1) == "1") {
					Key = Key + "ff ff ";
				}
				else if (bits.substr(i, 1) == "0") {
					Key = Key + "00 00 ";
				}
				//cout << bits.substr(i, 1) << "\n";
			}
		}
		//WriteTo << hex_to_string(final2);
		std::string Data_Partial;
		std::string Data_Final;
		int address = 0;
		for (int i = 0; i < Key.length(); i = i + 6) {
			std::string Key_Partial = Key.substr(i, 6);
			if (Key_Partial == "00 00 ") {
				//std::cout << "Data get mark detected.\n";
				Data_Partial = Data.substr(address, 6);
				address = address + 6;
				Data_Final = Data_Final + Data_Partial;
			}
			else if (Key_Partial == "ff ff ") {
				//std::cout << "Data copy mark detected.\n";
				Data_Final = Data_Final + Data_Partial;
			}
		}
		int Horiz = atoi(argv[4]);
		std::stringstream hex_H;
		hex_H << std::hex << Horiz;
		std::string Horizontal = hex_H.str();
		std::string HorizToHex;
		if (Horizontal.length() == 1) {
			HorizToHex = "0" + Horizontal + " 00" + " 00 00 ";
		}
		else if (Horizontal.length() == 2) {
			HorizToHex = Horizontal + " 00" + " 00 00 ";
		}
		else if (Horizontal.length() == 3) {
			std::string P1 = Horizontal.substr(1, 2);
			std::string P2 = Horizontal.substr(0, 1);
			HorizToHex = P1 + " 0" + P2 + " 00 00 ";
		}
		else if (Horizontal.length() == 4) {
			std::string P1 = Horizontal.substr(2, 2);
			std::string P2 = Horizontal.substr(0, 2);
			HorizToHex = P1 + " " + P2 + " 00 00 ";
		}
		//cout << "Horizontal resolution: " << Horiz << ", To Hex : " << HorizToHex << "\n";

		int Vertic = atoi(argv[5]);
		std::stringstream hex_V;
		hex_V << std::hex << Vertic;
		std::string Vertical = hex_V.str();
		std::string VerticToHex;
		if (Vertical.length() == 1) {
			VerticToHex = "0" + Vertical + " 00" + " 00 00 ";
		}
		else if (Vertical.length() == 2) {
			VerticToHex = Vertical + " 00" + " 00 00 ";
		}
		else if (Vertical.length() == 3) {
			std::string P1 = Vertical.substr(1, 2);
			std::string P2 = Vertical.substr(0, 1);
			VerticToHex = P1 + " 0" + P2 + " 00 00 ";
		}
		else if (Vertical.length() == 4) {
			std::string P1 = Vertical.substr(2, 2);
			std::string P2 = Vertical.substr(0, 2);
			VerticToHex = P1 + " " + P2 + " 00 00 ";
		}
		//cout << "Vertical resolution: " << Vertic << ", To Hex : " << VerticToHex << "\n";
		string BMPHeader;
		BMPHeader = "42 4d 00 00 00 00 00 00 00 00 46 00 00 00 38 00 00 00 " + HorizToHex + VerticToHex + "01 00 10 00 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 f8 00 00 e0 07 00 00 1f 00 00 00 00 00 00 00 ";
		//cout << "BMP Header: " << BMPHeader << "\n";
		string FinalWithBMPHeader;
		FinalWithBMPHeader = BMPHeader + Data_Final + "00 00 ";
		FinalWithBMPHeader.erase(remove(FinalWithBMPHeader.begin(), FinalWithBMPHeader.end(), ' '), FinalWithBMPHeader.end());
		WriteTo << hex_to_string(FinalWithBMPHeader);
		WriteTo.close();
	}
}