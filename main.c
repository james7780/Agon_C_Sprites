/*
 * Title:			Space Critters for Agon Light
 * Author:			James Higgs (Jum Hig)
 * Created:			2023
 * Last Updated:		2023-04-21
 *
 * Revisions:
 * 2023-04-21 - Updated to use MOS and VDP 1.03
 */
 
#include <stdio.h>
#include <ctype.h>
#include "mos-interface.h"
#include "vdp.h"

#include "spritedata.h"

#define BITMAP_WIDTH	16
#define BITMAP_HEIGHT	16

#define NUM_SPRITES 50

extern int rand();

typedef struct //SPRITEPOS_t
{
	UINT16 x;
	UINT16 y;
	INT16 dx;
	INT16 dy;
} SPRITEPOS;

SPRITEPOS sp[NUM_SPRITES];

// Get a random number up to R
int randR(int R)
{
	return (rand() % R);
}

// Initialise the sprite control blocks
void setupSprites()
{
	UINT8 i;
	for (i = 0; i < NUM_SPRITES; i++)
		{
		vdp_spriteSelect(i);
		vdp_spriteClearFramesSelected();
		vdp_spriteAddFrameSelected(2);			// UFO frame 1
		vdp_spriteAddFrameSelected(3);			// UFO frame 2
		vdp_spriteAddFrameSelected(4);			// UFO frame 3
		vdp_spriteSetFrameSelected(0);
		vdp_spriteShowSelected();
			
		sp[i].x = randR(300);
		sp[i].y = randR(190);
		sp[i].dx = (randR(100) < 50) ? -1 : 1;
		sp[i].dy = (randR(100) < 50) ? -1 : 1;
		}
}

// Wait a specific number of frames (vertical syncs)
void waitFrames(int numFrames)
{
	int i, j;
	for (i = 0; i < numFrames; i++)
		{
		waitvblank();
		// wait for a bit till we are out of vblank
		for (j= 0; j < 10000; j++) { }
		}
}

/// @param[in] argc			Argument count
/// @param[in] argv			Pointer to the argument string - zero terminated, parameters separated by spaces
int main(int argc, char * argv[]) {
	int	i, j;
	UINT8 k, flags;
	UINT16 t;
	
	vdp_mode(2);
	vdp_cursorDisable();
	vdp_cls();

	printf("C Sprites Demo\n\r");
	
	//k = vdp_asciiCodeAt(1, 0);
	//printf("Ascii code at 1,0 = %d\n\r", k);
	//getch();
	
	// Upload sprite data to the VDP
	printf("Uploading bitmap data...\n\r");
	vdp_bitmapSendData(0, BITMAP_WIDTH, BITMAP_HEIGHT, playerShipData);
	vdp_bitmapSendData(1, BITMAP_WIDTH, BITMAP_HEIGHT, playerBulletData);
	vdp_bitmapSendData(2, BITMAP_WIDTH, BITMAP_HEIGHT, ufoData[0]);	
	vdp_bitmapSendData(3, BITMAP_WIDTH, BITMAP_HEIGHT, ufoData[1]);
	vdp_bitmapSendData(4, BITMAP_WIDTH, BITMAP_HEIGHT, ufoData[2]);
	vdp_bitmapSendData(5, BITMAP_WIDTH, BITMAP_HEIGHT, ufoBulletData);
	
	for (i = 0; i < 10; i++)
		{
		vdp_bitmapDraw(0, randR(300) , randR(160) + 30);
		}
	vdp_spriteRefresh();
	waitFrames(50);
		
	// Set up sprites
	// Note: BBCBASIC procedure:
	//    1. Select sprite n   (VDU 23, 27, 4, n)
	//    2. Clear frames for current sprite  (VDU 23, 27, 5)
	//    3. Add bitmap 0 as frame 0 of current sprite (VDU 23, 27, 6, 0)
	//    4. Show the current sprite (VDU 23, 27, 11)
	//    5. Set total number of sprites to display (VDU 23, 27, 7, total)
	//   Move the sprites:
	//    1. Select sprite n (VDU 23, 27, 4, n)
	//    2. Move selected sprite to  (VDU 23, 27, 13, X, Y) 
	//   Refresh the sprites
	//    1. *FX19    (wait for VSYNC)
	//    2. Refresh (VDU 23, 27, 15)
	
	printf("Setting up sprites...\n\r");
	setupSprites();		// sets up 16 sprites
	
	vdp_spriteActivateTotal(NUM_SPRITES);
	
	// Move the sprites to their initial positions
	for (i = 0; i < NUM_SPRITES; i++)
		{
		vdp_spriteMoveTo(i, sp[i].x, sp[i].y);
		}	
	
	vdp_spriteRefresh();

	printf("Animating... Press any key to exit\n\r");
	for (i = 0; i < 1000; i++)
		{
		waitvblank();
		if (i > 400)
			{
			//vdp_scroll(1, 1, 1);
			vdp_plotColour(randR(16));
			vdp_plotPoint(randR(1024), randR(1024));		// plot coords are now "normalised" to a 1024x1024 "virtual screen"?
			}

		for (j = 0; j < NUM_SPRITES; j++)
			{
			sp[j].x += sp[j].dx;
			if (sp[j].x < 2)
				sp[j].dx = 1;
			else if (sp[j].x > 317)
				sp[j].dx = -1;
			
			sp[j].y += sp[j].dy;
			if (sp[j].y < 2)
				sp[j].dy = 1;
			else if (sp[j].y > 253)
				sp[j].dy = -1;
			
			vdp_spriteSelect(j);
			vdp_spriteMoveToSelected(sp[j].x, sp[j].y);
			vdp_spriteNextFrameSelected();
			}
		
		vdp_spriteRefresh();

		// Check for keypress
		k = getsysvar_vkeydown();
		if (k != 0)
			break;
		//printf("%d\n\r", k);
		}
		
	vdp_spriteActivateTotal(0);
	vdp_spriteRefresh();
	vdp_mode(1);
	vdp_cursorEnable();
	
	return 0;
}