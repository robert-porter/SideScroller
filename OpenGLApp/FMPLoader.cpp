#include "FMPLoader.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <gl/gl.h>

#if 0
#define RBCHAR char	/* 8bits */
#define RBSHORT short	/* 16 bits */
#define RBINT int		/* 32 bits */
#define RBFLOAT float

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

#endif


int MapDrawBG (int, int, int, int, RBFLOAT, RBFLOAT, RBFLOAT, RBFLOAT, RBFLOAT);
int MapDrawFG (int, int, int, int, RBFLOAT, RBFLOAT, RBFLOAT, RBFLOAT, RBFLOAT);
int MapDrawRisers (int, int, int, int, RBFLOAT, RBFLOAT, RBFLOAT, RBFLOAT, RBFLOAT);
int MapLoad (char *, int);
int MapDecode (unsigned char *, int);
void MapInitAnims (void);
void MapUpdateAnims (void);
void MapFreeMem (void);
int MapGenerateYLookup (void);
int MapChangeLayer (int);
BLKSTR * MapGetBlock (int, int);
void MapSetBlock (int, int, int);

/*
extern int mapwidth, mapheight;
extern int mapblockwidth, mapblockheight, mapblockdepth;
extern int mapnumblockstr, mapnumblockgfx, mapnumanimstr;
extern int * mappt;
extern int ** maparraypt;
*/
/* Release 3 */




static const RBFLOAT maptexcrds[64] = {
0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, /*normal*/
1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, /*fliph*/
0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, /*flipv*/
1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, /*flipvh*/
};



int FMPLoader::GenerateYLookup (void)
{
int i, j;

	for (i=0;i<8;i++) {
		if (mapmaparraypt[i]!=NULL) { free (mapmaparraypt[i]); mapmaparraypt[i] = NULL; }
		if (mapmappt[i]!=NULL) {
			mapmaparraypt[i] = (int **)malloc (mapheight*sizeof(int *));
			if (mapmaparraypt[i] == NULL) return -1;
			for (j=0;j<mapheight;j++) mapmaparraypt[i][j] = (mapmappt[i]+(j*mapwidth));
			if (mapmappt[i] == mappt) maparraypt = mapmaparraypt[i];
		}
	}
	return 0;
}

BLKSTR * FMPLoader::GetBlock (int x, int y)
{
int * mymappt;

	if (maparraypt!= NULL) {
		mymappt = maparraypt[y]+x;
	} else {
		mymappt = mappt;
		mymappt += x;
		mymappt += y*mapwidth;
	}
	if (*mymappt>=0) return mapblockstrpt+mymappt[0];
	else return mapblockstrpt+mapanimseqpt[mapanimstrpt[(-*mymappt)-1].ancuroff];
}

void FMPLoader::SetBlock (int x, int y, int strvalue)
{
int * mymappt;

	if (maparraypt!= NULL) {
		mymappt = maparraypt[y]+x;
	} else {
		mymappt = mappt;
		mymappt += x;
		mymappt += y*mapwidth;
	}
	*mymappt = strvalue;
}

int FMPLoader::ChangeLayer (int newlyr)
{
	if (newlyr<0 || newlyr>7 || mapmappt[newlyr] == NULL) return -1;
	mappt = mapmappt[newlyr]; maparraypt = mapmaparraypt[newlyr];
	return newlyr;
}


RBINT FMPLoader::Getchksz (unsigned RBCHAR * locpt)
{
	return ((((RBINT) (locpt[0]))<<24)|(((RBINT) (locpt[1]))<<16)|
		(((RBINT) (locpt[2]))<<8)|((RBINT) (locpt[3])));
}

int FMPLoader::Getshort (unsigned RBCHAR * locpt)
{
	if (mapislsb)
	return ((((int) (locpt[1]))<<8)|((int) (locpt[0])));
	else
	return ((((int) (locpt[0]))<<8)|((int) (locpt[1])));
}

int FMPLoader::Getlong (unsigned RBCHAR * locpt)
{
	if (mapislsb)
	return ((((int) (locpt[3]))<<24)|(((int) (locpt[2]))<<16)|
		(((int) (locpt[1]))<<8)|((int) (locpt[0])));
	else
	return ((((int) (locpt[0]))<<24)|(((int) (locpt[1]))<<16)|
		(((int) (locpt[2]))<<8)|((int) (locpt[3])));
}

