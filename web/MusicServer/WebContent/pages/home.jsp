<%@ page language="java" contentType="text/html; charset=UTF-8"
	pageEncoding="UTF-8"%>
<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>Music Search Engine</title>
<link rel="bookmark" type="image/x-icon" href="/MusicServer/img/play.jpg"/>
<link rel="shortcut icon" href="/MusicServer/img/play.jpg">
<link rel="icon" href="/MusicServer/img/play.jpg">
<script src="${pageContext.request.contextPath}/js/jquery.min.js"
	type="text/javascript"></script>
<script src="${pageContext.request.contextPath}/js/jquery-ui.js"
	type="text/javascript"></script>
<link rel="stylesheet"
	href="${pageContext.request.contextPath}/css/bootstrap.min.css">
<link rel="stylesheet"
	href="${pageContext.request.contextPath}/css/jquery-ui.css">

<script>
var username = '${session.username}';
var DATA_URL = "http://202.120.38.28:2333/MSE/data/";
var XIAMI_URL = "http://img.xiami.net/";
</script>

<script type="text/javascript">
// generate singer links of a song.
function getSingerLink(singer_list){
	var str = "";
	for(var i = 0; i < singer_list.length; i++){
		if(i > 0){
			str += "/";
		}
		str += "<a href='javascript:showSingerDetail("+singer_list[i].artistId+");'>"+singer_list[i].artistName+"</a>";
	}
	if(str == ""){
		str = "Unknown";
	}
	return str;
}

// get song file.
function getSongFile(song_id){
	var str = DATA_URL + song_id + ".mp3";
	return str;
}

// generate play link.
function getPlayLink(song, text){
	var str = "<a href='javascript:play("+song.song_id+",&quot;"+song.title+"&quot;,&quot;"+song.album_pic+"&quot;);'>";
	str += text + "</a>";
	return str;
}

// generate download link.
function getDownloadLink(song, text){
	var str = "<a href='"+getSongFile(song.song_id)+"' download='"+song.title+"'>";
	str += text + "</a>";
	return str;
}

// generate songs table.
function getSongTable(type, song_list){
	var str = "";
	if(song_list.length == 0){
		switch(type){
		case "TYPE_SEARCH":
			str += "<tr><th>Sorry that no song is found! </th></tr>";
			break;
		case "TYPE_SINGER":
			str += "<tr><th>Error: there must be something wrong with the database! </th><tr>";
			break;
		case "TYPE_HISTORY":
			str += "<tr><th>Listen History</th></tr>";
			str += "<tr><th>You have not listened any songs! </th></tr>";
			break;
		default:
			alert("Error: unknown type!");
			break;
		}
	}
	else{
		switch(type){
		case "TYPE_SEARCH":
			str += "<tr><th>title</th><th>singer</th><th>album</th><th>rank</th><th>operation</th></tr>";
			break;
		case "TYPE_SINGER":
			str += "<tr><th>title</th><th>singer</th><th>album</th><th>rank</th><th>operation</th><tr>";
			break;
		case "TYPE_HISTORY":
			str += "<tr><th>Listen History</th></tr>";
			str += "<tr><th>title</th><th>singer</th><th>album</th><th>listenNum</th><th>clickNum</th><th>lastListened</th><th>operation</th></tr>";
			break;
		default:
			alert("Error: unknown type!");
			break;
		}
		for (var i = 0; i < song_list.length; i++) {
			str += "<tr>";
			str += "<td><a href='javascript:showSongDetail("+song_list[i].song_id+");'>"+song_list[i].title+"</a></td>";
			str += "<td>"+getSingerLink(song_list[i].singer_info)+"</td>";
			str += "<td>"+song_list[i].album_name+"</td>";
			if(type == "TYPE_HISTORY"){
				str += "<td>"+song_list[i].listenNum+"</td>";
				str += "<td>"+song_list[i].clickNum+"</td>";
				str += "<td>"+song_list[i].lastModified+"</td>";
			}
			else{
				str += "<td>"+song_list[i].collect_count+"</td>";
			}
			str += "<td>" + getPlayLink(song_list[i], "播放") + "&nbsp;&nbsp;";
			str += getDownloadLink(song_list[i], "下载") + "</td>";
			str += "</tr>";
		}
	}
	return str;
}

