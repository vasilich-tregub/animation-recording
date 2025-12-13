// imageDataURLtool.cpp : Defines the entry point for the application.
// The base64 encoder, which exploits bit fields and structures 
// instead of commonly used bit shifts and masks, is borrowed from 
// https://stackoverflow.com/questions/342409/how-do-i-base64-encode-decode-in-c#79840702
//

#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <algorithm>

constexpr auto min_imagemagic_size = 2;
constexpr auto max_imagemagic_size = 18;

constexpr auto min_imagefile_size = min_imagemagic_size + 1;

inline const auto png_imagemagic = std::vector<std::uint8_t>{ 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
inline const auto jpeg_imagemagic= std::vector<std::uint8_t>{ 0xFF, 0xD8, 0xFF };
inline const auto webp_imagemagic = std::vector<std::uint8_t>{ 0x52, 0x49, 0x46, 0x46 };
inline const auto gif_imagemagic = std::vector<std::uint8_t>{ 0x47, 0x49, 0x46, 0x38 };
inline const auto jpg_2000_imagemagic = std::vector<std::uint8_t>{ 0x00, 0x00, 0x00, 0x0C, 0x6A, 0x50, 0x20, 0x20, 0x0D, 0x0A, 0x87, 0x0A };
inline const auto bmp_imagemagic = std::vector<std::uint8_t>{ 0x42, 0x4D };

static auto datasceme_magic_map = std::vector<std::pair<std::string, std::vector<uint8_t>>>{

	{ "image/png", png_imagemagic },
	{ "image/jpeg", jpeg_imagemagic },
	{ "image/gif", gif_imagemagic },
	{ "image/webp", webp_imagemagic },
	{ "image/jp2", jpg_2000_imagemagic },
	{ "image/bmp", bmp_imagemagic },

};

typedef struct B64QuadBody {
	uint32_t a : 6;
	uint32_t b : 6;
	uint32_t c : 6;
	uint32_t d : 6;
	uint32_t : 8;
} B64Quad;

typedef union B64PtrBody {
	char    raw[3];
	B64Quad q;
} B64Chunk;

static char const b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "Command line to launch:" << std::endl;
		std::cout << ">" << argv[0] << " <imagefile>" << std::endl;
		return 1;
	}

	std::ifstream file(argv[1], std::ios::binary);
	if (!file) {
		std::cerr << "Error opening file." << std::endl;
		return 1;
	}

	// Determine file size
	file.seekg(0, std::ios::end);
	size_t fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	if (fileSize < min_imagefile_size) {
		std::cout << "File too short to be an image file." << std::endl;
		return -1;
	}

	// Allocate buffer and read data
	char* buffer = new char[fileSize];
	file.read(buffer, fileSize);
	file.close();

	std::string strDataScheme = "";
	for (auto iter : datasceme_magic_map) {
		if (std::equal(begin(iter.second), end(iter.second), (uint8_t*)buffer, (uint8_t*)buffer + iter.second.size()))
		{
			strDataScheme = iter.first;
			break;
		}
	}


	std::string dataURLprefix = "data:" + strDataScheme + ";base64,";
	B64Chunk c;

	size_t dataURLsize = dataURLprefix.size() + 4 * (fileSize / 3 + !!(fileSize % 3)) + 1;
	char* encoded = (char*)calloc(dataURLsize, 1);

	for (int i = 0; i < dataURLprefix.size(); ++i) {
		encoded[i] = dataURLprefix[i];
	}
	
	char* p_enc = encoded;

	if (p_enc == nullptr) return -1;

	p_enc += dataURLprefix.size();

	auto p_buf = buffer;

	while (fileSize > 2) {
		c.raw[2] = p_buf[0];
		c.raw[1] = p_buf[1];
		c.raw[0] = p_buf[2];
		*(p_enc++) = b64[c.q.d];
		*(p_enc++) = b64[c.q.c];
		*(p_enc++) = b64[c.q.b];
		*(p_enc++) = b64[c.q.a];
		p_buf += 3;
		fileSize -= 3;
	}
	switch (fileSize) {
	case 2:
		*(p_enc++) = b64[c.q.d];
		*(p_enc++) = b64[c.q.c];
		*(p_enc++) = b64[c.q.b];
		*(p_enc++) = '=';
		break;
	case 1:
		*(p_enc++) = b64[c.q.d];
		*(p_enc++) = b64[c.q.c];
		*(p_enc++) = '=';
		*(p_enc++) = '=';
		break;
	case 0:
	default:
		break;
	}

	std::ofstream outfile("textureBackgroundImage.js");

	outfile << "const textureBkgdDataURL = \"" << encoded << "\";" << std::endl;
	outfile.close();

	delete[] buffer; // Free memory
	delete[] encoded; // Free memory

	return 0;
}