void FMPLoader::FreeMem (void)
{
int i;

	if (mapcmappt != NULL) { free (mapcmappt); mapcmappt = NULL; }
	if (mapblockstrpt != NULL) { free (mapblockstrpt); mapblockstrpt = NULL; }
	if (mapanimstrpt != NULL) { free (mapanimstrpt); mapanimstrpt = NULL; }
	if (mapanimseqpt != NULL) { free (mapanimseqpt); mapanimseqpt = NULL; }

	for (i=0;i<8;i++) { if (mapmappt[i] != NULL) { free (mapmappt[i]); mapmappt[i] = NULL; } }
	for (i=0;i<8;i++) { if (mapmaparraypt[i] != NULL)
		{ free (mapmaparraypt[i]); mapmaparraypt[i] = NULL; } }
	mappt = NULL; maparraypt = NULL;
}

int FMPLoader::DecodeMPHD (unsigned RBCHAR * mdatpt)
{
	mdatpt += 8;
	if (mdatpt[0] > 0) { maperror = MER_WRONGMAPVER; return -1; }
	if (mdatpt[2] == 1) mapislsb = 1; else mapislsb = 0;

	mapwidth = (int) Getshort (mdatpt+4);
	mapheight = (int) Getshort (mdatpt+6);
	mapblockwidth = (int) Getshort (mdatpt+12);
	mapblockheight = (int) Getshort (mdatpt+14);
	mapblockdepth = (int) Getshort (mdatpt+16);
	mapblockstrsize = (int) Getshort (mdatpt+18);
	mapnumblockstr = (int) Getshort (mdatpt+20);
	mapnumblockgfx = (int) Getshort (mdatpt+22);
	return 0;
}

int FMPLoader::DecodeCMAP (unsigned RBCHAR * mdatpt)
{
	mapcmappt = (unsigned char *)malloc (Getchksz(mdatpt+4));
	if (mapcmappt == NULL) { maperror = MER_OUTOFMEM; return -1; }

	memcpy (mapcmappt, mdatpt+8, Getchksz(mdatpt+4));
	return 0;
}

int FMPLoader::DecodeBKDT (unsigned RBCHAR * mdatpt)
{
int i, j;
BLKSTR * myblkstrpt;

	mapblockstrpt = (BLKSTR *)malloc (mapnumblockstr * (sizeof(BLKSTR)));
	if (mapblockstrpt == NULL) { maperror = MER_OUTOFMEM; return -1; }

	mdatpt += 8;
	myblkstrpt = mapblockstrpt;
	j = mapblockwidth*mapblockheight*((mapblockdepth+1)/8);
	for (i=0;i<mapnumblockstr;i++)
	{
		myblkstrpt->bg = (Getlong (mdatpt)/j);
		myblkstrpt->fg = (Getlong (mdatpt+4)/j);
		myblkstrpt->fg2 = (Getlong (mdatpt+8)/j);
		myblkstrpt->fg3 = (Getlong (mdatpt+12)/j);
		myblkstrpt->user1 = (Getlong (mdatpt+16));
		myblkstrpt->user2 = (Getlong (mdatpt+20));
		myblkstrpt->user3 = (Getshort (mdatpt+24));
		myblkstrpt->user4 = (Getshort (mdatpt+26));
		myblkstrpt->user5 = mdatpt[28];
		myblkstrpt->user6 = mdatpt[29];
		myblkstrpt->user7 = mdatpt[30];
		if (mdatpt[31]&0x01) myblkstrpt->tl = 1; else myblkstrpt->tl = 0;
		if (mdatpt[31]&0x02) myblkstrpt->tr = 1; else myblkstrpt->tr = 0;
		if (mdatpt[31]&0x04) myblkstrpt->bl = 1; else myblkstrpt->bl = 0;
		if (mdatpt[31]&0x08) myblkstrpt->br = 1; else myblkstrpt->br = 0;
		if (mdatpt[31]&0x10) myblkstrpt->trigger = 1; else myblkstrpt->trigger = 0;
		if (mdatpt[31]&0x20) myblkstrpt->unused1 = 1; else myblkstrpt->unused1 = 0;
		if (mdatpt[31]&0x40) myblkstrpt->unused2 = 1; else myblkstrpt->unused2 = 0;
		if (mdatpt[31]&0x80) myblkstrpt->unused3 = 1; else myblkstrpt->unused3 = 0;

		mdatpt += mapblockstrsize;
		myblkstrpt++;
	}

	return 0;
}

