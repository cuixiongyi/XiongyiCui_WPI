import math
import copy
glbMin = math.inf
si = []
glbSi = []
def evalArm(li, si):
    assert len(li) == len(si), "si li not the same size"
    m1 = 0
    m2 = 0
    idx = 0
    for i in range(len(si)):
        idx += li[i]*si[i]
        if idx < m1:
            m1 = idx
        if idx > m2:
            m2 = idx

    return math.fabs(m2-m1)

def incSi(sii):
    if 0 == sii:
        return -1
    elif -1 == sii:
        return 1
    elif 1 == sii:
        return 1

def forloop(li, si, idx):
    global glbMin
    global glbSi
    if idx == len(li)-1:
        for i in range(2):
            si[idx] = incSi(si[idx])
            minmax = evalArm(li, si)
            #print(si)
            #print(minmax)
            if minmax < glbMin:
                glbMin = minmax
                glbSi = copy.deepcopy(si)
                minmax = evalArm(li, si)

    else:
        for i in range(2):
            si[idx] = incSi(si[idx])
            for ii in range(idx+1, len(si)):
                si[ii] = 0
            forloop(li, si, idx+1)


def solve(li):
    si = [0] * len(li)
    forloop(li, si, 0)
    print(glbSi)
    print(glbMin)
    return glbMin

def main():
    idx = 0
    li = [1,1,1]
    solve(li)

if __name__ == '__main__':
    main()






