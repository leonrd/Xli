//
// Copyright (C) 2010-2014 Outracks Technologies
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial
// portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#include <Xli/DateTime.h>
#include <Xli/Exception.h>
#include <Xli/Time.h>
#include <cstdio>

#ifndef XLI_COMPILER_MSVC
# define sscanf_s sscanf
#endif

namespace Xli
{
    bool DateTime::IsLeapYear(int year)
    {
        return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
    }

    int DateTime::GetDaysInMonth(int month, int year)
    {
        switch (month)
        {
        case 2:
            return IsLeapYear(year) ? 29 : 28;

        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            return 31;

        case 4:
        case 6:
        case 9:
        case 11:
            return 30;

        default:
            XLI_THROW("Invalid month");
        };
    }

    int DateTime::GetDaysInYear(int year)
    {
        return IsLeapYear(year)? 366: 365;
    }

    DateTime::DateTime()
    {
        Year = 1601;
        Month = 1;
        Day = 1;
        Hour = 0;
        Minute = 0;
        Second = 0;
        Millisecond = 0;
    }

    DateTime::DateTime(Timestamp timestamp)
    {
        // Calculate year
        Year = 1601;

        while (true)
        {
            UInt64 YearLength = GetDaysInYear(Year) * PerDay;

            if (timestamp >= YearLength)
            {
                timestamp  -= YearLength;
                Year++;
            }
            else
            {
                break;
            }
        }

        // Calculate month
        Month = 1;

        while (true)
        {
            UInt64 MonthLength = GetDaysInMonth(Month, Year) * PerDay;

            if (timestamp >= MonthLength)
            {
                timestamp -= MonthLength;
                Month++;
            }
            else
            {
                break;
            }
        }

        Day = (int)(timestamp / PerDay);
        timestamp -= Day * PerDay;
        Day++;

        Hour = (int)(timestamp / PerHour);
        timestamp -= Hour * PerHour;

        Minute = (int)(timestamp / PerMinute);
        timestamp -= Minute * PerMinute;

        Second = (int)(timestamp / PerSecond);
        timestamp -= Second * PerSecond;

        Millisecond = (int)(timestamp / PerMillisecond);
    }

    Timestamp DateTime::ToTimestamp() const
    {
        Timestamp timestamp = 0;

        int year = Year;
        while (year > 1601)
        {
            timestamp += GetDaysInYear(year-1) * PerDay;
            year--;
        }

        int month = Month;
        while (month > 1)
        {
            timestamp += GetDaysInMonth(month-1, year) * PerDay;
            month--;
        }

        timestamp += (Day-1) * PerDay;
        timestamp += Hour * PerHour;
        timestamp += Minute * PerMinute;
        timestamp += Second * PerSecond;
        timestamp += Millisecond * PerMillisecond;
        return timestamp;
    }

    String DateTime::ToString() const
    {
        return String::Format("%04d-%02d-%02d %02d:%02d:%02d.%03d", Year, Month, Day, Hour, Minute, Second, Millisecond);
    }

    DateTime DateTime::FromString(const String& str)
    {
        DateTime dt;
        sscanf_s(str.Ptr(), "%04d-%02d-%02d %02d:%02d:%02d.%03d", &dt.Year, &dt.Month, &dt.Day, &dt.Hour, &dt.Minute, &dt.Second, &dt.Millisecond);
        return dt;
    }

    DateTime DateTime::Now()
    {
        return DateTime(GetTimestamp());
    }

    DateTime DateTime::NowUtc()
    {
        return DateTime(GetTimestampUtc());
    }

    DateTime DateTime::operator - (const DateTime& dt) const
    {
        DateTime r;
        r.Year = Year - dt.Year;
        r.Month = Month - dt.Month;
        r.Day = Day - dt.Day;
        r.Hour = Hour - dt.Hour;
        r.Minute = Minute - dt.Minute;
        r.Second = Second - dt.Second;
        r.Millisecond = Millisecond - dt.Millisecond;

        // TODO

        return r;
    }

    DateTime DateTime::operator + (const DateTime& dt) const
    {
        DateTime r;
        r.Year = Year + dt.Year;
        r.Month = Month + dt.Month;
        r.Day = Day + dt.Day;
        r.Hour = Hour + dt.Hour;
        r.Minute = Minute + dt.Minute;
        r.Second = Second + dt.Second;
        r.Millisecond = Millisecond + dt.Millisecond;

        while (dt.Millisecond >= 1000)
        {
            r.Second++;
            r.Millisecond -= 1000;
        }

        while (dt.Second >= 60)
        {
            r.Minute++;
            r.Second -= 60;
        }

        while (dt.Minute >= 60)
        {
            r.Hour++;
            r.Minute -= 60;
        }

        while (dt.Hour >= 24)
        {
            r.Day++;
            r.Hour -= 24;
        }

        if (dt.Month >= 1 && dt.Month <= 12 && dt.Year > 0)
        {
            while (dt.Day > DateTime::GetDaysInMonth(dt.Month, DateTime::IsLeapYear(dt.Year)))
            {
                r.Day -= DateTime::GetDaysInMonth(dt.Month, DateTime::IsLeapYear(dt.Year));
                r.Month++;

                if (dt.Month == 13)
                {
                    r.Month = 1;
                    r.Year++;
                }
            }
        }

        return r;
    }
        
    DateTime& DateTime::operator = (const DateTime& dt)
    {
        Year = dt.Year;
        Month = dt.Month;
        Day = dt.Day;
        Hour = dt.Hour;
        Minute = dt.Minute;
        Second = dt.Second;
        Millisecond = dt.Millisecond;
        return *this;
    }
}
