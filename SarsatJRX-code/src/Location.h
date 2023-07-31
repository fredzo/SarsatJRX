#ifndef LOCATION_H
#define LOCATION_H
class Location
{
 public:
 double x,y;
 double distance(const Location &P);
 Location milieu(const Location &P);
};
#endif 