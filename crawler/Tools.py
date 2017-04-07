# -*- coding: utf-8 -*-
import urllib
import urllib2
import urlparse
import math
import re
import json

class Tools:
    def __init__(self):
        pass

    @classmethod
    def download(cls,url, type, user_agent='classproject', proxy=None, num_retries=2):
        '''
        type: 0=html, 1=json, 2=music
        '''
        """Download function with support for proxies"""
        print 'Downloading:', url
        headers = {'User-agent': user_agent}
        request = urllib2.Request(url, headers=headers)
        opener = urllib2.build_opener()
        if proxy:
            proxy_params = {urlparse.urlparse(url).scheme: proxy}
            opener.add_handler(urllib2.ProxyHandler(proxy_params))
        try:
            html = opener.open(request).read()
        except urllib2.URLError as e:
            print 'Download error:', e.reason
            html = None
            if num_retries > 0:
                if hasattr(e, 'code') and 500 <= e.code < 600:
                    # retry 5XX HTTP errors
                    html = cls.download(url, user_agent, proxy, num_retries - 1)
        if(type==1):
            json_obj = json.loads(html)
            html = json.dumps(json_obj, ensure_ascii=False).encode('utf-8')
        return html
    '''转码，得到MP3文件的下载地址
    '''
    @classmethod
    def song_id_to_url(cls,code):
        l10 = None
        l2 = int(code[0])
        l3 = code[1:]
        l4 = int(math.floor(len(l3) / l2))
        l5 = len(l3) % l2
        l6 = []
        l7 = 0
        while (l7 < l5):
            if l7 >= len(l6):
                l6.append('')
            l6[l7] = l3[(l4 + 1) * l7:(l4 + 1 + (l4 + 1) * l7)]
            l7 += 1
        l7 = l5
        while l7 < l2:
            if l7 >= len(l6):
                l6.append(l3[(l4 * (l7 - l5) + (l4 + 1) * l5):(l4 + (l4 * (l7 - l5) + (l4 + 1) * l5))])
            else:
                l6[l7] = l3[(l4 * (l7 - l5) + (l4 + 1) * l5):(l4 + (l4 * (l7 - l5) + (l4 + 1) * l5))]
            l7 += 1
        l8 = ''
        l7 = 0
        while l7 < len(l6[0]):
            l10 = 0
            while l10 < len(l6):
                if l7 < len(l6[l10]):
                    l8 = l8 + l6[l10][l7]
                l10 += 1
            l7 += 1
        l8 = urllib.unquote(l8)
        l9 = ''
        l7 = 0
        while l7 < len(l8):
            if l8[l7] == '^':
                l9 += '0'
            else:
                l9 += l8[l7]
            l7 += 1
        l9 = l9.replace('+', ' ')
        return l9
    '''搜索当前url中所有的歌曲id
    '''
    @classmethod
    def search_id_list(cls,url):
        html = cls.download(url,type=0)
        rule = re.compile('onclick="play\(\'(.*?)\'')
        id_list = rule.findall(html)
        return id_list,html
    '''搜索当前html对应的歌曲的id
    '''
    @classmethod
    def search_id(cls,html):
        rule = re.compile('<div class="cd2play">(.*?)</div>', re.DOTALL)
        content = rule.findall(html)[0]
        rule = re.compile('onclick="play\(\'(.*?)\'')
        id = rule.findall(content)
        return id[0]

    @classmethod
    def fetch_one_song(cls,id,html):
        info_json=cls.download('http://www.xiami.com/song/playlist/id/'+id+'/object_name/default/object_id/0/cat/json',type=1)
        info=json.loads(info_json)
        title=info['data']['trackList'][0]['songName']
        singer_source=info['data']['trackList'][0]['singersSource']
        singer=[]
        for i in singer_source:
            tmp={}
            tmp["artistName"]=i['artistName']
            tmp["artistId"] = i['artistId']
            tmp["artistStringId"] = i['artistStringId']
            tmp["artistLogo"] = i['artistLogo']
            singer.append(tmp)
        songwriter=info['data']['trackList'][0]['songwriters']

        rule = re.compile('<div class="lrc_main">(.*?)</div>', re.DOTALL)
        lyrics=rule.findall(html)
        if len(lyrics)>0:
            lyrics=lyrics[0]
            lyrics = lyrics.replace('\n', '')
            lyrics = lyrics.replace('\r', '')
            lyrics = lyrics.replace('\t', '')
            lyrics = lyrics.replace('<br />', '\n')
        else:
            lyrics=''
        song_file=cls.download(cls.song_id_to_url(info['data']['trackList'][0]['location']),type=2)
        return lyrics,title,singer,songwriter,song_file,info_json

    @classmethod
    def save_song(cls,id,lyrics, title, singer , songwriter, song_file, info_json, html,dir='data'):

        song_meta={}
        song_meta['id']=id

        song_meta['lyrics']=lyrics


        song_meta['title']=title

        song_meta['singer']=singer

        song_meta['songwriter']=songwriter

        song_meta_json=json.dumps(song_meta)

        json_obj = json.loads(song_meta_json)
        song_meta_json = json.dumps(json_obj, ensure_ascii=False).encode('utf-8')


        #write meta data of song
        song_meta_output = open(dir+'/'+id+'.meta', 'w')
        song_meta_output.write(song_meta_json)
        song_meta_output.close()

        #write song's info json
        info_json_output = open(dir+'/'+id + '.json', 'w')
        info_json_output.write(info_json)
        info_json_output.close()

        #write mp3 file
        song_file_output = open(dir+'/'+id + '.mp3', 'wb')
        song_file_output.write(song_file)
        song_file_output.close()

        #write html file
        html_output = open(dir+'/'+id + '.html', 'w')
        html_output.write(html)
        html_output.close()