int FMPLoader::DecodeANDT (unsigned RBCHAR * mdatpt)
{
int i, j;
ANISTR * myanistrpt;
unsigned RBCHAR * countpt;
int * myseqpt;

	j = Getchksz(mdatpt+4);
	mdatpt += 8;
	countpt = (mdatpt + j)-16;
	mapnumanimstr = 0;
	while (((RBCHAR *)countpt)[0] != AN_END) { mapnumanimstr++; countpt-=16; }

	mapanimstrpt = (ANISTR *)malloc (mapnumanimstr * (sizeof(ANISTR)));
	if (mapanimstrpt == NULL) { maperror = MER_OUTOFMEM; return -1; }
	mapanimseqpt = (int *) malloc (((countpt-mdatpt)/4)*sizeof(int));
	if (mapanimseqpt == NULL) { maperror = MER_OUTOFMEM; return -1; }

	myseqpt = mapanimseqpt;
	while (mdatpt!=countpt) {
		myseqpt[0] = (Getlong (mdatpt)/32);
		myseqpt++; mdatpt += 4;
	}

	mdatpt += (mapnumanimstr*16);
	myanistrpt = mapanimstrpt;
	for (i=0;i<mapnumanimstr;i++)
	{
		myanistrpt->antype = (int) mdatpt[0];
		if (myanistrpt->antype == 255) myanistrpt->antype = -1;
		myanistrpt->andelay = (int) mdatpt[1];
		myanistrpt->ancount = (int) mdatpt[2];
		myanistrpt->anuser = (int) mdatpt[3];

		myanistrpt->ancuroff = (int) ((j + Getlong (mdatpt+4))/4);
		myanistrpt->anstartoff = (int) ((j + Getlong (mdatpt+8))/4);
		myanistrpt->anendoff = (int) ((j + Getlong (mdatpt+12))/4);

		myanistrpt++; mdatpt -= 16;
	}

	return 0;
}

int FMPLoader::DecodeBGFX (unsigned RBCHAR * mdatpt)
{
int i, j, k, tindex;
unsigned RBCHAR * tgfxpt, * tgfx2pt;

	tgfxpt = (unsigned char *)malloc (mapblockwidth*(mapblockheight+2)*4);
	if (tgfxpt == NULL) { maperror = MER_OUTOFMEM; return -1; }
	memset (tgfxpt, 0, mapblockwidth*(mapblockheight+2)*4);

	mdatpt += 8;
	tindex = mapstexindex;
	for (i=0;i<mapnumblockgfx;i++) {

		tgfx2pt = tgfxpt+(mapblockwidth*4);	// 1 pix border
		for (j=0;j<mapblockheight;j++) {
			for (k=0;k<mapblockwidth;k++) {
				switch (mapblockdepth) {
					case 8:
						if (mdatpt[0] == 0) tgfx2pt[k*4+3] = 0;
						else tgfx2pt[k*4+3] = 0xFF;
						tgfx2pt[k*4] = mapcmappt[mdatpt[0]*3];
						tgfx2pt[k*4+1] = mapcmappt[mdatpt[0]*3+1];
						tgfx2pt[k*4+2] = mapcmappt[mdatpt[0]*3+2];
						mdatpt++;
						break;
					case 15:
						tgfx2pt[k*4] = ((mdatpt[0])&0x7C)<<1;
						tgfx2pt[k*4+1] = ((((mdatpt[0])&0x3)<<3)|
							((mdatpt[1])>>5))<<3;
						tgfx2pt[k*4+2] = ((mdatpt[1])&0x1F)<<3;
						tgfx2pt[k*4] |= ((tgfx2pt[k*4]>>5)&0x07);
						tgfx2pt[k*4+1] |= ((tgfx2pt[k*4+1]>>5)&0x07);
						tgfx2pt[k*4+2] |= ((tgfx2pt[k*4+2]>>5)&0x07);
						if (tgfx2pt[k*4] == MTRANSPR &&
							tgfx2pt[k*4+1] == MTRANSPG &&
							tgfx2pt[k*4+2] == MTRANSPB) tgfx2pt[k*4+3] = 0;
						else tgfx2pt[k*4+3] = 0xFF;
						mdatpt += 2;
						break;
					case 16:
						tgfx2pt[k*4] = ((mdatpt[0])&0xF8);
						tgfx2pt[k*4+1] = ((((mdatpt[0])&0x7)<<3)|
							((mdatpt[1])>>5))<<2;
						tgfx2pt[k*4+2] = ((mdatpt[1])&0x1F)<<3;
						tgfx2pt[k*4] |= ((tgfx2pt[k*4]>>5)&0x07);
						tgfx2pt[k*4+1] |= ((tgfx2pt[k*4+1]>>6)&0x03);
						tgfx2pt[k*4+2] |= ((tgfx2pt[k*4+2]>>5)&0x07);
						if (tgfx2pt[k*4] == MTRANSPR &&
							tgfx2pt[k*4+1] == MTRANSPG &&
							tgfx2pt[k*4+2] == MTRANSPB) tgfx2pt[k*4+3] = 0;
						else tgfx2pt[k*4+3] = 0xFF;
						mdatpt += 2;
						break;
					case 24:
						tgfx2pt[k*4] = mdatpt[0];
						tgfx2pt[k*4+1] = mdatpt[1];
						tgfx2pt[k*4+2] = mdatpt[2];
						if (tgfx2pt[k*4] == MTRANSPR &&
							tgfx2pt[k*4+1] == MTRANSPG &&
							tgfx2pt[k*4+2] == MTRANSPB) tgfx2pt[k*4+3] = 0;
						else tgfx2pt[k*4+3] = 0xFF;
						mdatpt += 3;
						break;
					case 32:
						tgfx2pt[k*4] = mdatpt[1];
						tgfx2pt[k*4+1] = mdatpt[2];
						tgfx2pt[k*4+2] = mdatpt[3];
						tgfx2pt[k*4+3] = mdatpt[0];
						mdatpt += 4;
						break;
					default:
						break;
				}
			}
			tgfx2pt += (mapblockwidth*4);
		}

#ifdef MTSMOOTHCOLOURKEY
		if (mapblockdepth < 32) {
		tgfx2pt = tgfxpt;
		for (j=1;j<=mapblockheight;j++) {
			for (k=0;k<mapblockwidth;k++) {
				if (tgfx2pt[(j*mapblockwidth*4)+(k*4)+3] == 0) {
					l = -1;
					if (tgfx2pt[((j-1)*mapblockwidth*4)+(k*4)+3] != 0)
						{ l = j-1; m = k; }
					else if (tgfx2pt[(j*mapblockwidth*4)+((k-1)*4)+3] != 0)
						{ l = j; m = k-1; }
					else if (tgfx2pt[(j*mapblockwidth*4)+((k+1)*4)+3] != 0)
						{ l = j; m = k+1; }
					else if (tgfx2pt[((j+1)*mapblockwidth*4)+(k*4)+3] != 0)
						{ l = j+1; m = k; }

					if (l != -1) {
						tgfx2pt[(j*mapblockwidth*4)+k*4] = tgfx2pt[(l*mapblockwidth*4)+m*4];
						tgfx2pt[(j*mapblockwidth*4)+(k*4)+1] = tgfx2pt[(l*mapblockwidth*4)+(m*4)+1];
						tgfx2pt[(j*mapblockwidth*4)+(k*4)+2] = tgfx2pt[(l*mapblockwidth*4)+(m*4)+2];
					}
				}
			}
		}
		}
#endif

		glBindTexture (GL_TEXTURE_2D, tindex);
		glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_REPEAT);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //GL_REPEAT);
/* glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); */
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
/* glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); */
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexImage2D (GL_TEXTURE_2D, 0, texFormat, mapblockwidth, mapblockheight,
			0, texFormat, GL_UNSIGNED_BYTE, tgfxpt+(mapblockwidth*4));
		tindex++;
	}
	free (tgfxpt);
	return 0;
}

