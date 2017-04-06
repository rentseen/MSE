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
    def download(cls,url, user_agent='wswp', proxy=None, num_retries=2):
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
        return html

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
    @classmethod
    def search_ids(cls,url):
        html = cls.download(url)
        rule = re.compile('<div class="cd2play">(.*?)</div>', re.DOTALL)
        content = rule.findall(html)[0]
        print content
        rule = re.compile('onclick="play\(\'(.*?)\'')
        ids = rule.findall(content)
        return ids

    @classmethod
    def fetch_one_song(cls,id):
        info_json=cls.download('http://www.xiami.com/song/playlist/id/'+id+'/object_name/default/object_id/0/cat/json')
        info=json.loads(info_json)
        singer=info['data']['trackList'][0]['singers']


if __name__ == '__main__':
    print Tools.search_ids("http://www.xiami.com/song/1774490672")
