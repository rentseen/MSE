package com.devil.music.Common;

import com.mongodb.MongoClient;
import com.mongodb.client.MongoDatabase;

/**
 * 
 * 
 * User: Devil Date: Apr 7, 2017 1:22:25 PM
 */

public class MongoSessionFactory {
	public static MongoSessionFactory theInstance;
	public MongoClient mongoClient;
	public MongoDatabase mongoDatabase;
	
	public static MongoSessionFactory getInstance() {
		if (theInstance == null) {
			theInstance = new MongoSessionFactory();
		}
		return theInstance;
	}
	
	public MongoSessionFactory(){
		// Default connection.
		mongoClient = new MongoClient(AppSettings.DATABASE_URL, AppSettings.DATABASE_PORT);
		mongoDatabase = mongoClient.getDatabase(AppSettings.DATABASE_NAME);
	}
	
	public void setDatabase(String db_name){
		mongoDatabase = mongoClient.getDatabase(db_name);
	}
	
	public MongoDatabase getDatabase(){
		return mongoDatabase;
	}
	
	public void reconnect(){
		mongoClient.close();
		mongoClient = new MongoClient(AppSettings.DATABASE_URL, AppSettings.DATABASE_PORT);
		mongoDatabase = mongoClient.getDatabase(AppSettings.DATABASE_NAME);
	}
	
	public void close(){
		mongoClient.close();
	}

}
