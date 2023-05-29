import os
dir_path = r'src\\sites\\'
print("---HTML -> header file---")

for path in os.listdir(dir_path):
    if os.path.isfile(os.path.join(dir_path, path)):
#zmień na with

        fin = open(dir_path + path, "r") #file in a nie fin xd
        name = os.path.splitext(path)[0]
        print("converting: " + path)

        fout = open("include/sites/"+name+".h","w")
        fout.write('const char '+name+'[] PROGMEM = R"=====(\n')
        
        site_lines = ""
        for line in fin:
            site_lines += line# line.replace(" ", "")
        
      #  site_lines = site_lines.replace("\n", "")    
        fout.write(site_lines)
        
        fout.write('\n)=====";')

        fin.close()
        fout.close()
#print("------------------------")
