#include "Location.h"

double Location::distance(const Location &P)
{
 double dx,dy;
 dx = x - P.x;
 dy = y - P.y;
 return 0 ;
}
Location Location::milieu(const Location &P)
{
 Location M;
 M.x = (P.x+x) /2;
 M.y = (P.y+y) /2;
 return M;
} 