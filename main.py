from tkinter import *
import os

Const_goto_path = 'CXY_GOTO_PATH'
repo_dir = 'repo'
class ExecuteCommandLine():
    def __init__(self):
        self.commandLine = ''

    def execute(self, commandLine):
        idx = commandLine.find(Const_goto_path)
        ret = -1
        if -1 != idx:
            ret = os.chdir(commandLine[idx+Const_goto_path.__len__()+1:])
        ret = os.system(commandLine)
        if 0 != ret:
            raise Exception("Command Line Error:  "+commandLine)


class InstallAble():
    installCommonStr_const = "sudo apt-get install -y "

    def __init__(self, name, PackageName, comment = ""\
                 , preCommand = ['']\
                 , postCommand = ['']):
        self.itemName = name
        self.installPackageName = PackageName
        self.comment = comment
        self.preCommand = preCommand
        self.postCommand = postCommand

    def getName(self):
        return self.itemName

    def getCommandLines(self):
        return [self.preCommand, self.installCommonStr_const + self.installPackageName, self.postCommand]

class ExecuteAble():
    def __init__(self, name, command, comment = ''):
        self.name = name
        self.commands = command
        self.comment = comment
    def getName(self):
        return self.name
    def getCommandLines(self):
        return self.commands



installItems = []
installItems.append(InstallAble("git",
                                'git'))
installItems.append(InstallAble("vim",
                                'vim'))

installItems.append(InstallAble("terminator",
                                'terminator'))
installItems.append(InstallAble("meld", 'meld'))

installItems.append(InstallAble("nodejs",
                                'nodejs',
                                'for jeklly',
                                ['sudo gem install jekyll\n']))
installItems.append(InstallAble("bundler",
                                'bundler',
                                'for jeklly'))
##
installItems.append(InstallAble("htop",
                                'htop'))
installItems.append(InstallAble("Java",
                                'oracle-java8-installer',
                                'install Oracle Java',
                                ['sudo add-apt-repository -y ppa:webupd8team/java\n'
                                ,'sudo apt-get update\n']))

executeItems = []
executeItems.append(ExecuteAble('set repohome',
                                ['mkdir ~/'+repo_dir+'\n'
                                ,'echo alias repohome=\"\'cd ~/'+repo_dir+'/\'\" >> ~/.bashrc']))
executeItems.append(ExecuteAble('install ypage',
                                [
                                'cd ~/'+repo_dir+' && git clone git@github.com:cuixiongyi/cuixiongyi.github.io.git\n'
                                ,'cd ~/'+repo_dir+'/cuixiongyi.github.io\n'
                                ,'sudo apt-get install nodejs\n'
                                #,'sudo gem install jekyll\n'
                                #,'sudo gem install bundler\n'
                                , 'sudo apt-get install -y zlib1g-dev\n'
                                ,'cd ~/'+repo_dir+'/cuixiongyi.github.io && '+'bundle install\n'
                                ,'echo alias ypage=\'repohome && cd cuixiongyi.github.io && bundle exec jekyll serve\' >> ~/.bashrc']))

executeItems.append(ExecuteAble('git setting',
                                 ['git config --global user.name \"Xiongyi Cui\"\n'
                                 ,'git config --global user.email cuixiongyi2013@gmail.com\n'
                                 ,'git config --global credential.helper \"cache --timeout=7200\"']))

class MainWindow(Tk):
    def __init__(self, parent):
        Tk.__init__(self, parent)
        self.parent = parent
        self.installVars = []
        self.executeVars = []
        self.initialize()


    def state(self):
        return map((lambda var: var.get()), self.installVars)

    def var_states(self):
        global installItems
        execCL = ExecuteCommandLine
        for ii in range(len(installItems)):
            tmp1 = self.installVars[ii].get()
            if 1 == tmp1:
                commandList = installItems[ii].getCommandLines()
                for str1 in commandList:
                    self.labelResult['text'] = str1
                    execCL.execute(execCL, commandLine = str1)
        for ii in range(len(executeItems)):
            tmp1 = self.executeVars[ii].get()
            if 1 == tmp1:
                commands = executeItems[ii].getCommandLines()
                for strtmp in commands:
                    self.labelResult['text'] = strtmp
                    execCL.execute(execCL, commandLine = strtmp)

    def initialize(self):
        self.grid()
        rowStart = 2
        global installItems
        rows = rowStart
        cols = 1
        a = ExecuteCommandLine()
        a.execute('echo \'# >>>>>>>>>>>>>>> CXY_Linux_setup_START\' >> ~/.bashrc')

        for item in installItems:
            var = IntVar()
            chk = Checkbutton(self, text=item.getName(), variable=var).grid(row=rows, sticky=W)
            #chk.pack(side=side, anchor=anchor, expand=YES)
            self.installVars.append(var)
            rows += 1

        rows += 1
        Button(self, text='Quit', command=quit).grid(row=rows, sticky=W, pady=4)
        rows += 1
        Button(self, text='Show', command=self.var_states).grid(row=rows, sticky=W, pady=4)
        rows += 1
        self.labelResult = Label(self, text='command')
        self.labelResult.grid(row=rows, sticky=W, pady=4)
        rows += 1

        global executeItems
        cols += 1
        rows = rowStart
        for item in executeItems:
            var = IntVar()
            chk = Checkbutton(self, text=item.getName(), variable=var)
            chk.grid(row=rows, column=cols, sticky=W)
            #chk.pack(side=LEFT, expand=YES)
            self.executeVars.append(var)
            rows += 1

if __name__ == "__main__":
    app = MainWindow(None)
    app.title('my application')
    app.mainloop()




