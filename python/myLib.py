#这个文件放一些自己觉得有用的库
#import os
#import time

#返回当前时刻年月日即可,格式为"2019mark02mark13",mark为分隔符号
def getCurrentTime(mark = ''):
        localtime = time.localtime(time.time())
        stryear = str(localtime.tm_year)
        strmonth = str(localtime.tm_mon)
        if localtime.tm_mon < 10:
                strmonth = "0" + strmonth
        strday = str(localtime.tm_mday)
        if localtime.tm_mday < 10:
                strday = "0" + strday
        #ans = stryear + strmonth + strday
        ans = stryear + mark + strmonth + mark + strday
        
        #print(ans)
        return ans
  
  #获取本路径下（或上一层路径下）的某个文件的完整路径
  def getExcelFileName(fileName, isUpperPath = False):
        #获取此文件的上一层文件路径,也就是版本进度文件所在路径
        if isUpperPath == True:
                fileUrl = os.path.split(os.getcwd())[0]
        else:
                fileUrl = os.getcwd()
        
        #print(fileUrl)
        
        for root, dirs, files in os.walk(fileUrl):
                #print(root)
                #print(dirs)
                #print(files)
                
                for file in files:
                        if file.find(fileName) >= 0:
                                return fileUrl + "\\" + file
                break #在第一回就停止
        
        return "0"
         
#使用当前时间的秒数，以避免重复,这个函数当时是用来生成图片名字的
def makePicName():
        strSeconds = str(time.time())
        #print(strSeconds)
        strPic = getCurrentTime() + "_" + strSeconds + ".PNG"
        return strPic
        
        

