#ifndef SUNRISE_SUNSET_CALCULATOR_H
#define SUNRISE_SUNSET_CALCULATOR_H

class		SunriseSunsetCalculator
{
	public:

		SunriseSunsetCalculator();

		void			computeSunrise(int &hour, int &minute);
		void			computeSunset(int &hour, int &minute);

		// Properties

		void			Month(int v)			{ month_ = v; }			// 1 - 12
		void			Day(int v)				{ day_ = v; }				// 1 - 31
		void			Year(int v)				{ year_ = v; }				// YYYY
		void			Latitude(double v)	{ latitude_	= v; }		// Positive for North, Decimal Notation: degrees + (minutes/60) + (seconds/3600)
		void			Longitude(double v)	{ longitude_ = v; }		// Positive for West,  Decimal Notation: degrees + (minutes/60) + (seconds/3600)
		void			Timezone(int v)		{ timezone_ = v; }		// Standard Time. Negative for Western Hemisphere
		void			AdjustForDST(bool v)	{ adjust_for_dst_ = v; }

	private:

		double		equationOfTime(int D);
		double		hoursAroundLocalNoon(int D);
		double		localNoon();

		// Properties

		int			month_;
		int			day_;
		int			year_;
		double		latitude_;
		double		longitude_;
		int			timezone_;
		bool			adjust_for_dst_;
};

#endif
