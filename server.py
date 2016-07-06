# -*- coding: utf-8 -*-
import SocketServer
import time
import urllib2
import HTMLParser
from threading import Thread
from lxml import etree
from lxml.html import fromstring, tostring

node = {u'松山機場站':'007',u'中山國中站':'008',u'南京復興站':'009',u'忠孝復興站':'010',u'大安站':'011',
        u'科技大樓站':'012',u'六張犁站':'013',u'麟光站':'014',u'辛亥站':'015',u'萬芳醫院站':'016',
        u'萬芳社區站':'017',u'木柵站':'018',u'動物園站':'019',u'大直站':'021',u'劍南路站':'022',
        u'西湖站':'023',u'港墘站':'024',u'文德站':'025',u'內湖站':'26',u'大湖公園站':'027',
        u'葫洲站':'028',u'東湖站':'029',u'南港軟體園區站':'030',u'南港展覽館站':'031',
        u'小碧潭站':'032',u'新店站':'033',u'新店區公所站':'034',u'七張站':'035',u'大坪林站':'036',
        u'景美站':'037',u'萬隆站':'038',u'公館站':'039',u'台電大樓站':'040',u'古亭站':'041',
        u'中正紀念堂站':'042',u'小南門站':'043',u'頂溪站':'045',u'永安市場站':'046',u'景安站':'047',
        u'南勢角站':'048',u'台大醫院站':'050',u'台北車站':'051',u'中山站':'053',u'雙連站':'054',
        u'民權西路站':'055',u'圓山站':'056',u'劍潭站':'057',u'士林站':'058',u'芝山站':'059',
        u'明德站':'060',u'石牌站':'061',u'唭哩岸站':'062',u'奇岩站':'063',u'北投站':'064',
        u'新北投站':'065',u'復興崗站':'066',u'忠義站':'067',u'關渡站':'068',u'竹圍站':'069',
        u'紅樹林站':'070',u'淡水站':'071',u'頂埔站':'076',u'永寧站':'077',u'土城站':'078',
        u'海山站':'079',u'亞東醫院站':'080',u'府中站':'081',u'板橋站':'082',u'新埔站':'083',
        u'江子翠站':'084',u'龍山寺站':'085',u'西門站':'086',u'善導寺站':'088',u'忠孝新生站':'089',
        u'忠孝敦化站':'091',u'國父紀念館站':'092',u'市政府站':'093',u'永春站':'094',u'後山埤站':'095',
        u'昆陽站':'096',u'南港站':'097',u'象山站':'099',u'台北101/世貿站':'100',u'信義安和站':'101',
        u'大安森林公園站':'103',u'北門站':'105',u'台北小巨蛋站':'109',u'南京三民站':'110',
        u'松山站':'111',u'輔大站':'121',u'新莊站':'122',u'頭前庄站':'123',u'先嗇宮站':'124',
        u'三重站':'125',u'菜寮站':'126',u'台北橋站':'127',u'大橋頭站':'128',u'中山國小站':'130',
        u'行天宮站':'131',u'松江南京站':'132',u'東門站':'134',u'蘆洲站':'174',u'三民高中站':'175',
        u'徐匯中學站':'176',u'三和國中站':'177',u'三重國小站':'178',u'迴龍站':'179',u'丹鳳站':'180',
        u'可直達':'000'}

def get_url(start,dest):
    url = 'http://web.trtc.com.tw/c/2stainfo.asp?action=query&s1elect=%s&s2elect=%s' % (start,dest)
    urlweb = urllib2.urlopen(url)
    return urlweb

def parse(urlweb):
    textdecode=HTMLParser.HTMLParser()
    urldata=urlweb.read()
    ET=etree.HTML(urldata)
    tr=ET.xpath('//tr[2]/td/form/div')
    try:
        #------ time ----------------------------------------------------
        time = etree.tostring(tr[1][0][1])
        time=textdecode.unescape(time)
        _tag = u'乘車時間'
        t_value = time[time.find('"-1">')+len('"-1">')+2:time.find('</font>')-3]
        _time = _tag +' : '+ t_value
        #------ price ----------------------------------------------------
        price = etree.tostring(tr[0][0][1][0][0][0])
        price=textdecode.unescape(price)
        _tag = u'單程票價'
        p_value = price[price.find('">')+len('">'):price.find('</font>')-1]
        _price = _tag +' : '+ p_value        
        #------ route ----------------------------------------------------
        route = etree.tostring(tr[1][0][2])
        route=textdecode.unescape(route)
        _tag = u'轉乘資訊'
        beg = 0
        end = 0
        transfer = []
        while end != -1 and beg !=-1:
            beg = route.find('=>',end)+len('=>')+1
            end = route.find(u'轉乘',beg)
            if end != -1 and beg !=-1:
                transfer.append(route[beg:end])
            else:
                transfer.append(u'可直達')
                transfer.append(u'可直達')
                break
        _route = _tag +' : '

        i = 0
        for n in transfer:
            if n == u'可直達' and i>0:
                break
            else:
                _route = _route +'->'+ n +'\n\t   '
            i +=1
        #------ start -> destination ----------------------------------------------------
        _start = route[route.find('<font size="-1">')+len('<font size="-1">'):route.find(u'搭乘')]
        _dest = route[beg:route.find('</font>')]
        _station = _start +' -> '+ _dest
        info = _station+u'\n▪'+_time +u'分鐘\n▪'+ _price +u'元\n▪'+ _route
        app = _station+u'A▪'+_time +u'分鐘A▪'+ _price +u'元A▪'+ _route
        pxa = t_value+','+p_value+','+node.get(transfer[0])+','+node.get(transfer[1])
        TX = pxa+'QAQ'+info
        return TX
    except:
        pass

class service(SocketServer.BaseRequestHandler):    
    def handle(self):
        print "Client connected with ", self.client_address
        while True:
            self.request.settimeout(0.5)
            try:
                recv = self.request.recv(1024).strip() #receive the data            
                if recv == "exit":
                    break
                elif recv:
                    print "{}:{}".format(self.client_address,recv)
                    station=recv.split(',')
                    start = station[0][1:4]
                    dest = station[1][1:4]
                    urlweb = get_url(start,dest)
                    info = parse(urlweb)
                    print info
                    try:
                        self.request.sendall(info.encode('utf-8'))
                    except:
                        self.request.sendall('404 Not Found!')
            except:
                pass
            
            time.sleep(0.2)
        self.request.close()
        print "Client disconnected with", self.client_address

class ThreadedTCPServer(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
    pass

if __name__ == "__main__":
    t = ThreadedTCPServer(('',12345), service)
    try:
        t.serve_forever()
    except:
        t.shutdown()
        print "Server shut down"
        t.server_close()
        
