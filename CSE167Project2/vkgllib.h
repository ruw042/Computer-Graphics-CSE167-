//referenced Vahe Karamian's file vkgllib for bitmap loading

#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>


using namespace std;

#define WINDOW_WIDTH	640				// Window Width  Default
#define WINDOW_HEIGHT 480				// Window Height Default

// definition of PI
#define PI						3.14159265

// Used to defien the title of the window
#define WINDOW_TITLE  "Vahe Karamian - OpenGL Terrain Generation - CodeProject Version 1.0"

// A simple structure to define a point whose coordinates are integers
/*typedef struct { GLint x, y; } GLintPoint;

// This structure is used to store the vertices of a polyline
typedef struct { int num; GLintPoint pt[100]; } GLintPointArray;

// Data for an Icosahedron
#define ICO_X	0.525731112119133606
#define ICO_Z	0.850650808352039932*/

/*static GLfloat vdataICO[12][3] =
{
	{ -ICO_X, 0.0, ICO_Z }, { ICO_X, 0.0, ICO_Z }, { -ICO_X, 0.0, -ICO_Z }, { ICO_X, 0.0, -ICO_Z },
	{ 0.0, ICO_Z, ICO_X }, { 0.0, ICO_Z, -ICO_X }, { 0.0, -ICO_Z, ICO_X }, { 0.0, -ICO_Z, -ICO_X },
	{ ICO_Z, ICO_X, 0.0 }, { -ICO_Z, ICO_X, 0.0 }, { ICO_Z, -ICO_X, 0.0 }, { -ICO_Z, -ICO_X, 0.0 }
};

static GLuint tindicesICO[20][3] =
{
	{ 1, 4, 0 }, { 4, 9, 0 }, { 4, 5, 9 }, { 8, 5, 4 }, { 1, 8, 4 },
	{ 1, 10, 8 }, { 10, 3, 8 }, { 8, 3, 5 }, { 3, 2, 5 }, { 3, 7, 2 },
	{ 3, 10, 7 }, { 10, 6, 7 }, { 6, 11, 7 }, { 6, 0, 11 }, {6, 1, 0 },
	{ 10, 1, 6 }, { 11, 0, 9 }, { 2, 11, 9 }, { 5, 2, 9 }, { 11, 2, 7 }
};*/

#ifndef WIN32

#pragma pack(2)
typedef struct                       /**** BMP file header structure ****/
{
    unsigned short bfType;           /* Magic number for file */
    unsigned int   bfSize;           /* Size of file */
    unsigned short bfReserved1;      /* Reserved */
    unsigned short bfReserved2;      /* ... */
    unsigned int   bfOffBits;        /* Offset to bitmap data */
} BITMAPFILEHEADER;
#pragma pack()

#  define BF_TYPE 0x4D42             /* "MB" */

typedef struct                       /**** BMP file info structure ****/
{
    unsigned int   biSize;           /* Size of info header */
    int            biWidth;          /* Width of image */
    int            biHeight;         /* Height of image */
    unsigned short biPlanes;         /* Number of color planes */
    unsigned short biBitCount;       /* Number of bits per pixel */
    unsigned int   biCompression;    /* Type of compression to use */
    unsigned int   biSizeImage;      /* Size of image data */
    int            biXPelsPerMeter;  /* X pixels per meter */
    int            biYPelsPerMeter;  /* Y pixels per meter */
    unsigned int   biClrUsed;        /* Number of colors used */
    unsigned int   biClrImportant;   /* Number of important colors */
} BITMAPINFOHEADER;

/*
 * Constants for the biCompression field...
 */

#  define BI_RGB       0             /* No compression - straight BGR data */
#  define BI_RLE8      1             /* 8-bit run-length compression */
#  define BI_RLE4      2             /* 4-bit run-length compression */
#  define BI_BITFIELDS 3             /* RGB bitmap with RGB masks */

typedef struct                       /**** Colormap entry structure ****/
{
    unsigned char  rgbBlue;          /* Blue value */
    unsigned char  rgbGreen;         /* Green value */
    unsigned char  rgbRed;           /* Red value */
    unsigned char  rgbReserved;      /* Reserved */
} RGBQUAD;

typedef struct                       /**** Bitmap information structure ****/
{
    BITMAPINFOHEADER bmiHeader;      /* Image header */
    RGBQUAD          bmiColors[256]; /* Image colormap */
} BITMAPINFO;

#endif /* !WIN32 */


// Data for Tetrahedron
static GLfloat P1T[3] = { -2, 3, 0 }; 
static GLfloat P2T[3] = { -3, 0, 0 };
static GLfloat P3T[3] = { -1, 0, 3 }; 
static GLfloat P4T[3] = { -4, 0, 0 };


