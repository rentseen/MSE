# -*- coding: utf-8 -*-
from Tools import Tools


queue=["2088114","1775713590","1792702528","1024665","1795444216","1775504460","1795531539"]
record=[]
count=0
threshold=12000

record_output=open("data/songID.txt","w+")

while(len(queue)>0 and count<threshold):
    id=queue.pop(0)
    if id not in record:
        id_list, html = Tools.search_id_list('http://www.xiami.com/song/' + id)
        lyrics, title, singer, songwriter, song_file, info_json = Tools.fetch_one_song(id, html)
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


record_output.write(str(len(record)))
record_output.write('\n')
record_output.close()