int FMPLoader::DecodeLayer (unsigned RBCHAR * mdatpt, int lnum)
{
int i, j;
int * mymappt;

	mapmappt[lnum] = (int *)malloc (mapwidth*mapheight*sizeof(int));
	if (mapmappt[lnum] == NULL) { maperror = MER_OUTOFMEM; return -1; }

	mdatpt += 8;
	mymappt = mapmappt[lnum];
	for (j=0;j<mapheight;j++) {
		for (i=0;i<mapwidth;i++) {
			*mymappt = (int) Getshort (mdatpt);
			if (*mymappt > 0x7FFF) { *mymappt -= 0x10000; *mymappt /= 16; }
			else { *mymappt /= 32; }
			mdatpt+=2; mymappt++;
		}
	}

	if (lnum == 0) mappt = mapmappt[lnum];
	return 0;
}

int FMPLoader::Decode (unsigned RBCHAR * fmappt, int starttextureindex)
{
unsigned RBCHAR * fmappospt;
RBINT mapfilesize;

	FreeMem ();
	mapstexindex = starttextureindex;
	fmappospt = fmappt;
	if (strncmp ((const char *)fmappt, "FORM", 4)) {
		maperror = MER_MAPLOADERROR;
		return -1;
	}

	mapfilesize = Getchksz (fmappt+4);

	if (strncmp ((const char *)fmappt+8, "FMAP", 4)) {
		maperror = MER_MAPLOADERROR;
		return -1;
	}

	fmappospt += 12;
	mapfilesize -= 4;

	numLayers = 0;

	while (mapfilesize > 0) {
		if (!strncmp ((const char *)fmappospt, "MPHD", 4)) DecodeMPHD (fmappospt);
		if (!strncmp ((const char *)fmappospt, "CMAP", 4)) DecodeCMAP (fmappospt);
		if (!strncmp ((const char *)fmappospt, "BKDT", 4)) DecodeBKDT (fmappospt);
		if (!strncmp ((const char *)fmappospt, "ANDT", 4)) DecodeANDT (fmappospt);
		if (!strncmp ((const char *)fmappospt, "BGFX", 4)) DecodeBGFX (fmappospt);
		if (!strncmp ((const char *)fmappospt, "BODY", 4))
		{
			layersUsed[numLayers] = 0;
			numLayers++;
			DecodeLayer (fmappospt, 0);
		}
		if (!strncmp ((const char *)fmappospt, "LYR1", 4)) 
		{
			layersUsed[numLayers] = 1;
			numLayers++;
			DecodeLayer (fmappospt, 1);
		}
		if (!strncmp ((const char *)fmappospt, "LYR2", 4))
		{
			layersUsed[numLayers] = 2;
			numLayers++;
			DecodeLayer (fmappospt, 2);
		}
		if (!strncmp ((const char *)fmappospt, "LYR3", 4)) 
		{
			layersUsed[numLayers] = 3;
			numLayers++;
			DecodeLayer (fmappospt, 3);
		}
		if (!strncmp ((const char *)fmappospt, "LYR4", 4)) 
		{
			layersUsed[numLayers] = 4;
			numLayers++;
			DecodeLayer (fmappospt, 4);
		}
		if (!strncmp ((const char *)fmappospt, "LYR5", 4))
		{			
			layersUsed[numLayers] = 5;
			numLayers++;
			DecodeLayer (fmappospt, 5);
		}
		if (!strncmp ((const char *)fmappospt, "LYR6", 4))
		{			
			layersUsed[numLayers] = 6;
			numLayers++;
			DecodeLayer (fmappospt, 6);
		}
		if (!strncmp ((const char *)fmappospt, "LYR7", 4)) 
		{
			layersUsed[numLayers] = 7;
			numLayers++;
			DecodeLayer (fmappospt, 7);
		}
		mapfilesize -= 8;
		mapfilesize -= Getchksz (fmappospt+4);
		fmappospt += Getchksz (fmappospt+4);
		fmappospt += 8;

		if (maperror != MER_NONE) { FreeMem (); return -1; }
	}
	return 0;
}

