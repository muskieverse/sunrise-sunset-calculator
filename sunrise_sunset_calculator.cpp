#include <math.h>
#include "sunrise_sunset_calculator.h"

// Constants

const	double		PI		= 4.0 * atan(1.0);


// Helper Functions

//
//	Convert the angle from Radians to Degrees
//
double		toDegrees(double radians)
{
	return (radians * (180 / PI));
}

//
//	Convert the angle from Degress to Radians
//
double		toRadians(double degrees)
{
	return (degrees * (PI / 180));
}

//
//	Determine the day number of the year (sometimes called Julian Day).
//
int	dayOfYear(int year, int month, int day)
{
	static	int	month_values[11] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30};

	// If a leap year, adjust February.

	month_values[1]	= (((year % 4) == 0) && (((year % 100) != 0) || ((year % 400) == 0))) ? 29 : 28;

	// Add all days for prior months and then add the days so far this month.

	int	julian = 0;

	for (int i = 0; i < (month - 1); i++)
	{
		julian += month_values[i];
	}

	julian += day;

	return (julian);
}

//
// Calculate the day of the week using Jan 2, 1977 as our reference.
//	We end up with the day of week, 1 = Sunday, 7 = Saturday.
//
int	dayOfWeek(int year, int month, int day)
{
	int	leap_days	= (year - 1977) / 4;
	int	weekday		= (((((year - 1977) * 365) + dayOfYear(year, month, day) + leap_days) - 2) % 7) + 1;
	return(weekday);
}

//
// Determine if the date is in Daylight Savings Time (United States).
//
bool isDST(int day, int month, int dow)
{
	// January, february, and December are NOT within DST.

	if ((month < 3) || (month > 11))
	{
		 return (false);
	}

	// April to October are in DST

	if ((month > 3) && (month < 11))
	{
		 return (true);
	}

	int previous_sunday = day - dow;

	// In march, we are DST if our previous sunday was on or after the 8th.

	if (month == 3)
	{
		 return (previous_sunday >= 8);
	}

	// In november we must be before the first sunday to be dst.
	// That means the previous sunday must be before the 1st.

	return (previous_sunday <= 0);
}



// Member Functions

SunriseSunsetCalculator::SunriseSunsetCalculator()
	: month_(1)
	, day_(1)
	, year_(2015)
	, latitude_(39.9611)
	, longitude_(82.9989)
	, timezone_(-5)						// EST
	, adjust_for_dst_(true)
{
}

//
// Calculate the approximate time of Sunrise for our given properties.
//
void	SunriseSunsetCalculator::computeSunrise(int &sunrise_hour, int &sunrise_minute)
{
	double	sunrise_val	= localNoon() - hoursAroundLocalNoon( dayOfYear(year_, month_, day_) );

	sunrise_hour	= (int)sunrise_val;
	sunrise_minute	= (int)((sunrise_val - sunrise_hour) * 60);

	if (adjust_for_dst_ && (isDST(day_, month_, dayOfWeek(year_, month_, day_))))
	{
		 sunrise_hour   += 1;
	}
}

//
// Calculate the approximate time of Sunset for our given properties.
//
void	SunriseSunsetCalculator::computeSunset(int &sunset_hour, int &sunset_minute)
{
	double	sunset_val	= localNoon() + hoursAroundLocalNoon( dayOfYear(year_, month_, day_) );

	sunset_hour		= (int)sunset_val;
	sunset_minute	= (int)((sunset_val - sunset_hour) * 60);

	if (adjust_for_dst_ && (isDST(day_, month_, dayOfWeek(year_, month_, day_))))
	{
		 sunset_hour	+= 1;
	}
}

//
// Calculate the Equation of Time for the Julian date D
// http://en.wikipedia.org/wiki/Equation_of_time#Alternative_calculation
//
double  SunriseSunsetCalculator::equationOfTime(int D)
{
	double  W	= 360 / 365.24;
	double  A	= W * (D + 10);
	double  B	= A + 1.914  * sin(toRadians(W * (D - 2)));
	double  C	= (A - toDegrees( atan( tan( toRadians(B) ) / cos( toRadians(23.44) ) ) )) / 180;
	int     iC	= (int)(C + 0.5);
	double  EoT	= 720 * (C - iC);
	return (EoT);
}

//
// Calculate the hours around local noon (rise and set) for the Julian date D
// H = | (1/15)*arccos[-tan(L)*tan(23.44*sin(360(D+284)/365))] |.
// http://www.had2know.com/society/sunrise-sunset-time-calculator-formula.html
//
double  SunriseSunsetCalculator::hoursAroundLocalNoon(int D)
{
	double  Dec	= sin( toRadians((360.0 * (D + 284.0)) / 365.0) );
	double  H	= fabs( (1.0 / 15.0) * toDegrees( acos( -tan( toRadians(latitude_) ) * tan( toRadians(23.44 * Dec) ) )));
	return (fabs(H));
}

//
// Compute the time of Local Noon for our Longitute and Timezone
//	Requires timezone to be positive for western hemisphere (opposite to our property.)
//
double  SunriseSunsetCalculator::localNoon()
{
	double  standard_meridian				= (-timezone_) * 15;
	double  longitude_deviation_time		= (longitude_ - standard_meridian) / 15;
	double  correction						= longitude_deviation_time - ( equationOfTime( dayOfYear(year_, month_, day_) ) / 60.0 );
	return (12.0 + correction);
}