// control which div to show.
function showDiv(div_name){
	$("#song_list").css("display","none");
	$("#song_detail").css("display","none");
	$("#singer_detail").css("display","none");
	$("#user_detail").css("display","none");
	if(div_name!="none"){
		$("#"+div_name).css("display","block");
	}
}

// control play songs. 
function play(src,title,img_url){
	var audio = $("#audio")[0];
	audio.pause();
	$("#song-name").text(title);
	$("#song-name").attr("href","javascript:showSongDetail("+src+")");
	$("#player-img").attr("src",img_url);
	audio.src = getSongFile(src);
	audio.play();
	// play one song will add listenNum, clickNum, rank of this song++;
	$.post("saveListenHistory.action",{songId:src});
}

// show song detail.
function showSongDetail(song_id){
	var state = {page:"PAGE_SONG",songId:song_id};
	history.pushState(state, "Music Search Engine", "#song/?id="+song_id);
	pageControl(state);
}

// show singer detail.
function showSingerDetail(singer_id){
	var state = {page:"PAGE_SINGER", singerId:singer_id};
	history.pushState(state, "Music Search Engine", "#singer/?id="+singer_id);
	pageControl(state);
}

// show user listen history detail.
function showUserDetail(){
	var state = {page:"PAGE_HISTORY"};
	history.pushState(state, "Music Search Engine", "#history");
	pageControl(state);	
}

// control all pages.
// why we need this? Because ajax don't refresh the page, 
// if we click back/forward button of the browser, it will
// jump to the first page instead of the page we want. So, 
// we have to control pages manually, with the help of h5 
// functions, pushstate/popstate.
function pageControl(state){
	var page = state.page;
	switch (page){
	case "PAGE_HOME":
		showDiv("none");
		break;
	case "PAGE_SEARCH":	
		$.post("search.action",state,function(data){
			var jsonArray = eval("(" + data + ")");
			$("#song_table").html(getSongTable("TYPE_SEARCH", jsonArray));
		});	
		showDiv("song_list");
		break;
	case "PAGE_HISTORY":
		$.post("getUserInfo.action",function(data){
			var jsonArray = eval("(" + data + ")");
			$("#history_table").html(getSongTable("TYPE_HISTORY", jsonArray));
		});
		showDiv("user_detail");
		break;
	case "PAGE_SINGER":
		$.post("showSingerDetail.action",state,function(data){
			var singer_obj = eval("(" + data + ")");
			$("#singer-img").attr("src",singer_obj.img_url);
			$("#singer-name").html(singer_obj.artistName);
			$("#singer-alias").html(singer_obj.alias);
			$("#singer-pinyin").html(singer_obj.pinyin);
			$("#singer-area").html(singer_obj.area);
			$("#singer_table").html(getSongTable("TYPE_SINGER", singer_obj.song_list));
		});
		showDiv("singer_detail");
		break;
	case "PAGE_SONG":
		$.post("showSongDetail.action",state,function(data){
			var song_obj = eval("(" + data + ")");
			$("#detail-img").attr("src",song_obj.album_pic);
			$("#detail-title").html(song_obj.title);
			$("#detail-singer").html(getSingerLink(song_obj.singer_info));
			$("#detail-album").html(song_obj.album_name);
			$("#detail-rank").html(song_obj.collect_count);
			var operations = "";
			operations += getPlayLink(song_obj, "播放")+"&nbsp;&nbsp;"+getDownloadLink(song_obj, "下载");
			$("#detail-operation").html(operations);
			var lyric = song_obj.lyrics;
			var lyric_new = lyric.replace(/\n/g,"<br>");
			$("#detail-lyric").html(lyric_new);
		});
		showDiv("song_detail");
		break;
	}
}

// used for back/forward ajax.
window.addEventListener("popstate", function(event) {	
	pageControl(event.state);
});