int FMPLoader::Load(char * mapname, int starttextureindex)
{
int mretval;
FILE * fpt;
RBCHAR idtag[4];
unsigned RBCHAR tempc;
unsigned RBCHAR * fmappt;
RBINT mapfilesize;

	maperror = MER_NONE;
	fpt = fopen (mapname, "rb");
	if (fpt == NULL) { maperror = MER_NOOPEN; return -1; }

	mapfilesize = 0;
	maperror = MER_MAPLOADERROR;
	if (fread (&idtag[0], 1, 1, fpt) == 1) {
	if (fread (&idtag[1], 1, 1, fpt) == 1) {
	if (fread (&idtag[2], 1, 1, fpt) == 1) {
	if (fread (&idtag[3], 1, 1, fpt) == 1) {
	if (fread (&tempc, 1, 1, fpt) == 1) {
	mapfilesize = (((RBINT) tempc)<<24);
	if (fread (&tempc, 1, 1, fpt) == 1) {
	mapfilesize |= (((RBINT) tempc)<<16);
	if (fread (&tempc, 1, 1, fpt) == 1) {
	mapfilesize |= (((RBINT) tempc)<<8);
	if (fread (&tempc, 1, 1, fpt) == 1) {
	mapfilesize |= ((RBINT) tempc);
	mapfilesize += 8;
	if (!strncmp (idtag, "FORM", 4)) maperror = MER_NONE;
	} } } } } } } }

	if (maperror != MER_NONE) { fclose (fpt); return -1; }

	fmappt = (unsigned char *)malloc (mapfilesize);
	if (fmappt == NULL) { fclose (fpt); maperror = MER_OUTOFMEM; return -1; }

	fseek (fpt, 0, SEEK_SET);
	if (fread (fmappt, 1, mapfilesize, fpt) != mapfilesize) {
		fclose (fpt);
		free (fmappt);
		maperror = MER_MAPLOADERROR;
		return -1;
	}
	fclose (fpt);

	mretval = Decode (fmappt, starttextureindex);
	free (fmappt);
	return mretval;
}


