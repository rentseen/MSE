package com.devil.music.Common;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * Get the date from string or format string from date.
 * 
 * User: Devil Date: Apr 20, 2017 7:37:38 PM
 */

public class DateUtility {
	public static final SimpleDateFormat NORMAL_TIME = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
	public static final SimpleDateFormat NORMAL_DATE = new SimpleDateFormat("yyyy/MM/dd");

	public static Date getDateFromText(String text) {
		Date result = null;

		try {
			result = NORMAL_TIME.parse(text);
		} catch (ParseException e) {
			e.printStackTrace();
		}

		return result;
	}

	public static Date getDateFromText(String text, SimpleDateFormat sdf) {
		Date result = null;

		try {
			result = sdf.parse(text);
		} catch (ParseException e) {
			e.printStackTrace();
		}

		return result;
	}

	public static String getTextFromTime(Date date) {
		return (date == null ? "" : NORMAL_TIME.format(date));
	}

	public static String getTextFromDate(Date date) {
		return (date == null ? "" : NORMAL_DATE.format(date));
	}

}
