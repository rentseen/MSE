package com.devil.music.Web.Action;

import static com.mongodb.client.model.Filters.*;
import static com.mongodb.client.model.Sorts.*;
import static com.mongodb.client.model.Updates.*;

import java.util.Date;

import org.bson.Document;
import org.json.JSONArray;
import org.json.JSONObject;

import com.devil.music.Common.AppSettings;
import com.devil.music.Common.DateUtility;
import com.mongodb.client.MongoCursor;

/**
 * Search songs, singers, lyrics, user listen history.
 * 
 * User: Devil Date: Apr 19, 2017 1:33:26 PM
 */

public class SearchAction extends BasicAction {
	private String searchContent;
	private int searchType;
	private int songId;
	private int singerId;

	public String search() {
		JSONArray list = new JSONArray();
		String result = "";
		MongoCursor<Document> iterator = null;
		switch (searchType) {
		case TYPE_SONG: {
			iterator = getCollection("song").find(regex("title", searchContent, "i")).sort(descending("collect_count"))
					.iterator();
			break;
		}
		case TYPE_SINGER: {
			iterator = getCollection("song").find(regex("singer_info.artistName", searchContent, "i"))
					.sort(descending("collect_count")).iterator();

			break;
		}
		case TYPE_LYRIC: {
			iterator = getCollection("song").find(regex("lyrics", searchContent, "i")).sort(descending("collect_count"))
					.iterator();

			break;
		}
		default: {
			System.out.println("Error: unknown search type.");
			break;
		}
		}
		while (iterator.hasNext()) {
			Document doc = iterator.next();
			JSONObject object = new JSONObject(doc.toJson());
			list.put(object);
		}
		result = list.toString();
		//System.out.println(result);
		sendResponse(result);
		return null;
	}

	public String getUserInfo() {
		if(getSession().containsKey("username")){
			String username = (String) getSession().get("username");
			MongoCursor<Document> iterator = getCollection("history").find(eq("username",username)).sort(descending("listenNum")).iterator();
			JSONArray list = new JSONArray();
			while(iterator.hasNext()){
				Document history = iterator.next();
				int song_id = history.getInteger("song_id");
				Document song = getCollection("song").find(eq("song_id",song_id)).first();
				JSONObject song_obj = new JSONObject(song.toJson());
				song_obj.put("lastModified", DateUtility.getTextFromTime(history.getDate("lastModified")));
				song_obj.put("clickNum", history.getInteger("clickNum"));
				song_obj.put("listenNum", history.getInteger("listenNum"));
				list.put(song_obj);
			}
			String result = list.toString();
			//System.out.println(result);
			sendResponse(result);
		}
		return null;
	}
	
	// listen once, all of clickNum,listenNum,collect_count +=1;
	public String saveListenHistory(){
		if(getSession().containsKey("username")){	
			String username = (String) getSession().get("username");
			getCollection("song").findOneAndUpdate(eq("song_id",songId), inc("collect_count",1));
			Document doc = getCollection("history").findOneAndUpdate(and(eq("username",username),eq("song_id",songId)), combine(currentDate("lastModified"),inc("listenNum",1),inc("clickNum",1)));	
			// first listen this song.
			if(doc == null){
				Document history = new Document("username",username).append("song_id", songId);
				history.append("lastModified", new Date());
				history.append("clickNum", 1);
				history.append("listenNum", 1);
				getCollection("history").insertOne(history);
			}
		}
		return null;
	}
	
	// click once, only clickNum +=1;
	public String saveClickHistory(){
		if(getSession().containsKey("username")){
			String username = (String) getSession().get("username");
			Document doc = getCollection("history").findOneAndUpdate(and(eq("username",username),eq("song_id",songId)), inc("clickNum",1));
			// first click this song.
			if(doc == null){
				Document history = new Document("username",username).append("song_id", songId);
				history.append("lastModified", new Date());
				history.append("clickNum", 1);
				history.append("listenNum", 0);
				getCollection("history").insertOne(history);
			}
		}
		return null;
	}
	
	public String showSongDetail(){
		saveClickHistory();
		Document song = getCollection("song").find(eq("song_id",songId)).first();
		JSONObject song_obj = new JSONObject(song.toJson());
		sendResponse(song_obj.toString());
		return null;
	}
	
	public String showSingerDetail(){
		Document singer = getCollection("singer").find(eq("artistId",singerId)).first();
		JSONObject singer_obj = new JSONObject(singer.toJson());
		
		// cut and regenerate singer image url.
		// xiami image file has five types: 
		// no image; end with .xxx; end with _1.xxx; end with _1_1.xxx; end with _2_1.xxx;
		// xxx is jpg/png/ or other types.
		// _1/_1_1/_2_1.xxx are all small images, so change all of them to .xxx;
		String artistLogo = singer_obj.getString("artistLogo");
		String img_url = "";
		if (artistLogo.equals("")){
			img_url = AppSettings.DEFAULT_IMG_URL;
		}
		else{
			img_url = artistLogo.replace("_1.", ".");
			img_url = img_url.replace("_1.", ".");
			img_url = img_url.replace("_2.", ".");
			img_url = AppSettings.XIAMI_URL + img_url;
		}
		singer_obj.put("img_url", img_url);
		System.out.println(artistLogo);
		System.out.println(img_url);
		
		// join song list of the given singer.
		JSONArray list = new JSONArray();
		MongoCursor<Document> iterator = getCollection("song").find(eq("singer_ids", singerId))
				.sort(descending("collect_count")).iterator();
		while (iterator.hasNext()) {
			Document doc = iterator.next();
			JSONObject object = new JSONObject(doc.toJson());
			list.put(object);
		}
		singer_obj.put("song_list", list);
		
		sendResponse(singer_obj.toString());
		return null;
	}

	public String getSearchContent() {
		return searchContent;
	}

	public void setSearchContent(String searchContent) {
		this.searchContent = searchContent;
	}

	public int getSearchType() {
		return searchType;
	}

	public void setSearchType(int searchType) {
		this.searchType = searchType;
	}

	public int getSongId() {
		return songId;
	}

	public void setSongId(int songId) {
		this.songId = songId;
	}

	public int getSingerId() {
		return singerId;
	}

	public void setSingerId(int singerId) {
		this.singerId = singerId;
	}

}