int FMPLoader::DrawBG (int mx, int my, int w, int h, RBFLOAT x, RBFLOAT y, RBFLOAT z,
	RBFLOAT xadd, RBFLOAT yadd)
{
int wtemp, tid;
RBFLOAT mxtemp;
int * mymappt, * tempmappt;
const RBFLOAT * tc;
BLKSTR * blk;

	mymappt = mappt;
	mymappt += mx + (my * mapwidth);
	tempmappt = mymappt;
	wtemp = w; mxtemp = x;
 
 	while (h) {
		w = wtemp; x = mxtemp; while (w) {
			if (*mymappt >= 0) blk = mapblockstrpt+(*mymappt);
			else
			blk = mapblockstrpt+(mapanimseqpt[mapanimstrpt[(-*mymappt)-1].ancuroff]);
			tid = mapstexindex+blk->bg;

			tc = maptexcrds;
#ifdef MPYFLIP
			if (blk->unused2) tc += 16;
			if (blk->unused3) tc += 32;
#endif
#ifdef MPYROTATE
			if (blk->user7 < 4) tc += (blk->user7 * 2);
#endif
			glBindTexture (GL_TEXTURE_2D, tid);
			glBegin (GL_QUADS);
			glNormal3f (0, 0, -1);
			glTexCoord2f (tc[0], tc[1]);
			glVertex3f (x, y, z);
			glTexCoord2f (tc[2], tc[3]);
			glVertex3f (x+xadd, y, z);
			glTexCoord2f (tc[4], tc[5]);
			glVertex3f (x+xadd, y+yadd, z);
			glTexCoord2f (tc[6], tc[7]);
			glVertex3f (x, y+yadd, z);
			glEnd ();
			x += xadd;
			mymappt++;
			w--;
		}
		tempmappt += mapwidth; mymappt = tempmappt;
		y += yadd;
		h--;
		tid++;
	}
	return 0;
}

int FMPLoader::DrawFG (int mx, int my, int w, int h, RBFLOAT x, RBFLOAT y, RBFLOAT z,
	RBFLOAT xadd, RBFLOAT yadd)
{
int wtemp, tid;
RBFLOAT mxtemp;
int * mymappt, * tempmappt;
const RBFLOAT * tc;
BLKSTR * blk;

	mymappt = mappt;
	mymappt += mx + (my * mapwidth);
	tempmappt = mymappt;
	wtemp = w; mxtemp = x;
 
 	while (h) {
		w = wtemp; x = mxtemp; while (w) {
			if (*mymappt >= 0) blk = mapblockstrpt+(*mymappt);
			else
			blk = mapblockstrpt+(mapanimseqpt[mapanimstrpt[(-*mymappt)-1].ancuroff]);
			tid = mapstexindex+blk->fg;

			tc = maptexcrds;
#ifdef MPYFLIP
			if (blk->unused2) tc += 16;
			if (blk->unused3) tc += 32;
#endif
#ifdef MPYROTATE
			if (blk->user7 < 4) tc += (blk->user7 * 2);
#endif
			glBindTexture (GL_TEXTURE_2D, tid);
			glBegin (GL_QUADS);
			glNormal3f (0, 0, -1);
			glTexCoord2f (tc[0], tc[1]);
			glVertex3f (x, y, z);
			glTexCoord2f (tc[2], tc[3]);
			glVertex3f (x+xadd, y, z);
			glTexCoord2f (tc[4], tc[5]);
			glVertex3f (x+xadd, y+yadd, z);
			glTexCoord2f (tc[6], tc[7]);
			glVertex3f (x, y+yadd, z);
			glEnd ();
			x += xadd;
			mymappt++;
			w--;
		}
		tempmappt += mapwidth; mymappt = tempmappt;
		y += yadd;
		h--;
		tid++;
	}
	return 0;
}

