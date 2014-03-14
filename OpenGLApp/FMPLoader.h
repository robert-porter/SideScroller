#pragma once

#include <windows.h>
#include <GL/gl.h>
// Loads Flexible MaP files.  Tile maps created by mappy.

#define RBCHAR char	/* 8bits */
#define RBSHORT short	/* 16 bits */
#define RBINT int		/* 32 bits */
#define RBFLOAT float

#define MPYFLIP 1		/* enable flipped tiles */
//#define MPYROTATE 1	/* enable rotated tiles */

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

//#define MTSMOOTHCOLOURKEY 1	/* to prevent colourkey being drawn round edges */
#define MTRANSP8 0		/* Transparency index for paletted screen (0 to 255)*/
#define MTRANSPR 0xFF	/* 8bit RED transparency value for high/true colour */
#define MTRANSPG 0x00	/* 8bit GREEN transparency value for high/true colour */
#define MTRANSPB 0xFF	/* 8bit BLUE transparency value for high/true colour */

#define MER_NONE 0		/* All the horrible things that can go wrong */
#define MER_OUTOFMEM 1
#define MER_MAPLOADERROR 2
#define MER_NOOPEN 3
#define MER_WRONGMAPVER 4

#define AN_END -1			/* Animation types, AN_END = end of anims */
#define AN_NONE 0			/* No anim defined */
#define AN_LOOPF 1		/* Loops from start to end, then jumps to start etc */
#define AN_LOOPR 2		/* As above, but from end to start */
#define AN_ONCE 3			/* Only plays once */
#define AN_ONCEH 4		/* Only plays once, but holds end frame */
#define AN_PPFF 5			/* Ping Pong start-end-start-end-start etc */
#define AN_PPRR 6			/* Ping Pong end-start-end-start-end etc */
#define AN_PPRF 7			/* Used internally by playback */
#define AN_PPFR 8			/* Used internally by playback */
#define AN_ONCES 9		/* Used internally by playback */

/* structure defs */
typedef struct {				/* Structure for data blocks */
int bg, fg;					/* number of graphic */
int fg2, fg3;		 		/* more overlay blocks */
unsigned RBINT user1, user2;		/* user long data */
unsigned RBSHORT user3, user4;	/* user short data */
unsigned RBCHAR user5, user6, user7; /* user byte data */
unsigned RBCHAR tl : 1;			/* bits for collision detection */
unsigned RBCHAR tr : 1;
unsigned RBCHAR bl : 1;
unsigned RBCHAR br : 1;
unsigned RBCHAR trigger : 1;			/* transparency */
unsigned RBCHAR unused1 : 1;
unsigned RBCHAR unused2 : 1;
unsigned RBCHAR unused3 : 1;
} BLKSTR;

typedef struct {		/* Animation control structure */
int antype;			/* Type of anim, AN_? */
int andelay;		/* Frames to go before next frame */
int ancount;		/* Counter, decs each frame, till 0, then resets to andelay */
int anuser;			/* User info */
int ancuroff;		/* current offset in list (mapanimseqpt) */
int anstartoff;		/* start of blkstr offsets list, AFTER ref. blkstr offset */
int anendoff;		/* end of blkstr offsets list */
} ANISTR;

class FMPLoader
{
public:
	FMPLoader(void);
	~FMPLoader(void);

	int Load(char *, int);
	int ChangeLayer (int);
	BLKSTR * GetBlock (int, int);
	void SetBlock (int, int, int);
	int GetWidth() const { return mapwidth; }
	int GetHeight() const { return mapheight; }
	int GetNumLayers() const { return numLayers; }
	int GetLayersUsed(int n) { return layersUsed[n]; } //TODO: Change this.  Sort it or something
	

private:
	int DrawBG (int, int, int, int, RBFLOAT, RBFLOAT, RBFLOAT, RBFLOAT, RBFLOAT);
	int DrawFG (int, int, int, int, RBFLOAT, RBFLOAT, RBFLOAT, RBFLOAT, RBFLOAT);
	int DrawRisers (int, int, int, int, RBFLOAT, RBFLOAT, RBFLOAT, RBFLOAT, RBFLOAT);


	void UpdateAnims (void);


	int Decode (unsigned char *, int);
	int DecodeLayer (unsigned RBCHAR * mdatpt, int lnum);
	int DecodeMPHD (unsigned RBCHAR * mdatpt);
	int DecodeANDT (unsigned RBCHAR * mdatpt);
	int DecodeBGFX (unsigned RBCHAR * mdatpt);
	int DecodeBKDT (unsigned RBCHAR * mdatpt);
	int DecodeCMAP (unsigned RBCHAR * mdatpt);
	
	RBINT Getchksz (unsigned RBCHAR * locpt);
	int Getshort (unsigned RBCHAR * locpt);
	int Getlong (unsigned RBCHAR * locpt);

	void InitAnims (void);
	void FreeMem (void);
	int GenerateYLookup (void);



	
	RBINT maperror;
	RBINT mapislsb;
	int mapwidth, mapheight;
	int mapblockwidth, mapblockheight, mapblockdepth;
	int mapblockstrsize, mapnumblockstr, mapnumblockgfx, mapnumanimstr;
	unsigned RBCHAR * mapcmappt;
	BLKSTR * mapblockstrpt;
	ANISTR * mapanimstrpt;
	int * mapanimseqpt;
	int * mappt;
	int ** maparraypt;
	int * mapmappt[8];
	int ** mapmaparraypt[8];

	int mapstexindex;
	GLenum texFormat;

	int  numLayers;
	int layersUsed[8];
};