$(document).ready(function() {
	
	// used for create database and test.
	// $.post("test.action");
	
	// used for page control.
	history.replaceState({page: "PAGE_HOME"}, "Music Search Engine", "#home");
	
	showDiv("none");
	
	if(username!=""){
		$("#username").text(username);
	}
	else{
		$("#user_info").html("<a href='gotoLogin.action'>登录</a>&nbsp;&nbsp;<a href='gotoRegister.action'>注册</a>");
	}

	$("#button-search").click(function(){
		var keywords = $("#search-input").val();
		var type = $("input:radio[name='input-radio']:checked").val();
		if (keywords==""){
			alert("输入不能为空！");
		}
		else{
			// used for page control.
			var state = {page:"PAGE_SEARCH",searchContent:keywords,searchType:type};
			history.pushState(state, "Music Search Engine", "#search/?s="+keywords+"&t="+type);
			pageControl(state);	
		}
	});
});
</script>
<style type="text/css">
/*
div {
	border: 1px solid #fff;
}
*/

/* css of top divs */
#top_div {
	position: fixed;
	top: 0px;
	width: 100%;
	height: 8em;
	padding: 1em 2em 1em 2em;
	z-index: 100;
	background-color: #fff;
	box-shadow: 0px 3px 3px #888888;
}

/* css of search div */
#search {
	width: 50%;
	float: left;
	height: 6em;
	background: url(/MusicServer/img/logo.jpg) no-repeat;
	background-size: 6em 5.8em;
	padding-left: 9em;
	padding-right: 3em;
	padding-top: 1em;
}

input[type="radio"] {
	width: 1.1em;
	height: 1.1em;
}

#search-radio {
	font-size: 1.3em;
}

#button-search {
	color: white;
	background-color: #1c87e1;
	border-color: #46b8da;
}

/* css of player div */
#player-div {
	width: 30%;
	float: left;
	height: 6em;
	margin-left: 2em;
	/*background:url(http://img.xiami.net/images/album/img73/159/58e4863811766_7967273_1491371576.jpg) no-repeat ;
	background-size:6em 6em;*/
	padding-left: 1em;
}

#player-img {
	width: 6em;
	height: 6em;
}

#player-content {
	position: relative;
	top: -6em;
	left: 7em;
}

#player-title {
	margin-bottom: 0.5em;
	font-size: 1.2em;
}

/* css of current user div */
#user_info {
	width: 10%;
	float: right;
	font-size: 1.2em;
	text-align: right;
}

/* css of content divs */
#content_div {
	position: relative;
	top: 8em;
	left: 5%;
	width: 80%;
}

/* css of song detail div */
#song_detail {
	margin-left: 10%;
	padding-top: 3em;
	padding-left: 3em;
	min-height: 100em;
	border: 2px solid #ddd;
}

#song-img-div {
	float: left;
	width: 20%;
	box-shadow: 3px 3px 3px #888888;
}

#song-detail-div {
	float: left;
	width: 70%;
	margin-left:5%;
}

#detail-table td,#detail-table th{
	border: 1px solid #fff!important;
}

/* css of singer detail div */
#singer_detail {
	margin-left: 10%;
	padding-top: 3em;
	padding-left: 3em;
	padding-right: 3em;
	min-height: 100em;
	border: 2px solid #ddd;
}

#singer-img-div {
	float: left;
	width: 20%;
	box-shadow: 3px 3px 3px #888888;
}

#singer-detail-div {
	float: left;
	width: 70%;
	margin-left:5%;
}

#singer-detail-table td,#singer-detail-table th{
	border: 1px solid #fff!important;
}

