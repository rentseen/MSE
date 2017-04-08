# -*- coding: utf-8 -*-
from Tools import Tools
import time

#parameters
queue=["1776120826","1774998332","1774112695","1773273264","1774781482","1770506605"]
record=Tools.load_record("data/songID.txt")
count=len(record)
threshold=20000

record_output=open("data/songID.txt","w+")

for id in record:
    record_output.write(id)
    record_output.write('\n')

while(len(queue)>0 and count<threshold):
    id=queue.pop(0)
    if id not in record:
        id_list, html = Tools.search_id_list('http://www.xiami.com/song/' + id)
        if html==None:
            time.sleep(100)
            continue
        lyrics, title, singer, songwriter, song_file, info_json = Tools.fetch_one_song(id, html)
        if lyrics==None:
            time.sleep(100)
            continue
        if len(lyrics)<5:
            continue
        record.append(id)
        count=count+1
        id_list.remove(id)
        if count+len(queue)<threshold:
            for i in range(len(id_list)/3):  #减少广度，增加深度
                if id_list[i] not in record:
                    queue.append(id_list[i])
        Tools.save_song(id,lyrics,title,singer,songwriter,song_file,info_json,html)
        record_output.write(id)
        record_output.write('\n')
        print count


record_output.close()
