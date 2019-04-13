#!/usr/bin/python3
# encoding: utf-8
import subprocess, time
import threading
import queue
import os

policy = ["FIFO", "LRU"]
page_size = [512,4096,32768]
tlb_size = [32,128,512]
program = ["./test_program/heapsort", "./test_program/mergesort", "./test_program/quicksort" ]
ins_size = ["100", "1000", "10000"]
data=[]

class mythread (threading.Thread):
    def __init__(self, poli , page, tlb, pro, ins):
        threading.Thread.__init__(self)
        self.poli = poli
        self.page = page
        self.tlb = tlb
        self.pro = pro
        self.ins = ins
        #self.i = i

    def run(self):
        threadLimiter.acquire()

        sort = subprocess.Popen(['valgrind', '--tool=lackey', '--trace-mem=yes', str(self.pro), str(self.ins)]
                                , stderr=subprocess.STDOUT, stdout=subprocess.PIPE)

        main_pro = ['./valtlb379', '-p', str(self.poli), str(self.page), str(self.tlb)]
        result = subprocess.Popen(main_pro, stdin=sort.stdout, stdout=subprocess.PIPE)
        threadLock.acquire()
        
        total_handled = (int)(result.stdout.readline().decode("UTF-8").strip())
        total_misses = (int)(result.stdout.readline().decode("UTF-8").strip())
        total_hits = (int)(result.stdout.readline().decode("UTF-8").strip())
        misses_rate = '{:.6f}'.format(total_misses/total_handled)

        print(str(self.pro)+" ins_size:"+str(self.ins)+" "+str(self.poli)+" page_size:"+str(self.page)
        +" tlb_size:"+str(self.tlb)+" total_handled:"+str(total_handled)+" total_misses:"+str(total_misses)
        +" total_hits:"+str(total_hits)+" miss_rate:"+misses_rate)

        data.append(str(self.pro)+" "+str(self.ins)+" "+str(self.tlb)+" "+str(self.poli)+" "+str(self.page)+" "+str(misses_rate))

        threadLock.release()
        threadLimiter.release()




if __name__ == "__main__":
    threadLock = threading.Lock()
    threadLimiter = threading.BoundedSemaphore(12)

    for pro in program:
        for ins in ins_size:
            for tlb in tlb_size:
                for p in policy:
                    for page in page_size:
                        t = mythread(p,page,tlb, pro, ins)
                        t.start()
                        t.join()

    i = len(page_size)*len(tlb_size)*len(policy)
    j=0
    filedata={}
    misses_rate=[]
    for dat in data:
        j=j+1
        misses_rate.append(dat.split(" ")[-1])
        if j%i ==0:
            imgname=dat.split(" ")[0]+" "+dat.split(" ")[1]
            filedata.update({imgname:misses_rate})
            misses_rate=[]
    


    os.chdir("./gnuplot")
    for key,value in filedata.items():
        name=key.split(" ")[0].split("/")[2]+" with "+key.split(" ")[1]+" instance sizes"
        print(name)
        with open("./"+name+".dat","w") as f:
            print("MissRate F512 F4096 F32768 L512 L4096 L32768")
            f.write("MissRate F512 F4096 F32768 L512 L4096 L32768\n")
            for index,rate in enumerate(value):
                if index%(len(page_size)*len(policy))== 0:
                    print(str(tlb_size[index//(len(page_size)*len(policy))])+" ",end="")
                    f.write(str(tlb_size[index//(len(page_size)*len(policy))])+" ")
                print(rate+" ",end="")
                f.write(rate+" ")
                if (index+1)%(len(page_size)*len(policy))== 0:
                    print()
                    f.write("\n")

        
        with open("./"+name+".plt","w") as f:
            f.write("set term push\n")
            f.write("set term png\n")
            f.write("set output \""+name+".png\"\n")
            f.write("set title \""+name+"\"\n")
            f.write("set ylabel \"misses rataes\"\n")
            f.write("set auto x\n")
            f.write("set yrange [0:0.005]\n")
            f.write("set style data histogram\n")
            f.write("set key under nobox\n")
            f.write("set style histogram clustered gap 1 title offset 2,0.25\n")
            f.write("set style fill solid noborder\n")
            f.write("set boxwidth 0.95\n")
            f.write("unset xtics\n")
            f.write("set xtics nomirror font \",8\"\n")
            f.write("set xlabel offset 0,-2\n")
            f.write("set ytics font \",8\"\n")
            f.write("set grid y\n")
            f.write("set auto y\n")
            f.write("plot newhistogram \"FIFO\",'"+name+".dat'"+" using \"F512\":xtic(1) t col, '' u \"F4096\" t col, '' u \"F32768\" t col,")
            f.write("newhistogram \"LRU\",'"+name+".dat'"+" using \"L512\":xtic(1) t col, '' u \"L4096\" t col, '' u \"L32768\" t col\n")
            f.write("set output\n")
            f.write("set term pop\n")
        subprocess.Popen(['gnuplot',"./"+name+".plt"])
        print("----------------------------------")