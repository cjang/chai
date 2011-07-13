#include <iostream>
#include <math.h>
#include <peakstream.h>

using namespace chai;
using namespace std;

//
// Sample code from a presentation given by Matthew Papakipos at Stanford in
// 2007 (page 24 of "Public Google PeakStream.ppt")
//
// http://www.stanford.edu/class/ee380/Abstracts/070926-PeakStream.pdf
//

int main(int argc, char *argv[])
{
    init();

    const int pixels_x = 10, pixels_y = 10;
    const float phi = 1, zoom = 1, cx = 1, cy = 1;
    const int max_iter = 100;

    { // force array destructors before shutdown

    Arrayf32 Iter = -1;
    Arrayf32 XP = (Arrayf32::index(0, pixels_x, pixels_y) - pixels_x / 2);
    Arrayf32 YP = (Arrayf32::index(1, pixels_x, pixels_y) - pixels_y / 2);
    Arrayf32 Xprime = XP * cos(phi) - YP * sin(phi);
    Arrayf32 Yprime = YP * cos(phi) - XP * sin(phi);
    XP = Xprime / (pixels_x * zoom) + cx;
    YP = Yprime / (pixels_x * zoom) + cy;
    Arrayf32 X = 0., Y = 0., Y2 = 0.; // not in original code
    for (int iteration = 0; iteration < max_iter; iteration++)
    {
        // Iterate
        /*Arrayf32*/ Y = 2 * X * Y + YP;
        /*Arrayf32*/ X = X * X - Y2 + XP;
        /*Arrayf32*/ Y2 = Y * Y;

        // Test for escape condition
        Arrayf32 Eval = cond(X * X + Y2 < 4, 0, 1);
        Iter = cond(Iter < 0 && Eval > 0, iteration, Iter);
    }

    } // force array destructors before shutdown

    shutdown();

    return 0;
}