int FMPLoader::DrawRisers (int mx, int my, int w, int h, RBFLOAT x, RBFLOAT y, RBFLOAT z,
	RBFLOAT xadd, RBFLOAT yadd)
{
const RBFLOAT * tc;
int wtemp, tid = 0;
RBFLOAT mxtemp;
int * mymappt, * tempmappt;
BLKSTR * myblk;
RBFLOAT zadd;

	mymappt = mappt;
	mymappt += mx + (my * mapwidth);
	tempmappt = mymappt;
	wtemp = w; mxtemp = x;
 
 	while (h) {
		w = wtemp; x = mxtemp; while (w) {
			zadd = 0.0;
			if (*mymappt >= 0) myblk = mapblockstrpt + *mymappt;
			else
	{ myblk = mapblockstrpt+mapanimseqpt[mapanimstrpt[(-*mymappt)-1].ancuroff]; }
			while (myblk->unused1) {
			if (myblk->bg) {
			glEnable (GL_TEXTURE_2D);
			glBindTexture (GL_TEXTURE_2D, myblk->bg+mapstexindex); //west
			glBegin (GL_QUADS);
			glNormal3f (-1, 0, 0);
			glTexCoord2f (0.0f, 1.0f);
			glVertex3f (x, y, z+zadd);
			glTexCoord2f (0.0f, 0.0f);
			glVertex3f (x, y, z+zadd+xadd);
			glTexCoord2f (1.0f, 0.0f);
			glVertex3f (x, y+yadd, z+zadd+xadd);
			glTexCoord2f (1.0f, 1.0f);
			glVertex3f (x, y+yadd, z+zadd);
			glEnd ();
			}

			if (myblk->fg) {
			glEnable (GL_TEXTURE_2D);
			glBindTexture (GL_TEXTURE_2D, myblk->fg+mapstexindex); //north
			glBegin (GL_QUADS);
			glNormal3f (0, -1, 0);
			glTexCoord2f (0.0f, 1.0f);
			glVertex3f (x+xadd, y, z+zadd);
			glTexCoord2f (0.0f, 0.0f);
			glVertex3f (x+xadd, y, z+zadd+xadd);
			glTexCoord2f (1.0f, 0.0f);
			glVertex3f (x, y, z+zadd+xadd);
			glTexCoord2f (1.0f, 1.0f);
			glVertex3f (x, y, z+zadd);
			glEnd ();
			}

			if (myblk->fg2) {
			glEnable (GL_TEXTURE_2D);
			glBindTexture (GL_TEXTURE_2D, myblk->fg2+mapstexindex); //east
			glBegin (GL_QUADS);
			glNormal3f (1, 0, 0);
			glTexCoord2f (0.0f, 1.0f);
			glVertex3f (x+xadd, y+yadd, z+zadd);
			glTexCoord2f (0.0f, 0.0f);
			glVertex3f (x+xadd, y+yadd, z+zadd+xadd);
			glTexCoord2f (1.0f, 0.0f);
			glVertex3f (x+xadd, y, z+zadd+xadd);
			glTexCoord2f (1.0f, 1.0f);
			glVertex3f (x+xadd, y, z+zadd);
			glEnd ();
			}

			if (myblk->fg3) {
			glEnable (GL_TEXTURE_2D);
			glBindTexture (GL_TEXTURE_2D, myblk->fg3+mapstexindex); //south
			glBegin (GL_QUADS);
			glNormal3f (0, 1, 0);
			glTexCoord2f (0.0f, 1.0f);
			glVertex3f (x, y+yadd, z+zadd);
			glTexCoord2f (0.0f, 0.0f);
			glVertex3f (x, y+yadd, z+zadd+xadd);
			glTexCoord2f (1.0f, 0.0f);
			glVertex3f (x+xadd, y+yadd, z+zadd+xadd);
			glTexCoord2f (1.0f, 1.0f);
			glVertex3f (x+xadd, y+yadd, z+zadd);
			glEnd ();
			}
			myblk++;
			zadd += xadd;
			}
			if (myblk->fg) {
			tc = maptexcrds;
#ifdef MPYFLIP
			if (myblk->unused2) tc += 16;
			if (myblk->unused3) tc += 32;
#endif
#ifdef MPYROTATE
			if (myblk->user7 < 4) tc += (myblk->user7 * 2);
#endif
			glBindTexture (GL_TEXTURE_2D, myblk->fg+mapstexindex);
			glBegin (GL_QUADS);
			glNormal3f (0, 0, -1);
			glTexCoord2f (tc[0], tc[1]);
			glVertex3f (x, y, z+zadd);
			glTexCoord2f (tc[2], tc[3]);
			glVertex3f (x+xadd, y, z+zadd);
			glTexCoord2f (tc[4], tc[5]);
			glVertex3f (x+xadd, y+yadd, z+zadd);
			glTexCoord2f (tc[6], tc[7]);
			glVertex3f (x, y+yadd, z+zadd);
			glEnd ();
			zadd = 0.0;
			}
			tc = maptexcrds;
#ifdef MPYFLIP
			if (myblk->unused2) tc += 16;
			if (myblk->unused3) tc += 32;
#endif
#ifdef MPYROTATE
			if (myblk->user7 < 4) tc += (myblk->user7 * 2);
#endif
			glBindTexture (GL_TEXTURE_2D, myblk->bg+mapstexindex);
			glBegin (GL_QUADS);
			glNormal3f (0, 0, -1);
			glTexCoord2f (tc[0], tc[1]);
			glVertex3f (x, y, z+zadd);
			glTexCoord2f (tc[2], tc[3]);
			glVertex3f (x+xadd, y, z+zadd);
			glTexCoord2f (tc[4], tc[5]);
			glVertex3f (x+xadd, y+yadd, z+zadd);
			glTexCoord2f (tc[6], tc[7]);
			glVertex3f (x, y+yadd, z+zadd);
			glEnd ();
			x += xadd;
			mymappt++;
			w--;
		}
		tempmappt += mapwidth; mymappt = tempmappt;
		y += yadd;
		h--;
		tid++;
	}
	return 0;
}

