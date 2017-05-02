package com.devil.music.Web.Action;

import static com.mongodb.client.model.Filters.*;

import org.bson.Document;

/**
 * User register, login and so on.
 * 
 * User: Devil Date: Apr 18, 2017 4:47:00 PM
 */

public class LoginAction extends BasicAction {
	private String username;
	private String password;

	public String login() {
		boolean result = getCollection("user").find(and(eq("username", username), eq("password", password))).iterator()
				.hasNext();
		getSession().put("username", username);
		return result ? SUCCESS : FAIL;
	}
	
	public String register(){
		boolean result = getCollection("user").find(eq("username",username)).iterator().hasNext();
		if(!result){
			Document user = new Document("username", username);
			user.append("password", password);
			getCollection("user").insertOne(user);
			getSession().put("username", username);
		}
		return result ? FAIL : SUCCESS;
	}

	public String getUsername() {
		return username;
	}

	public void setUsername(String username) {
		this.username = username;
	}

	public String getPassword() {
		return password;
	}

	public void setPassword(String password) {
		this.password = password;
	}

}
