import sys
# gen.py out "frame:key,frame:key,..." ; prefix mashes A 200..1450 every 50
K={'A':0x3FE,'B':0x3FD,'S':0x3F7,'R':0x3EF,'L':0x3DF,'U':0x3BF,'D':0x37F,'N':0x3FF}
ev=[(0,0x3FF)]
for f in range(200,1450,50): ev+=[(f,0x3FE),(f+4,0x3FF)]
for p in sys.argv[2].split(','):
    if p:
        f,k=p.split(':'); f=int(f); ev+=[(f,K[k]),(f+4,0x3FF)]
open(sys.argv[1],'w').write('\n'.join(f"{a},0x{b:03X}" for a,b in ev)+'\n')
