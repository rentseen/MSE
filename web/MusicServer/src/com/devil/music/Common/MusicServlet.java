package com.devil.music.Common;

import javax.servlet.ServletContextEvent;
import javax.servlet.ServletContextListener;

/**
 * 
 *  
 * User: Devil
 * Date: Apr 7, 2017 1:20:36 PM
 */

public class MusicServlet implements ServletContextListener{

	@Override
	public void contextDestroyed(ServletContextEvent arg0) {
		// TODO Auto-generated method stub
		MongoSessionFactory.getInstance().close();
		//System.out.println("close database connection.");
	}

	@Override
	public void contextInitialized(ServletContextEvent arg0) {
		// TODO Auto-generated method stub
		MongoSessionFactory.getInstance();
		//System.out.println("Connect to database success.");
	}

}
