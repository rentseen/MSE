package com.devil.music.Web.Action;

import com.devil.music.Common.AppSettings;
import com.devil.music.Web.Service.TestService;

import com.mongodb.client.model.IndexOptions;
import com.mongodb.client.model.Indexes;
import com.mongodb.client.model.Updates;
import com.mongodb.client.MongoCollection;

import org.bson.Document;
import org.json.JSONArray;
import org.json.JSONObject;

import java.util.Arrays;
import static com.mongodb.client.model.Filters.*;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.ArrayList;

/**
 * Test, save data and create database.
 * 
 * User: Devil Date: Apr 6, 2017 6:59:35 PM
 */

public class TestAction extends BasicAction {
	private TestService testService;

	public String test() {
		System.out.println("test_start");
		// saveInfo();
		// createDatabase();
		/*
		MongoCursor<Document> iterator = getCollection("singer").find().iterator();
		int total = 0;
		int t1 = 0;
		int t2 = 0;
		while (iterator.hasNext()) {
			total ++;
			Document doc = iterator.next();
			String url = doc.getString("artistLogo");
			int first_line = url.indexOf("_");
			int first_line1 = url.indexOf("_1.jpg");
			if(first_line1 == -1){
				System.out.println(url);
				t1++;
			}
			if(first_line!=first_line1){
				System.out.println(url);
				t2++;
			}
		}
		System.out.println(total);
		System.out.println(t1);
		System.out.println(t2);
		*/
		System.out.println("test_end");
		return null;
	}

	@SuppressWarnings("unchecked")
	void createDatabase() {

		boolean recreate = false;
		if (recreate) {
			getCollection("song").drop();
			getCollection("singer").drop();
			getCollection("album").drop();
			getDatabase().createCollection("song");
			getDatabase().createCollection("singer");
			getDatabase().createCollection("album");
		}

		MongoCollection<Document> col_song = getCollection("song");
		MongoCollection<Document> col_singer = getCollection("singer");
		MongoCollection<Document> col_album = getCollection("album");

		IndexOptions indexOptions = new IndexOptions().unique(true);
		col_song.createIndex(Indexes.ascending("song_id"), indexOptions);
		col_singer.createIndex(Indexes.ascending("artistId"), indexOptions);
		col_album.createIndex(Indexes.ascending("album_id"), indexOptions);

		File file = new File(AppSettings.DATA_PATH + "songID.txt");
		if (!file.exists()) {
			System.out.println("Error: can't find songID.txt.");
		}

		try {
			FileInputStream in = new FileInputStream(file);
			BufferedReader bf = new BufferedReader(new InputStreamReader(in, "UTF-8"));

			String line = null;
			int singer_nums = 0;
			int song_nums = 0;
			int traversal_nums = 0;

			while (true) {
				line = bf.readLine();
				if (line == null) {
					break;
				}

				String str_json = readFile(AppSettings.DATA_PATH + line + ".data");

				Document doc_song = new Document();
				doc_song = Document.parse(str_json);
				int song_id = doc_song.getInteger("song_id");
				ArrayList<Document> singer_list = (ArrayList<Document>) doc_song.get("singer_info");
				
				for (Document singer : singer_list) {
					int singer_id = singer.getInteger("artistId");
					// if this singer already insert, add song_id to list
					// song_ids.
					Document isFirst = col_singer.findOneAndUpdate(eq("artistId", singer_id),
							Updates.addToSet("song_ids", song_id));
					// if not, insert this singer.
					if (isFirst == null) {
						singer.append("song_ids", Arrays.asList(song_id));
						col_singer.insertOne(singer);
						singer_nums++;
					}
					traversal_nums++;
				}
				col_song.insertOne(doc_song);
				song_nums++;
			}
			bf.close();
			System.out.println("Create database success!");
			System.out.println("Number of songs insert: " + song_nums);
			System.out.println("Number of singers insert: " + singer_nums);
			System.out.println("Number of traversal: " + traversal_nums);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	void saveInfo() {
		File file = new File(AppSettings.DATA_PATH + "songID.txt");
		if (!file.exists()) {
			System.out.println("Error: can't find songID.txt.");
		}
		try {
			FileInputStream in = new FileInputStream(file);
			BufferedReader bf = new BufferedReader(new InputStreamReader(in, "UTF-8"));
			String line = null;

			while (true) {
				line = bf.readLine();
				if (line == null) {
					break;
				}
				String str_json = readFile(AppSettings.DATA_PATH + line + ".json");
				String str_meta = readFile(AppSettings.DATA_PATH + line + ".meta");
				JSONObject json_obj = new JSONObject(str_json);
				JSONObject j1 = json_obj.getJSONObject("data");
				JSONArray j2 = j1.getJSONArray("trackList");
				JSONObject j3 = j2.getJSONObject(0);
				int album_id = j3.getInt("album_id");
				String album_name = j3.getString("album_name");
				String album_logo = j3.getString("album_logo");
				String album_pic = j3.getString("album_pic");
				int collect_count = j3.getInt("collectCount");
				JSONArray singer_list = j3.getJSONArray("singersSource");
				if (singer_list.length() > 1) {
					System.out.println(j3.getInt("song_id"));
				}
				for (int i = 0; i < singer_list.length(); i++) {
					singer_list.getJSONObject(i).remove("isMusician");
					singer_list.getJSONObject(i).remove("isShow");
				}
				JSONObject meta_obj = new JSONObject(str_meta);
				meta_obj.put("album_id", album_id);
				meta_obj.put("album_name", album_name);
				meta_obj.put("album_logo", album_logo);
				meta_obj.put("album_pic", album_pic);
				meta_obj.put("collect_count", collect_count);
				int song_id = Integer.parseInt(meta_obj.getString("id"));
				meta_obj.put("song_id", song_id);
				meta_obj.remove("id");
				meta_obj.put("singer_info", singer_list);
				meta_obj.put("singer_ids", j3.getJSONArray("singerIds"));
				meta_obj.remove("singer");
				FileOutputStream fos = new FileOutputStream(AppSettings.DATA_PATH + line + ".data");
				OutputStreamWriter osw = new OutputStreamWriter(fos, "UTF-8");
				osw.write(meta_obj.toString());
				osw.flush();
				osw.close();
			}
			bf.close();
		} catch (Exception e) {
			e.printStackTrace();
		}

	}

	public TestService getTestService() {
		return testService;
	}

	public void setTestService(TestService testService) {
		this.testService = testService;
	}

}
