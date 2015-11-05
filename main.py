from Tkinter import *


class InstallAble():
    installCommonStr_const = "sudo apt-get install -y "

    def __init__(self, p_itemName, p_installPackageName, p_comment = ""):
        self.itemName = p_itemName
        self.commandLine = self.installCommonStr_const
        self.installPackageName = p_installPackageName
        self.comment = p_comment

    def getName(self):
        return self.itemName

    def getCommandLine(self):
        return self.commandLine + self.installPackageName

installItems = []
installItems.append(InstallAble("git", 'git'))
installItems.append(InstallAble("terminator", 'terminator'))
installItems.append(InstallAble("meld", 'meld'))
installItems.append(InstallAble("nodejs", 'nodejs', 'for jeklly'))
installItems.append(InstallAble("bundler", 'bundler', 'for jeklly'))
##
installItems.append(InstallAble("htop", 'htop'))
installItems.append(InstallAble("terminator", 'terminator'))

class MainWindow(Tk):


    def __init__(self, parent):
        Tk.__init__(self, parent)
        self.parent = parent
        self.vars = []
        self.initialize()



    def state(self):
        return map((lambda var: var.get()), self.vars)

    def var_states(self):
        global installItems
        for ii in range(len(installItems)):
            print(installItems[ii].getName)

    def initialize(self):
        self.grid()
        global installItems
        rows = 2
        for item in installItems:
            var = IntVar()
            chk = Checkbutton(self, text=item.getName(), variable=var).grid(row=rows, sticky=W)
            #chk.pack(side=side, anchor=anchor, expand=YES)
            self.vars.append(var)
            rows += 1

        rows += 1
        Button(self, text='Quit', command=quit).grid(row=rows, sticky=W, pady=4)
        rows += 1
        Button(self, text='Show', command=self.var_states).grid(row=rows, sticky=W, pady=4)

if __name__ == "__main__":
    app = MainWindow(None)
    app.title('my application')
    app.mainloop()



