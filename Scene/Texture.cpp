#include "Texture.h"
// Texture loader

unsigned char* loadPPM(const char* filename, int& width, int& height)
{
	cout << "Now reading in texture file " << filename << endl;
	const int BUFSIZE = 128;
	FILE* fp;
	unsigned int read;
	unsigned char* rawData;
	char buf[3][BUFSIZE];
	char* retval_fgets;
	size_t retval_sscanf;

	if((fp = fopen(filename, "rb")) == NULL)
	{
		cerr << "Error reading ppm file, could not locate " << filename << endl;
		width = 0;
		height = 0;
		return NULL;
	}

	// Read magic number:
	retval_fgets = fgets(buf[0], BUFSIZE, fp);

	// Read width and height:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while(buf[0][0] == '#');
	retval_sscanf = sscanf(buf[0], "%s %s", buf[1], buf[2]);
	width = atoi(buf[1]);
	height = atoi(buf[2]);

	// Read maxval:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while(buf[0][0] == '#');

	// Read image data:
	rawData = new unsigned char[width * height * 3];
	read = fread(rawData, width * height * 3, 1, fp);
	fclose(fp);
	if(read != 1)
	{
		cerr << "Error parsing ppm file, incomplete data!" << endl;
		delete[] rawData;
		width = 0;
		height = 0;
		return NULL;
	}

	cout << "Finished reading texture file!" << endl;

	return rawData;
}