void FMPLoader::InitAnims (void)
{
int i;
ANISTR * myanpt;

	if (mapanimseqpt == NULL || mapanimstrpt == NULL) return;

	myanpt = mapanimstrpt;
	for (i=0;i<mapnumanimstr;i++) {
		if (myanpt->antype==AN_PPFR) myanpt->antype = AN_PPFF;
		if (myanpt->antype==AN_PPRF) myanpt->antype = AN_PPRR;
		if (myanpt->antype==AN_ONCES) myanpt->antype = AN_ONCE;
		if ((myanpt->antype==AN_LOOPR) || (myanpt->antype==AN_PPRR))
		{
		myanpt->ancuroff = myanpt->anstartoff;
		if ((myanpt->anstartoff)!=(myanpt->anendoff)) myanpt->ancuroff=(myanpt->anendoff)-1;
		} else {
		myanpt->ancuroff = myanpt->anstartoff;
		}
		myanpt->ancount = myanpt->andelay;

		myanpt++;
	}
}

void FMPLoader::UpdateAnims (void)
{
int i;
ANISTR * myanpt;

	if (mapanimstrpt==NULL) return;
	myanpt = (ANISTR *) mapanimstrpt;
	for (i=0;i<mapnumanimstr;i++)
	{
		if (myanpt->antype!=AN_NONE) { myanpt->ancount--; if (myanpt->ancount<0) {
		myanpt->ancount = myanpt->andelay;
		if (myanpt->antype==AN_LOOPF)
		{
			if (myanpt->anstartoff!=myanpt->anendoff) { myanpt->ancuroff++;
			if (myanpt->ancuroff==myanpt->anendoff) myanpt->ancuroff = myanpt->anstartoff;
		} }
		if (myanpt->antype==AN_LOOPR)
		{
			if (myanpt->anstartoff!=myanpt->anendoff) { myanpt->ancuroff--;
			if (myanpt->ancuroff==((myanpt->anstartoff)-1))
				myanpt->ancuroff = (myanpt->anendoff)-1;
		} }
		if (myanpt->antype==AN_ONCE)
		{
			if (myanpt->anstartoff!=myanpt->anendoff) { myanpt->ancuroff++;
			if (myanpt->ancuroff==myanpt->anendoff) { myanpt->antype = AN_ONCES;
				myanpt->ancuroff = myanpt->anstartoff; }
		} }
		if (myanpt->antype==AN_ONCEH)
		{
			if (myanpt->anstartoff!=myanpt->anendoff) {
			if (myanpt->ancuroff!=((myanpt->anendoff)-1)) myanpt->ancuroff++;
		} }
		if (myanpt->antype==AN_PPFF)
		{
			if (myanpt->anstartoff!=myanpt->anendoff) { myanpt->ancuroff++;
			if (myanpt->ancuroff==myanpt->anendoff) { myanpt->ancuroff -= 2;
			myanpt->antype = AN_PPFR;
			if (myanpt->ancuroff<myanpt->anstartoff) myanpt->ancuroff ++; }
		} } else {
		if (myanpt->antype==AN_PPFR)
		{
			if (myanpt->anstartoff!=myanpt->anendoff) { myanpt->ancuroff--;
			if (myanpt->ancuroff==((myanpt->anstartoff)-4)) { myanpt->ancuroff += 2;
			myanpt->antype = AN_PPFF;
			if (myanpt->ancuroff>myanpt->anendoff) myanpt->ancuroff --; }
		} } }
		if (myanpt->antype==AN_PPRR)
		{
			if (myanpt->anstartoff!=myanpt->anendoff) { myanpt->ancuroff--;
			if (myanpt->ancuroff==((myanpt->anstartoff)-4)) { myanpt->ancuroff += 2;
			myanpt->antype = AN_PPRF;
			if (myanpt->ancuroff>myanpt->anendoff) myanpt->ancuroff --; }
		} } else {
		if (myanpt->antype==AN_PPRF)
		{
			if (myanpt->anstartoff!=myanpt->anendoff) { myanpt->ancuroff++;
			if (myanpt->ancuroff==myanpt->anendoff) { myanpt->ancuroff -= 2;
			myanpt->antype = AN_PPRR;
			if (myanpt->ancuroff<myanpt->anstartoff) myanpt->ancuroff ++; }
		} } }
	} } myanpt++; }
}

FMPLoader::FMPLoader(void)
{
	maperror = MER_NONE;
//	mapislsb;
//	mapwidth, mapheight;
//	mapblockwidth, mapblockheight, mapblockdepth;
//	mapblockstrsize, mapnumblockstr, mapnumblockgfx, mapnumanimstr;
	mapcmappt = NULL;
	mapblockstrpt = NULL;
	mapanimstrpt = NULL;
	mapanimseqpt = NULL;
	mappt = NULL;
	maparraypt = NULL;
	for(int i = 0; i < 8; i++)
	{
		mapmappt[i] = NULL;
		mapmaparraypt[i] = NULL;
	}
	mapstexindex = 0;
	texFormat = GL_RGBA;
}

FMPLoader::~FMPLoader(void)
{
}
