#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <iostream.h>

void main()
{
   /* request auto detection */
   int gdriver = DETECT, gmode, errorcode;

   int x, y, size = 5, colour = 15;
   char ch;

   randomize();

   //Ripped from Turbo C++ help
   /* initialize graphics and local variables */
   initgraph(&gdriver, &gmode, "c:\\tc\\bgi");

   /* read result of initialization */
   errorcode = graphresult();
   /* an error occurred */
   if (errorcode != grOk)
   {
      printf("Graphics error: %s\n", grapherrormsg(errorcode));
      printf("Press any key to halt:");
      getch();
      exit(1);
   }
   //End rip

   //Go to the middle of the screen
   x = (getmaxx() / 2);
   y = (getmaxy() / 2);

   moveto (x, y);

   //Loop forever
   while (1)
   {
      gotoxy (10, 25);
      cout << "(Q)uit     (C)lear screen     (+-) Size:        ([]) Colour:   ";
      gotoxy (51, 25);
      cout << size;
      gotoxy (71, 25);
      cout << colour;

      ch = getch();

      switch (ch)
      {
	//Change colour
	case '[': if (colour > 0)
		  {colour--; setcolor(colour); break;}
		  else break;
	case ']': if (colour < 15)
		  {colour++; setcolor(colour); break;}
		  else break;

	//Change distance moved by one keypress
	case '+': if (size <= 74)
		  {size++; break;}
		  else break;
	case '-': if (size > 1)
		  {size--; break;}
		  else break;

	//Exit when "q" pressed
	case 'q': closegraph(); exit(0);

	//Clear the screen when "c" pressed
	case 'c': initgraph(&gdriver, &gmode, "c:\\tc\\bgi"); moveto (x, y); setcolor(colour); break;

	//Movement
	case 'H': if ((y - size) < 1)
		  y = 0;
		  else
		  y -= size;
		  break;
	case 'P': if ((y + size) >= 350)
		  y = 350;
		  else
		  y += size;
		  break;
	case 'M': if ((x + size) >= 639)
		  x = 639;
		  else
		  x += size;
		  break;
	case 'K': if ((x - size) < 1)
		  x = 0;
		  else
		  x -= size;
		  break;
      }
      lineto (x, y);
   }
}