// Calculating the Normalized Cross Product of Two Vectors
void normalize( float v[3] )
{
	GLfloat d = sqrt( float(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]) );
	if( d==0.0 )
	{
		cerr<<"zero length vector"<<endl;
		return;
	}
	v[0] /= d;
	v[1] /= d;
	v[2] /= d;
}

void normcrossprod( float v1[3], float v2[3], float out[3] )
{
	out[0] = v1[1]*v2[2] - v1[2]*v2[1];
	out[1] = v1[2]*v2[0] - v1[0]*v2[2];
	out[2] = v1[0]*v2[1] - v1[1]*v2[0];
	normalize( out );
}

////// Defines
#define BITMAP_ID 0x4D42		      // the universal bitmap ID
#define MAP_X	256				         // size of map along x-axis
#define MAP_Z	256				         // size of map along z-axis
#define MAP_SCALE	200.0f		         // the scale of the terrain map

////// Texture Information
BITMAPINFOHEADER	bitmapInfoHeader;	// temp bitmap info header
BITMAPINFOHEADER	landInfo;			// land texture info header
BITMAPINFOHEADER	secondInfo;			// land texture info header
BITMAPINFOHEADER	thirdInfo;			// land texture info header
BITMAPINFOHEADER	waterInfo;			// water texture info header

//AUX_RGBImageRec
unsigned char*	      imageData;		   // the map image data
unsigned char*       landTexture;	   // land texture data
unsigned char*       secondTexture;	   // land texture data
unsigned int		   land;			      // the land texture object
unsigned int		   second;			      // the second texture object
unsigned char*       thirdTexture;	   // land texture data
unsigned int		   third;			      // the second texture object

////// Terrain Data
float terrain[MAP_X][MAP_Z][3];		// heightfield terrain data (0-255); 256x256

// LoadBitmapFile
// desc: Returns a pointer to the bitmap image of the bitmap specified
//       by filename. Also returns the bitmap header information.
//		   No support for 8-bit bitmaps.
unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
	FILE *filePtr;							      // the file pointer
	BITMAPFILEHEADER	bitmapFileHeader;		// bitmap file header
	unsigned char		*bitmapImage;			// bitmap image data
	int					imageIdx = 0;		   // image index counter
	unsigned char		tempRGB;				   // swap variable

	// open filename in "read binary" mode
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return NULL;

	// read the bitmap file header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	
	// verify that this is a bitmap by checking for the universal bitmap id
	if (bitmapFileHeader.bfType != BITMAP_ID)
	{
		fclose(filePtr);
		return NULL;
	}

	// read the bitmap information header
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	// move file pointer to beginning of bitmap data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// allocate enough memory for the bitmap image data
	bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

	// verify memory allocation
	if (!bitmapImage)
	{
		free(bitmapImage);
		fclose(filePtr);
		return NULL;
	}

	// read in the bitmap image data
	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);

	// make sure bitmap image data was read
	if (bitmapImage == NULL)
	{
		fclose(filePtr);
		return NULL;
	}

	// swap the R and B values to get RGB since the bitmap color format is in BGR
	for (imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx+=3)
	{
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}

	// close the file and return the bitmap image data
	fclose(filePtr);
	return bitmapImage;
}

bool LoadTextures()
{
	// load the land texture data
	landTexture = LoadBitmapFile("/Users/runpingwang/Desktop/Assignment/CSE167/final\ project/CSE167Project2/grass.bmp", &landInfo);
	if (!landTexture)
		return false;

	// generate the land texture as a mipmap
	glGenTextures(1, &land);                  
	glBindTexture(GL_TEXTURE_2D, land);       
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, landInfo.biHeight, landInfo.biWidth, GL_RGB, GL_UNSIGNED_BYTE, landTexture);

    
    secondTexture = LoadBitmapFile("/Users/runpingwang/Desktop/Assignment/CSE167/final\ project/CSE167Project2/second.bmp", &secondInfo);
    if (!secondTexture)
        return false;
    
    // generate the land texture as a mipmap
    
    glGenTextures(1, &second);
    glBindTexture(GL_TEXTURE_2D, second);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, secondInfo.biHeight, secondInfo.biWidth, GL_RGB, GL_UNSIGNED_BYTE, secondTexture);
    
    thirdTexture = LoadBitmapFile("/Users/runpingwang/Desktop/Assignment/CSE167/final\ project/CSE167Project2/grass2.bmp", &thirdInfo);
    if (!thirdTexture)
        return false;
    
    // generate the land texture as a mipmap
    
    glGenTextures(1, &third);
    glBindTexture(GL_TEXTURE_2D, third);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, thirdInfo.biHeight, thirdInfo.biWidth, GL_RGB, GL_UNSIGNED_BYTE, thirdTexture);
    

    
	return true;
}