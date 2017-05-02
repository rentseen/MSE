package com.devil.music.Web.Action;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.struts2.ServletActionContext;
import org.bson.Document;

import com.devil.music.Common.MongoSessionFactory;
import com.mongodb.Block;
import com.mongodb.client.MongoCollection;
import com.mongodb.client.MongoDatabase;
import com.opensymphony.xwork2.ActionContext;

/**
 * basic abstract action class.
 * 
 * User: Devil Date: Apr 6, 2017 7:06:45 PM
 */

public abstract class BasicAction {

	public static final String SUCCESS = "success";
	public static final String FAIL = "fail";
	
	public static final int TYPE_SONG = 1;
	public static final int TYPE_SINGER = 2;
	public static final int TYPE_LYRIC = 3;

	private HttpServletResponse response = ServletActionContext.getResponse();
	private HttpServletRequest request = ServletActionContext.getRequest();
	private Map<String, Object> session = ActionContext.getContext().getSession();

	public String readFile(String filepath) {
		File file = new File(filepath);
		if (!file.exists()) {
			System.out.println("Error: can't open file:" + filepath);
			return null;
		}
		StringBuffer buffer = new StringBuffer();
		try {
			FileInputStream in = new FileInputStream(file);
			InputStreamReader isr = new InputStreamReader(in, "UTF-8");
			BufferedReader reader = new BufferedReader(isr);
			String line = null;
			while ((line = reader.readLine()) != null) {
				buffer.append(line);
			}
			reader.close();
			isr.close();
			in.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		return buffer.toString();
	}

	public void sendResponse(String data) {
		try {
			response.setContentType("text/html;charset=UTF-8");
			//System.out.println(data);
			response.getWriter().print(data);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public String commonJump(){
		return SUCCESS;
	}

	public Block<Document> printBlock = new Block<Document>() {
		@Override
		public void apply(final Document document) {
			System.out.println(document.toJson());
		}
	};

	public MongoDatabase getDatabase() {
		return MongoSessionFactory.getInstance().getDatabase();
	}

	public MongoCollection<Document> getCollection(String col_name) {
		return getDatabase().getCollection(col_name);
	}

	public HttpServletResponse getResponse() {
		return response;
	}

	public HttpServletRequest getRequest() {
		return request;
	}

	public Map<String, Object> getSession() {
		return session;
	}

}
