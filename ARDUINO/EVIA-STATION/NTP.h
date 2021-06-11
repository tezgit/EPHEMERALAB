//    For UTC -5.00 : -5 * 60 * 60 : -18000
//    For UTC +1.00 : 1 * 60 * 60 : 3600
//    For UTC +0.00 : 0 * 60 * 60 : 0

#include <NTPClient.h>

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 7200;
const long utcOffsetInSeconds = 7200;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, daylightOffset_sec);
//NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

String formattedDate;
String dayStamp;
String timeStamp;
