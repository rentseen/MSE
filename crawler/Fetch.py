# -*- coding: utf-8 -*-
from Tools import Tools


queue=['1775504460']
record=[]
count=0
threshold=12000

record_output=open("data/songID.txt","w+")

while(len(queue)>0 and count<threshold):
    id=queue.pop()
    if id not in record:
        id_list, html = Tools.search_id_list('http://www.xiami.com/song/' + id)
        lyrics, title, singer, songwriter, song_file, info_json = Tools.fetch_one_song(id, html)

        record.append(id)
        count=count+1
        id_list.remove(id)
        if count+len(queue)<threshold:
            for i in id_list:
                if i not in record:
                    queue.append(i)
        Tools.save_song(id,lyrics,title,singer,songwriter,song_file,info_json,html)
        record_output.write(id)
        record_output.write('\n')
        print count


record_output.write(str(len(record)))
record_output.write('\n')
record_output.close()