#singer-songlist-div{
	position: relative;
	top: 1em;
}
</style>
</head>
<body>
	<div id="top_div">
		<!-- top search div -->
		<div id="search">
			<div class="input-group input-group-lg">
      			<input id="search-input" type="text" class="form-control" placeholder="Search for...">
      			<span class="input-group-btn">
        			<button class="btn" type="button" id="button-search">Search</button>
      			</span>
   			</div>
   			<div id="search-radio">
   			<label class="radio-inline">
  				<input type="radio" name="input-radio"  value="1" checked> &nbsp;song&nbsp;
			</label>
			<label class="radio-inline">
  				<input type="radio" name="input-radio"  value="2"> &nbsp;singer&nbsp;
			</label>
			<label class="radio-inline">
  				<input type="radio" name="input-radio"  value="3"> &nbsp;lyric&nbsp;
			</label>
			</div>
		</div>
		<!-- top player div -->
		<div id="player-div">
			<img id="player-img" src="/MusicServer/img/play.jpg"  class="img-thumbnail">
			<div id="player-content">
				<div id="player-title"> 
					当前播放：<a href="#" id="song-name"></a>
				</div>
				<audio id="audio"  controls="controls" autoplay="autoplay">
				</audio>
			</div>
		</div>
		<!-- top user div -->
		<div id="user_info">
			<a id="username" href="javascript:showUserDetail();"></a><br>
			<a href="javascript:showUserDetail();">查看听歌记录</a><br>
			<a href="gotoLogin.action">退出</a>
		</div>
	</div>
	<div id="content_div">
	
	<!-- show search result -->
	<div id="song_list">
		<table class = "table" id = "song_table">
	
		<!-- used for draw tables, do not delete.
		<tr><th>Sorry that no song is found! </th></tr>
		<tr>
			<th>title</th>
			<th>singer</th>
			<th>album</th>
			<th>rank</th>
			<th>operation</th>
		</tr>
		<tr>
			<td><a href="javascript:void(0);">告白气球</a></td>
			<td><a href="javascript:void(0);">周杰伦</a></td>
			<td><a href="javascript:void(0);">周杰伦的床边故事</a></td>
			<td><a href="javascript:void(0);">111111</a></td>
			<td><a href="javascript:void(0);">播放</a><a href="javascript:void(0);">&nbsp;&nbsp;下载</a></td>
		</tr>
	 	-->
	</table>
	</div>
	
	<!-- show song info -->
	<div id="song_detail">
		<div id="song-img-div">
			<img id="detail-img"
				src="http://img.xiami.net/images/album/img73/159/58e4863811766_7967273_1491371576.jpg"
				class="img-thumbnail">
		</div>
		<div id="song-detail-div">
			<table class = "table" id = "detail-table">
				<tr><th>Title: </th><td id="detail-title">青花瓷</td></tr>
				<tr><th>Singer: </th><td id="detail-singer">周杰伦</td></tr>
				<tr><th>Album: </th><td id="detail-album">xxx</td></tr>
				<tr><th>Rank: </th><td id="detail-rank">1111</td></tr>
				<tr><th>Operation:</th><td id="detail-operation"><a href="#">PLAY</a>&nbsp;<a href="#">DOWNLOAD</a></td></tr>
				<tr><th>lyric: </th><td id="detail-lyric">xxx</td></tr>
			</table>
		</div>
		<br> <br>
	</div>


	<!-- show singer info -->
	<div id="singer_detail">
		<div id="singer-img-div">
			<img id="singer-img"
				src="/MusicServer/img/default.jpg"
				class="img-thumbnail">
		</div>
		<div id="singer-detail-div">
			<table class = "table" id = "singer-detail-table">
				<tr><th>Name: </th><td id="singer-name">周杰伦</td></tr>
				<tr><th>Pinyin: </th><td id="singer-pinyin">zhou jie lun</td></tr>
				<tr><th>Alias: </th><td id="singer-alias">Jay chou</td></tr>
				<tr><th>Area: </th><td id="singer-area">Taiwan</td></tr>
			</table>
		</div>
			<div id="singer-songlist-div">
				<table class="table" id="singer_table">
				<!--  used for draw tables, do not delete.
					<tr>
						<th>title</th>
						<th>singer</th>
						<th>album</th>
						<th>rank</th>
						<th>operation</th>
					</tr>
				-->
				</table>
			</div>
			<br> <br>
	</div>
	
	
	<!-- show user listen history -->
	<div id="user_detail">
		<table class = "table" id = "history_table">
		<!--  used for draw tables, do not delete.
			<tr><th>You have not listened any songs. </th></tr>
			<tr>
				<th>title</th>
				<th>singer</th>
				<th>album</th>
				<th>listenNum</th>
				<th>clickNum</th>
				<th>lastListened</th>
				<th>operation</th>
			</tr>
		-->
		</table>
	</div>
	
	</div>
</body>
</html>